//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core0.c         //
//////////////////////////

#include "core0.h"

// Define pointer to CV array stored in flash memory, FLASH_TARGET_OFFSET is defined in CMakeLists.txt and depending on flash size,
// it gets stored in the highest address flash sector possible, to avoid interference with the program code.
// CMakelists.txt also checks for program code exceeding past the last sector. Just have to make sure the correct flash size is set in CMakeLists.txt
const uint8_t *CV_ARRAY_FLASH = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);

// level_table is used to store pwm levels for any output using PWM
uint16_t level_table[sizeof(uint32_t)*8] = {0};

// Volatile due to modification in gpio interrupt context 
volatile absolute_time_t falling_edge_time, rising_edge_time;

// Input bit buffer for DCC signal
uint64_t input_bit_buffer = 0;
dcc_ring_buffer_t dcc_r_buf = {0};


static void increment_ring_buffer_idx(size_t * idx, uint8_t ring_buffer_length) {
    // Takes index variable as a pointer and increments it by 1. When index reaches the end of the ring buffer, it wraps around.
    // The wrap around value is the length of the ring buffer.
    *idx = (*idx + 1) % ring_buffer_length;
}

static size_t get_flash_size() {
    // Function to read the flash size of the chip, used for debugging/testing purposes
    // Prepare TX and RX buffers for reading the JEDEC ID
    uint8_t tx_buf[4] = { FLASH_CMD_READ_JEDEC_ID, 0, 0, 0 }; // Send the 0x9F command
    uint8_t rx_buf[4] = { 0, 0, 0, 0 }; // RX buffer for the response
    size_t buf_size = sizeof(tx_buf);
    void * params[] = {tx_buf, rx_buf, &buf_size};
    int ret_val = flash_safe_execute(call_flash_do_cmd, params, FLASH_TIMEOUT_IN_MS);
    if (ret_val != PICO_OK){
        set_error(FLASH_SIZE_READBACK_FAILURE);
        return 0;
    }

    // The third byte of the response (rxbuf[3]) contains the log2 size (e.g., 0x18 for 16 MB)
    if (rx_buf[3] == 0) {
        set_error(FLASH_SIZE_READBACK_FAILURE);
        return 0;
    }

    // Calculate the actual size (2^size_code bytes)
    size_t flash_size = (1u<<rx_buf[3]);
    
    // Return the flash size in bytes
    return flash_size;
}

void hardfault_handler(void) {
    // Custom HardFault handler
    // Log hardfault
    LOG(1, "HARDFAULT @ core%u!\n", get_core_num());
    LOG(1, "Rebooting...\n")

    // Hardfault can't be recovered, trigger the watchdog to reset the system
    watchdog_reboot(0, 0, 1);
}

static void trigger_hardfault(void) {
    // Force a hardfault, used for debugging/testing purposes
    __asm volatile("UDF #0"); // Undefined instruction
}

static void call_flash_range_erase(void *param) {
    // This function will be called when it's safe to call flash_range_erase
    // Always erases one sector, see FLASH_SECTOR_SIZE
    // Flash can only be erased in sectors, which effectively sets all bits to 1
    // Flash can than be programmed by setting bits to 0
    // Extract the parameters
    uint32_t offset = ((uintptr_t*)param)[0];
    uint32_t byte_count = ((uintptr_t*)param)[1];
    // Check for offset alignment (Must be aligned to a 4096-byte flash sector) 
    // Check byte count (Must be a multiple of 4096 bytes (one sector))
    if ((offset % FLASH_SECTOR_SIZE != 0) || (byte_count % FLASH_SECTOR_SIZE !=0)) {
        set_error(FLASH_SAFE_EXECUTE_PROGRAM_FAILURE);
        return;
    }
    flash_range_erase(offset, byte_count);
}

static void call_flash_range_program(void *param) {
    // This function will be called when it's safe to call flash_range_program
    // Extract the parameters
    uint32_t offset = ((uintptr_t*)param)[0];
    uint32_t byte_count = ((uintptr_t*)param)[1];
    const uint8_t *data = (const uint8_t *)((uintptr_t*)param)[2];
    // Check for offset alignment (Must be aligned to a 256-byte flash page)
    // Check byte count (Must be a multiple of 256 bytes (one page))
    if ((offset % FLASH_PAGE_SIZE != 0) || (byte_count % FLASH_PAGE_SIZE != 0)) {
        set_error(FLASH_SAFE_EXECUTE_PROGRAM_FAILURE);
        return;
    }
    flash_range_program(offset, data, byte_count);
}

static void call_flash_do_cmd(void *param) {
    // This function will be called when it's safe to call flash_do_cmd
    // Cast void pointer to void pointer array
    void **params = (void **)param;
    // Extract the parameters
    uint8_t * tx_buf = (uint8_t *)params[0];
    uint8_t * rx_buf = (uint8_t *)params[1];
    uint8_t * size_ptr = (uint8_t *)params[2];
    flash_do_cmd(tx_buf, rx_buf, *size_ptr);
}

unsigned int absolute_val(int x) {
    // Extract the sign bit
    int mask = x >> (sizeof(x) * 8 - 1); 
    return (x + mask) ^ mask;
}

static uint16_t two_std_dev(const uint16_t arr[], const uint32_t length) {
    // Calculate arithmetic average
    uint64_t sum = 0;
    for (uint32_t i = 0; i < length; ++i) {
        sum += arr[i];
    }
    const uint64_t x_avg = sum / length;

    // Calculate sample variance and sampled standard deviation
    sum = 0;
    for (uint32_t i = 0; i < length; ++i) {
        sum += (arr[i] - x_avg) * (arr[i] - x_avg);
    }
    const uint16_t variance = sum / (length-1);
    const uint16_t std_dev = (uint16_t) sqrtf((float) variance);

    // Sum every element that is within two times the sampled standard deviation and compute new average
    sum = 0;
    uint32_t counter = 0;
    for (uint32_t i = 0; i < length; ++i) {
        const uint32_t diff = absolute_val(arr[i] - x_avg);
        if ( diff <= 2*std_dev ){
            sum += arr[i];
            counter++;
        }
    }
    return sum / counter;
}

static void adc_offset_adjustment(uint32_t const n) {
    // Measures constant adc offset and programs the offset into flash
    LOG(1, "ADC offset adjustment with %u iterations...\n", n);
    
    uint16_t offsets_fwd[n];
    uint16_t offsets_rev[n];
    // Set PWM level to 0 just to be sure
    pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
    pwm_set_gpio_level(MOTOR_REV_PIN, 0);
    busy_wait_ms(1000);


    // Measure n times in (reverse direction)
    LOG(1, "Measuring ADC offset in reverse direction (input channel: %u)...\n", REV_V_EMF_ADC_CHANNEL);
    adc_select_input(REV_V_EMF_ADC_CHANNEL);
    adc_run(true);
    for (uint32_t i = 0; i < n; i++) {
        offsets_rev[i] = adc_fifo_get_blocking();
    }
    adc_run(false);
    adc_fifo_drain();
    LOG(1, "Done measuring in reverse direction...\n");
    busy_wait_ms(1000);


    // Measure n times in (forward direction)
    LOG(1, "Measuring ADC offset in forward direction (input channel: %u)...\n", FWD_V_EMF_ADC_CHANNEL);
    adc_select_input(FWD_V_EMF_ADC_CHANNEL);
    adc_run(true);
    for (uint32_t i = 0; i < n; i++) {
        offsets_fwd[i] = adc_fifo_get_blocking();
    }
    adc_run(false);
    adc_fifo_drain();
    LOG(1, "Done measuring in forward direction...\n");

    // Find overall average discarding outliers in measurement
    const uint16_t offset_avg_fwd = two_std_dev(offsets_fwd, n);
    const uint16_t offset_avg_rev = two_std_dev(offsets_rev, n);
    const uint16_t overall_avg_offset = (offset_avg_fwd + offset_avg_rev) / 2;

    LOG(1, "New ADC Offset value CV_172 = %u\n", overall_avg_offset);

    // Create temporary array -> change CV 172 in temp array -> erase flash -> write temp array to flash
    uint8_t CV_ARRAY_TEMP[CV_ARRAY_SIZE];
    memcpy(CV_ARRAY_TEMP, CV_ARRAY_FLASH, sizeof(CV_ARRAY_TEMP));
    CV_ARRAY_TEMP[171] = overall_avg_offset;
    uintptr_t params_erase[] = {FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE};
    int ret_val = flash_safe_execute(call_flash_range_erase, params_erase, FLASH_TIMEOUT_IN_MS);
    if (ret_val != PICO_OK){
        set_error(FLASH_SAFE_EXECUTE_ERASE_FAILURE);
        return;
    }
    uintptr_t params_program[] = {FLASH_TARGET_OFFSET, sizeof(CV_ARRAY_TEMP), (uintptr_t)CV_ARRAY_TEMP};
    ret_val = flash_safe_execute(call_flash_range_program, params_program, FLASH_TIMEOUT_IN_MS);
    if(ret_val != PICO_OK){
        set_error(FLASH_SAFE_EXECUTE_PROGRAM_FAILURE);
        return;
    }
}

static void acknowledge() {
    // Acknowledge a CV rd/wr instruction by pulsing the motor in both directions
    const uint16_t max_lvl = _125M / (CV_ARRAY_FLASH[8] * 100 + 10000);
    pwm_set_gpio_level(MOTOR_FWD_PIN, max_lvl);
    busy_wait_ms(3);
    pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
    pwm_set_gpio_level(MOTOR_REV_PIN, max_lvl);
    busy_wait_ms(3);
    pwm_set_gpio_level(MOTOR_REV_PIN, 0);
}

static void verify_cv_bit(uint16_t const cv_address, const bool bit_val, uint8_t const bit_pos) {
    // Check for matching bit, when found call acknowledge()
    const uint8_t mask = 0b00000001;
    const bool res = ((CV_ARRAY_FLASH[cv_address] >> bit_pos) & mask) == bit_val;
    if (res) {
        acknowledge();
    }
}

static void verify_cv_byte(uint16_t const cv_address, uint8_t const cv_data) {
    // Check for matching byte, when found call acknowledge()
    if (CV_ARRAY_FLASH[cv_address] == cv_data) acknowledge();
}

static void write_cv_byte(uint16_t cv_address, uint8_t cv_data) {
    // Write the specified CV byte to flash
    uint8_t CV_ARRAY_TEMP[CV_ARRAY_SIZE];
    memcpy(CV_ARRAY_TEMP, CV_ARRAY_FLASH, sizeof(CV_ARRAY_TEMP));
    CV_ARRAY_TEMP[cv_address] = cv_data;
    // First erase necessary amount of  blocks in flash and then rewrite to flash
    uintptr_t params_erase[] = {FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE};
    int ret_val = flash_safe_execute(call_flash_range_erase, params_erase, FLASH_TIMEOUT_IN_MS);
    if (ret_val != PICO_OK){
        set_error(FLASH_SAFE_EXECUTE_ERASE_FAILURE);
        return;
    }
    uintptr_t params_program[] = { FLASH_TARGET_OFFSET, sizeof(CV_ARRAY_TEMP), (uintptr_t)CV_ARRAY_TEMP};
    ret_val = flash_safe_execute(call_flash_range_program, params_program, FLASH_TIMEOUT_IN_MS);
    if(ret_val != PICO_OK){
        set_error(FLASH_SAFE_EXECUTE_PROGRAM_FAILURE);
        return;
    }
    acknowledge();
}

static void reset_cv_array_to_default(){
    // Reset all CVs to default (CV_ARRAY_DEFAULT)
    uintptr_t params_erase[] = {FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE};
    int ret_val = flash_safe_execute(call_flash_range_erase, params_erase, FLASH_TIMEOUT_IN_MS);
    if (ret_val != PICO_OK){
        set_error(FLASH_SAFE_EXECUTE_ERASE_FAILURE);
        return;
    }
    uintptr_t params_program[] = {FLASH_TARGET_OFFSET, sizeof(CV_ARRAY_DEFAULT), (uintptr_t)CV_ARRAY_DEFAULT};
    ret_val = flash_safe_execute(call_flash_range_program, params_program, FLASH_TIMEOUT_IN_MS);
    if(ret_val != PICO_OK){
        set_error(FLASH_SAFE_EXECUTE_PROGRAM_FAILURE);
        return;
    }
}

static void write_cv_handler(uint16_t const cv_index, uint8_t const cv_data) {
    // There are some exceptions to writing CVs - e.g. writing to a read only CV is not valid - this gets handled here
    switch (cv_index) {
        case 0: //CV_1
            // CV_1 => Value = 0 is not allowed
            if (cv_data == 0 || cv_data > 127) {
                break;
            }
            else {
                write_cv_byte(cv_index, cv_data);
            }
            break;
        case 6: //CV_7
            // Read only (CV_7 - Version no.)
            // ADC offset Adjustment is triggered when setting CV_7 = 7
            if (cv_data == 7) {
                LOG(1, "Triggered ADC offset adjustment via CV_7 = 7\n");
                adc_offset_adjustment(ADC_CALIBRATION_ITERATIONS);
            }
            break;
        case 7: //CV_8
            // Read only (CV_8 - Manufacturer ID)
            // Reset all CVs to default (CV_ARRAY_DEFAULT) when setting CV_8 = 8)
            if (cv_data == 8) {
                LOG(1, "reset of flash triggered via CV_8 = 8\n");
                reset_cv_array_to_default();
            }
            break;
        case 16: // CV_17
            // CV_17 must have a value between 11000000->(192dec) and 11100111->(231dec) inclusive
            // CV_17 == 192 && CV_18 == 0 -> Address = 0 is also not valid and therefore shall not be set
            if ((cv_data < 192 || cv_data > 231) || (CV_ARRAY_FLASH[17] == 0 && cv_data == 192)) {
                break;
            }
            write_cv_byte(cv_index, cv_data);
            break;
        case 17: // CV_18
            if(CV_ARRAY_FLASH[16] == 192 && cv_data == 0){
                break;
            }
            write_cv_byte(cv_index, cv_data);
            break;
        case 30: // CV_31 & CV_32 Index isn't implemented and shall not be set
            break;
        case 31: // CV_31 & CV_32 Index isn't implemented and shall not be set
            break;
        default: // Regular CV write is carried out
            write_cv_byte(cv_index, cv_data);
    }
}

static void program_mode(size_t const number_of_bytes, const uint8_t *const byte_array) {
    //First check for valid programming command ("address" 112-127)
    if (byte_array[number_of_bytes - 1] < 128 && byte_array[number_of_bytes - 1] > 111) {
        const uint8_t instruction_type_mask = 0b00001100;
        const uint8_t instruction_type = instruction_type_mask & byte_array[number_of_bytes - 1];
        const uint8_t cv_address_ms_bits_mask = 0b00000011;
        const uint16_t cv_address_ms_bits = cv_address_ms_bits_mask & byte_array[number_of_bytes - 1];
        const uint16_t cv_address = byte_array[number_of_bytes - 2] + (cv_address_ms_bits << 8);

        if (instruction_type == 0b000001000) {
            // Verify CV bit instruction
            const uint8_t bit_pos_mask = 0b00000111;
            const uint8_t bit_pos = bit_pos_mask & byte_array[number_of_bytes - 3];
            const uint8_t bit_val_mask = 0b00000001;
            const uint8_t bit_val_uint = (byte_array[number_of_bytes - 3] >> 3) & bit_val_mask;
            const bool bit_val = bit_val_uint;
            verify_cv_bit(cv_address, bit_val, bit_pos);
        }

        else if (instruction_type == 0b000000100) {
            // Verify CV byte instruction
            const uint8_t cv_data = byte_array[number_of_bytes - 3];
            verify_cv_byte(cv_address, cv_data);
        }

        else if (instruction_type == 0b000001100) {
            // Write CV byte instruction
            const uint8_t cv_data = byte_array[number_of_bytes - 3];
            write_cv_handler(cv_address, cv_data);
        }
        busy_wait_ms(5);
    }
}

static void set_outputs(uint32_t const functions_to_set_bitmask) {
    // Get outputs with pwm enabled and preset outputs_to_set_PWM variable with resulting GPIO Bitmask
    const uint32_t PWM_enabled_outputs = get_32bit_CV(111);
    uint32_t outputs_to_set_PWM = PWM_enabled_outputs;

    // Get enabled output configuration corresponding to set functions and direction
    uint32_t outputs_to_set = 0;
    uint32_t temp_mask = 1;
    for (uint8_t i = 0; i < 32; i++) {
        if (temp_mask & functions_to_set_bitmask) {
            const direction_t dir  = get_direction_of_speed_step(speed_step_target);
            outputs_to_set |= get_32bit_CV(i * 8 + 260 - 4 * dir);
        }
        temp_mask <<= 1;
    }
    outputs_to_set_PWM &= outputs_to_set;                       // Outputs with PWM to be enabled
    outputs_to_set &= ~outputs_to_set_PWM;                      // Outputs without PWM to be enabled

    // Set regular outputs without PWM, preventing illegal GPIO by masking with GPIO_ALLOWED_OUTPUTS
    gpio_put_masked(GPIO_ALLOWED_OUTPUTS, outputs_to_set);

    // Set PWM enabled outputs to desired level
    temp_mask = 1;
    for (uint8_t i = 0; i < 32; ++i) {
        if (temp_mask & outputs_to_set_PWM) {
            pwm_set_gpio_level(i, level_table[i]);
            pwm_gpio_to_channel(i);
            pwm_gpio_to_slice_num(i);
        }
        else if (temp_mask & PWM_enabled_outputs) {
            pwm_set_gpio_level(i, 0);
        }
        temp_mask <<= 1;
    }
}

static void update_active_functions(uint32_t new_function_bitmask, const uint8_t clr_bit_ind, bool const direction_change) {
    // Update functions F0 - F31 when function command is received or in case of direction change
    static uint32_t active_functions;
    static uint32_t prev_active_func;

    // Outputs depend on direction, therefore on direction change outputs need to be set for the other direction.
    if (direction_change) {
        // In case of a direction change every output needs to be set according to configuration for the other direction
        set_outputs(active_functions);
    }
    else {
        // In case of a change in active functions only the function group corresponding to the instruction is considered for updating the outputs
        // First, check for changes
        new_function_bitmask &= ~(clr_bit_arr[clr_bit_ind]);    // Clear every bit except bits corresponding to group of functions
        active_functions &= clr_bit_arr[clr_bit_ind];           // Clear bits corresponding to group of functions
        active_functions |= new_function_bitmask;               // Write bits corresponding to group of functions
        const uint32_t changes = active_functions ^ prev_active_func; // Checks for changes from previous state
        // When there is changes, set outputs accordingly
        if (changes) {
            set_outputs(active_functions);
        }
        // Update previously active function variable
        prev_active_func = active_functions;
    }
}

static bool error_detection(size_t const number_of_bytes, const uint8_t *const byte_array) {
    // Bitwise XOR for all Bytes -> when result is: "0b00000000" error check is passed.
    uint8_t xor_byte = 0;
    for (int i = 0; i < number_of_bytes; i++) {
        xor_byte = xor_byte ^ byte_array[i];
    }
    return (0 == xor_byte);
}

static bool is_long_address(size_t const number_of_bytes, const uint8_t *const byte_array) {
    // Check for long address. Function returns true for long address, false for short address.
    if ((byte_array[number_of_bytes - 1] >> 6) == 0b00000011) {
        return true;
    }
    return false;
}

static bool address_evaluation(size_t const number_of_bytes, const uint8_t *const byte_array) {
    // First checks for idle message and then evaluates address included in byte_array[] which contains the received DCC message
    // Check for idle message
    if (byte_array[number_of_bytes - 1] == 255) {
        return false;
    }

    // Long/extended address decoder configuration (CV_29 Bit5)
    const bool CV_29_bit5 = (CV_ARRAY_FLASH[28] & 0b00100000) >> 5;
    uint16_t read_address;
    // Check if decoder is configured to receive long/extended address (CV_29 Bit5 set)
    if (CV_29_bit5) {
        // Check DCC message for long address format
        if (is_long_address(number_of_bytes, byte_array)) {
            // DCC message received has long address format
            // start of transmission -> address_byte_1 -> address_byte_0 -> ... -> end of transmission
            const uint16_t address_byte_1 = (byte_array[number_of_bytes - 1]) - 192;  // Remove long address identifier bits
            const uint16_t address_byte_0 = (byte_array[number_of_bytes - 2]);
            read_address = (address_byte_1 << 8) + address_byte_0;
            const uint16_t long_address = get_16bit_CV(16) & 0x3FFF;      // &Mask to remove long address identifier bits
            return (long_address == read_address);
        }
        else {
            // DCC message received does not have long address format
            return false;
        }
    }
    else {
        // Else decoder is configured to receive short address (CV_29 Bit5 not set)
        // Short address message:
        // start of transmission ->  address_byte_0 -> ... -> end of transmission
        read_address = byte_array[number_of_bytes - 1];
        return CV_ARRAY_FLASH[0] == read_address;
    }
}

static void instruction_evaluation(size_t const number_of_bytes, const uint8_t *const byte_array) {
    // Evaluate the type of instruction
    // start of transmission -> ... -> command_byte_n -> ... -> command_byte_0 -> ... -> end of transmission
    // The position of command bytes depend on whether the address is long or not
    uint8_t command_byte_start_index;
    if (is_long_address(number_of_bytes, byte_array)) {
        command_byte_start_index = number_of_bytes - 3;
    }
    else {
        command_byte_start_index = number_of_bytes - 2;
    }
    const uint8_t command_byte_n = byte_array[command_byte_start_index];

    if (command_byte_n == 0b00111111) {
        // 0011-1111 (128 Speed Step Control) - 2 Byte length
        speed_step_target_prev = speed_step_target;
        speed_step_target = byte_array[command_byte_start_index - 1];
        // Check for direction change
        const bool direction_changed = get_direction_of_speed_step(speed_step_target) !=
                                       get_direction_of_speed_step(speed_step_target_prev);
        // In case of a direction change, functions need to be updated because functions depend on direction
        if (direction_changed) {
            update_active_functions(0, 0, true);
        }
    }

    else if (command_byte_n >> 6 == 0b00000010) {
        // 10XX-XXXX (Function Group Instruction)
        switch (command_byte_n >> 4) {
            case 0b00001011:    // F5-F8
                update_active_functions((command_byte_n << 5), 1, false);
                break;
            case 0b00001010:    // F9-F12
                update_active_functions((command_byte_n << 9), 2, false);
                break;
            default:            // F0-F4
                update_active_functions((((command_byte_n & 0b00001111) << 1) + ((command_byte_n & 0b00010000) >> 4)), 0, false);
                break;
        }
    }

    else if (command_byte_n >> 5 == 0b00000110) {
        // Feature Expansion Instruction 110X-XXXX
        switch (command_byte_n) {
            case 0b11011110: // F13-F20
                update_active_functions((byte_array[command_byte_start_index - 1] << 13), 3, false);
                break;
            case 0b11011111: // F21-F28
                update_active_functions((byte_array[command_byte_start_index - 1] << 21), 4, false);
                break;
            case 0b11011000: // F29-F31
                update_active_functions((byte_array[command_byte_start_index - 1] << 29), 5, false);
                break;
            default:
                break;
        }
    }
}

static bool reset_message_check(size_t const number_of_bytes, const uint8_t *const byte_array) {
    // Check for reset message - When reset message is found, stop the motor and disable all functions.
    if (byte_array[number_of_bytes - 1] == 0b00000000 && byte_array[number_of_bytes - 2] == 0b00000000) {
        const direction_t dir = get_direction_of_speed_step(speed_step_target);
        // Set previous speed step target to current speed step target
        speed_step_target_prev = speed_step_target;
        // Set speed step target to emergency stop speed step ( 1 for Reverse direction / 129 for forward direction)
        // for details see get_direction_of_speed_step() comment in "shared.h"
        if(dir == DIRECTION_FORWARD) speed_step_target = SPEED_STEP_FORWARD_EMERGENCY_STOP;
        else if(dir == DIRECTION_REVERSE) speed_step_target = SPEED_STEP_REVERSE_EMERGENCY_STOP;
        update_active_functions(0,0,0);  // Disables all functions
        return true;
    }
    return false;
}

static size_t detect_dcc_packet() {
    // Checks for valid package - looking for correct preamble using bitmasks
    // returns number of bytes if valid bit-pattern is found. Otherwise INVALID_PACKAGE is returned.
    // INVALID_PACKAGE is defined in core0.h as SIZE_MAX (maximum value of size_t)
    size_t number_of_bytes = INVALID_PACKAGE;
    const uint64_t masked_message_3_bytes = input_bit_buffer & MESSAGE_MASK_3_BYTES;
    if (masked_message_3_bytes == MESSAGE_3_BYTES) number_of_bytes = 3;
    const uint64_t masked_message_4_bytes = input_bit_buffer & MESSAGE_MASK_4_BYTES;
    if (masked_message_4_bytes == MESSAGE_4_BYTES) number_of_bytes = 4;
    const uint64_t masked_message_5_bytes = input_bit_buffer & MESSAGE_MASK_5_BYTES;
    if (masked_message_5_bytes == MESSAGE_5_BYTES) number_of_bytes = 5;
    return number_of_bytes;
}

static void bits_to_dcc_packet_data(dcc_packet_t * packet) {
    // Writes data in input_bit_buffer to dcc_packet_t struct instance
    //start of transmission -> byte_n(address byte) -> ... -> byte_0(error detection byte) -> end of transmission
    for (uint8_t i = 0; i < packet->length; i++) {
        packet->data[i] = input_bit_buffer >> (i * 9 + 1);
    }
}

static void evaluate_packet(dcc_packet_t * packet) {
    // DCC packet evaluation
    static bool reset_message_flag;
    // Check for errors
    if (error_detection(packet->length, packet->data)) {
        // Check for matching address
        if (address_evaluation(packet->length, packet->data)) {
            reset_message_flag = false;
            instruction_evaluation(packet->length, packet->data);
        }
        else if (reset_message_flag) {
            // When previous packet was a reset message -> enter program mode
            program_mode(packet->length, packet->data);
        }
        else {
            // Check for reset message and set flag when reset message is received
            reset_message_flag = reset_message_check(packet->length, packet->data);
        }
    }
}

static void gpio_irq_cb(uint gpio, uint32_t events) {
    absolute_time_t start_time = get_absolute_time();
    // Check for rising edge on DCC input pin
    if ((events & GPIO_IRQ_EDGE_RISE) && (gpio == DCC_INPUT_PIN)) {
        track_signal_rise(gpio, events);
    }
    // Check for falling edge on DCC input pin
    else if ((events & GPIO_IRQ_EDGE_FALL) && (gpio == DCC_INPUT_PIN)) {
        track_signal_fall(gpio, events);
    }
    absolute_time_t end_time = get_absolute_time();
    if (absolute_time_diff_us(start_time, end_time) > 20) {
        LOG(1, "Time gpio_irq_cb: %lld us\n", absolute_time_diff_us(start_time, end_time));
    }
    
}

static void track_signal_rise() {
    // DCC-Signal pin rising edge. Write current time to rising_edge_time
    rising_edge_time = get_absolute_time();
}

static void track_signal_fall() {
    // DCC-Signal pin falling edge. Write current time to falling_edge_time
    falling_edge_time = get_absolute_time();
    // Time difference is equal to the time the signal was in a logical high state
    const int64_t time_logical_high = absolute_time_diff_us(rising_edge_time, falling_edge_time);
    // When logical high was longer than 87us write 0 bit into buffer otherwise 1
    bool bit;
    if (time_logical_high > 87) bit = 0;
    else bit = 1;
    input_bit_buffer <<= 1;
    input_bit_buffer |= bit;
    // Check if input buffer contains a valid dcc packet
    // number_of_bytes contains the length of the packet when detected, otherwise -1
    dcc_r_buf.packets[dcc_r_buf.wr_idx].length = detect_dcc_packet();
    if (dcc_r_buf.packets[dcc_r_buf.wr_idx].length != INVALID_PACKAGE) {
        // Write data into ring buffer containing dcc_packet_t struct instances
        bits_to_dcc_packet_data(&dcc_r_buf.packets[dcc_r_buf.wr_idx]);
        // Increment write index of ring buffer
        increment_ring_buffer_idx(&dcc_r_buf.wr_idx, RING_BUFFER_PACKETS);
    }
}

static void init_outputs() {
    LOG(1, "Initializing Outputs...\n");
    gpio_init_mask(GPIO_ALLOWED_OUTPUTS);
    gpio_set_dir_out_masked(GPIO_ALLOWED_OUTPUTS);  // Note: This might also disable UART on GPIO0 & GPIO1
    const uint32_t PWM_enabled_outputs = get_32bit_CV(111) & GPIO_ALLOWED_OUTPUTS;
    uint32_t mask = 1;
    for (uint8_t i = 0; i < sizeof(uint32_t)*8; ++i) {
        if (PWM_enabled_outputs & mask) {
            const uint32_t slice = pwm_gpio_to_slice_num(i);
            const uint32_t channel = pwm_gpio_to_channel(i);   // Channel A = "0"; Channel B = "1"
            const uint16_t wrap = get_16bit_CV(115 + 7 * slice);
            const uint16_t level = get_16bit_CV(118 + 7 * slice + 2 * channel);
            const uint8_t clock_divider = CV_ARRAY_FLASH[117 + 7 * slice] + 1;
            gpio_set_function(i, GPIO_FUNC_PWM);
            pwm_set_wrap(slice, wrap);
            pwm_set_gpio_level(i, 0);
            level_table[i] = level;
            pwm_set_clkdiv_int_frac(slice, clock_divider, 0);
            pwm_set_enabled(slice, true);
            LOG(1, "Set GPIO pin: %u as PWM output; PWM slice: %u; PWM channel: %u; Duty cycle: %f; Clock divider: %u\n", i, slice, channel, (float) level/wrap, clock_divider);
        }
        mask <<= 1;
    }
}

static void cv_setup_check() {
    LOG(1, "Checking CV array for factory state of flash or missing ADC offset setup...\n");
    // Check for flash factory setting and set CV_FLASH_ARRAY to default values when factory condition ("0xFF") is found.
    if (CV_ARRAY_FLASH[64] == 0xFF) {
        LOG(1, "Detected flash memory factory condidition (CV_65 == %u), resetting all CVs to default values...\n", CV_ARRAY_FLASH[64]);
        reset_cv_array_to_default();
    }

    // Check for existing ADC offset setup
    if (CV_ARRAY_FLASH[171] == 0xFF) {
        LOG(1, "Detected ADC offset factory condidition (CV_172 == %u), running offset adjustment measurement function...\n", CV_ARRAY_FLASH[171]);
        adc_offset_adjustment(ADC_CALIBRATION_ITERATIONS);
    }
    LOG(1, "CV check done! Setting wait_for_cv_setup_check flag to false!\n");
    cv_setup_check_done = true;
}

static void init_motor_pwm(uint8_t const gpio) {
    // Motor PWM initialization
    LOG(2, "Initializing Motor PWM on GPIO pin: %u...\n", gpio);
    // Set GPIO pin to PWM functionality
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    // Set wrap counter value saved in CV
    const uint16_t wrap_counter = (_125M / (CV_ARRAY_FLASH[8] * 100 + 10000)) - 1;
    const uint32_t slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_clkdiv_int_frac(slice_num, CV_ARRAY_FLASH[173], 0);
    pwm_set_wrap(slice_num, wrap_counter);
    // Set initial level to 0
    pwm_set_gpio_level(gpio, 0);
    // Enable PWM
    pwm_set_enabled(slice_num, true);
    LOG(2, "Initialized motor PWM on pin: %u; wrap_counter %u; clkdiv %d;\n", gpio, wrap_counter, CV_ARRAY_FLASH[173]);
}

static void init_adc() {
    LOG(1, "Initializing ADC...\n")
    // Initialize ADC
    adc_init();
    // Set ADC GPIO pins to ADC functionality
    adc_gpio_init(FWD_V_EMF_ADC_PIN);
    adc_gpio_init(REV_V_EMF_ADC_PIN);
    // Configure ADC FIFO
    adc_fifo_setup(true, false, 0, false, false);
    LOG(1, "ADC initialization done!\n")
}

static void init_digital_input() {
    LOG(1, "Initializing DCC input pin...\n");
    gpio_init(DCC_INPUT_PIN);
    gpio_set_dir(DCC_INPUT_PIN, GPIO_IN);
    gpio_pull_up(DCC_INPUT_PIN);
    LOG(1, "DCC input pin initialization done!\n");
}

static void wait_for_stdio_input() {
    while (true) {
        //watchdog_update();
        // Send a message to the user asking them to acknowledge it by sending a reply
        LOG(1, "Send any character to continue.\n");
        // Wait for a character
        int received_char = getchar_timeout_us(1e6);
        if ((received_char != PICO_ERROR_TIMEOUT) && (received_char > 0)) {
            // If a character is received, exit the function
            LOG(1, "Character '%c' received. Continuing...\n", (char)received_char);
            return;
        }
        // If no character is received, retry...
    }
}

int main() {
    exception_set_exclusive_handler(HARDFAULT_EXCEPTION, hardfault_handler);

    #ifdef RP2040_DECODER_DEFAULT_LED_PIN
        // Initialize LED GPIO pin
        gpio_init(RP2040_DECODER_DEFAULT_LED_PIN);
        gpio_set_dir(RP2040_DECODER_DEFAULT_LED_PIN, GPIO_OUT);
        // Blink LED once for 200ms to indicate startup
        gpio_put(RP2040_DECODER_DEFAULT_LED_PIN, 1);
        busy_wait_ms(200);
        gpio_put(RP2040_DECODER_DEFAULT_LED_PIN, 0);
        busy_wait_ms(200);
    #endif
    
    // Initialize stdio when enabled
    #if (STDIO_UART_ENABLED || STDIO_USB_ENABLED)
        if(stdio_init_all() != true){
            set_error(STDIO_INIT_FAILURE);
        }
        // When LOG_WAIT == 1 and LOGLEVEL > 0 wait for user input before continuing. User input can be any character.
        #if (LOGLEVEL > 0 && LOG_WAIT == 1)
            wait_for_stdio_input();
            printf("%u", STDIO_UART_ENABLED);
        #endif
    #endif

    LOG(1, "core0 Initialization...\n");
    
    // Check for reboot by watchdog and set error when true
    if (watchdog_caused_reboot()) {
        LOG(1, "Reboot by watchdog detected!\n");
        set_error(REBOOT_BY_WATCHDOG);
    }

    // Initialize Motor PWM pins
    init_motor_pwm(MOTOR_FWD_PIN);
    init_motor_pwm(MOTOR_REV_PIN);

    // Initialize outputs
    init_outputs();

    // Initialize ADC
    init_adc();

    // Initialize digital inputs and track signal irq
    init_digital_input();
    
    LOG(1, "core0 initialization done!\n");

    // Launch core1
    multicore_launch_core1(core1_entry);

    // Wait for core1 to call flash_safe_execute_core_init() and set the flash_safe_execute_core_init_done flag
    LOG(1, "Waiting for flash_safe_execute_core_init_done flag...\n");
    while (!flash_safe_execute_core_init_done) {
        watchdog_update();
    }
    
    // Check CV array for factory state of flash or missing ADC offset setup
    cv_setup_check();
    
    // Register GPIO IRQ callback
    gpio_set_irq_callback(gpio_irq_cb);
    // Enable IRQ
    gpio_set_irq_enabled(DCC_INPUT_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    irq_set_enabled(IO_IRQ_BANK0, true);
    
    // Enable the watchdog, requiring the watchdog to be updated every WATCHDOG_TIMER_IN_MS milliseconds or the chip will reboot
    // second arg is pause on debug which means the watchdog will pause when stepping through code
    // Watchdog is updated by core0 and core1 whenever there is time and hopefully soon enough to prevent a reboot.
    watchdog_enable(WATCHDOG_TIMER_IN_MS, 1);
    
    // Endless loop
    while (true) {
        // Check for new messages in ring buffer
        if (dcc_r_buf.wr_idx != dcc_r_buf.rd_idx) {
            absolute_time_t start_time = get_absolute_time();
            evaluate_packet(&dcc_r_buf.packets[dcc_r_buf.rd_idx]);
            increment_ring_buffer_idx(&dcc_r_buf.rd_idx, RING_BUFFER_PACKETS);
            absolute_time_t end_time = get_absolute_time();
            if (absolute_time_diff_us(start_time, end_time) > 40) {
                LOG(1, "Time to evaluate message: %lld us\n", absolute_time_diff_us(start_time, end_time));
            }
        }
        else {
            watchdog_update();
        }
    }
}