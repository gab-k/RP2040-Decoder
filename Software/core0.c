//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core0.c         //
//////////////////////////

#include "core0.h"

// XIP_BASE + FLASH_TARGET_OFFSET = "0x101FF000" <- This address need to be smaller than __flash_binary_end stored in RP2040-Decoder.elf.map
const uint8_t *CV_ARRAY_FLASH = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
uint64_t input_bit_buffer = 1;
uint16_t level_table[32] = {0};
absolute_time_t falling_edge_time, rising_edge_time;


// Function to read the flash size of the chip
uint32_t get_flash_size() {
    // Prepare TX and RX buffers
    uint8_t txbuf[4] = { FLASH_CMD_READ_JEDEC_ID, 0, 0, 0 }; // Send the 0x9F command
    uint8_t rxbuf[4] = { 0, 0, 0, 0 }; // RX buffer for the response

    // Read the JEDEC ID
    flash_do_cmd(txbuf, rxbuf, sizeof(txbuf));

    // The third byte of the response (rxbuf[3]) contains the log2 size (e.g., 0x18 for 16 MB)
    if (rxbuf[3] == 0) {
        set_error(FLASH_SIZE_READBACK_FAILURE);
    }

    // Calculate the actual size (2^size_code bytes)
    uint32_t flash_size = (1u<<rxbuf[3]);

    // Return the flash size in bytes
    return flash_size;
}

// This function will be called when it's safe to call flash_range_erase
// Always erases one sector, see FLASH_SECTOR_SIZE
static void call_flash_range_erase(void *param) {
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

// This function will be called when it's safe to call flash_range_program
static void call_flash_range_program(void *param) {
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

unsigned int abs_val(int x) {
    // Vorzeichenbit extrahieren (falls das höchstwertige Bit 1 ist, ist die Zahl negativ)
    int mask = x >> (sizeof(x) * 8 - 1); // Rechtsverschiebung des Vorzeichens
    printf("%d\n", mask);
    return (x + mask) ^ mask;
}

uint16_t two_std_dev(const uint16_t arr[], const uint32_t length) {
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
        const uint32_t diff = abs_val(arr[i] - x_avg);
        if ( diff <= 2*std_dev ){
            sum += arr[i];
            counter++;
        }
    }
    return sum / counter;
}


// Measures constant adc offset and programs the offset into flash
// TODO: check necessity and correctness of adc offset function
void adc_offset_adjustment(const uint32_t n) {
    LOG(1, "ADC offset adjustment...\n");

    uint16_t offsets_fwd[n];
    uint16_t offsets_rev[n];
    // Set PWM level to 0 just to be sure
    pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
    pwm_set_gpio_level(MOTOR_REV_PIN, 0);
    busy_wait_ms(1000);

    // Measure n times in "reverse direction" (GPIO 28)
    adc_select_input(2); //TODO: "2" as input argument?
    adc_run(true);
    for (uint32_t i = 0; i < n; i++) {
        offsets_rev[i] = adc_fifo_get_blocking();
    }
    adc_run(false);
    adc_fifo_drain();
    busy_wait_ms(1000);

    // Measure n times in "forward direction" (GPIO 29)
    adc_select_input(3);
    adc_run(true);
    for (uint32_t i = 0; i < n; i++) {
        offsets_fwd[i] = adc_fifo_get_blocking();
    }
    adc_run(false);
    adc_fifo_drain();

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
    int ret_val = flash_safe_execute(call_flash_range_erase, params_erase, UINT32_MAX);
    if (ret_val != PICO_OK){
        set_error(FLASH_SAFE_EXECUTE_ERASE_FAILURE);
        return;
    }
    uintptr_t params_program[] = {FLASH_TARGET_OFFSET, sizeof(CV_ARRAY_TEMP), (uintptr_t)CV_ARRAY_TEMP};
    ret_val = flash_safe_execute(call_flash_range_program, params_program, UINT32_MAX);
    if(ret_val != PICO_OK){
        set_error(FLASH_SAFE_EXECUTE_PROGRAM_FAILURE);
        return;
    }
}


void acknowledge() {
    const uint16_t max_lvl = _125M / (CV_ARRAY_FLASH[8] * 100 + 10000);
    pwm_set_gpio_level(MOTOR_FWD_PIN, max_lvl);
    busy_wait_ms(3);
    pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
    pwm_set_gpio_level(MOTOR_REV_PIN, max_lvl);
    busy_wait_ms(3);
    pwm_set_gpio_level(MOTOR_REV_PIN, 0);
}


void verify_cv_bit(const uint16_t cv_address, const bool bit_val, const uint8_t bit_pos) {
    // Check for matching bit, when found call acknowledge()
    const uint8_t mask = 0b00000001;
    const bool res = ((CV_ARRAY_FLASH[cv_address] >> bit_pos) & mask) == bit_val;
    if (res) {
        acknowledge();
    }
}


void verify_cv_byte(const uint16_t cv_address, const uint8_t cv_data) {
    // Check for matching byte, when found call acknowledge()
    if (CV_ARRAY_FLASH[cv_address] == cv_data) acknowledge();
}


void write_cv_byte(uint16_t cv_address, uint8_t cv_data) {
    uint8_t CV_ARRAY_TEMP[CV_ARRAY_SIZE];
    memcpy(CV_ARRAY_TEMP, CV_ARRAY_FLASH, sizeof(CV_ARRAY_TEMP));
    CV_ARRAY_TEMP[cv_address] = cv_data;
    // First erase necessary amount of  blocks in flash and then rewrite to flash
    uintptr_t params_erase[] = {FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE};
    int ret_val = flash_safe_execute(call_flash_range_erase, params_erase, UINT32_MAX);
    if (ret_val != PICO_OK){
        set_error(FLASH_SAFE_EXECUTE_ERASE_FAILURE);
        return;
    }
    uintptr_t params_program[] = { FLASH_TARGET_OFFSET, sizeof(CV_ARRAY_TEMP), (uintptr_t)CV_ARRAY_TEMP};
    ret_val = flash_safe_execute(call_flash_range_program, params_program, UINT32_MAX);
    if(ret_val != PICO_OK){
        set_error(FLASH_SAFE_EXECUTE_PROGRAM_FAILURE);
        return;
    }
    acknowledge();
}

void reset_cv_array_to_default(){
    uintptr_t params_erase[] = {FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE};
    int ret_val = flash_safe_execute(call_flash_range_erase, params_erase, UINT32_MAX);
    if (ret_val != PICO_OK){
        set_error(FLASH_SAFE_EXECUTE_ERASE_FAILURE);
        return;
    }
    uintptr_t params_program[] = {FLASH_TARGET_OFFSET, sizeof(CV_ARRAY_DEFAULT), (uintptr_t)CV_ARRAY_DEFAULT};
    ret_val = flash_safe_execute(call_flash_range_program, params_program, UINT32_MAX);
    if(ret_val != PICO_OK){
        set_error(FLASH_SAFE_EXECUTE_PROGRAM_FAILURE);
        return;
    }
}

// There are some exceptions to writing CVs - e.g. writing to a read only CV is not valid - this gets handled here
void write_cv_handler(const uint16_t cv_index, const uint8_t cv_data) {
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
        // CV_17 must have a value between 11000000->(192dec) and 11100111->(231dec) inclusive
        // CV_17 == 192 && CV_18 == 0 -> Address = 0 is also not valid and therefore shall not be set
        case 16: //CV_17
            if ((cv_data < 192 || cv_data > 231) || (CV_ARRAY_FLASH[17] == 0 && cv_data == 192)) {
                break;
            }
            write_cv_byte(cv_index, cv_data);
            break;
        case 17: //CV_18
            if(CV_ARRAY_FLASH[16] == 192 && cv_data == 0){
                break;
            }
            write_cv_byte(cv_index, cv_data);
            break;
            // CV_31 & CV_32 Index high byte isn't implemented and shall not be set
        case 30:    //CV_31
        case 31:    //CV_32
            break;
            // Regular CV write is carried out
        default:
            write_cv_byte(cv_index, cv_data);
    }
}


void program_mode(const uint8_t number_of_bytes, const uint8_t *const byte_array) {
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


void set_outputs(const uint32_t functions_to_set_bitmask) {
    // Get outputs with pwm enabled and preset outputs_to_set_PWM variable with resulting GPIO Bitmask
    const uint32_t PWM_enabled_outputs = get_32bit_CV(111);
    uint32_t outputs_to_set_PWM = PWM_enabled_outputs;

    // Get enabled output configuration corresponding to set functions and direction
    uint32_t outputs_to_set = 0;
    uint32_t temp_mask = 1;
    for (uint8_t i = 0; i < 32; i++) {
        if (temp_mask & functions_to_set_bitmask) {
            const bool direction = get_direction_of_speed_step(speed_step_target);
            outputs_to_set |= get_32bit_CV(i * 8 + 260 - 4 * direction);
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


// Update functions F0 - F31 when function command is received or in case of direction change
void update_active_functions(uint32_t new_function_bitmask, const uint8_t clr_bit_ind, const bool direction_change) {
    static uint32_t active_functions;
    static uint32_t prev_active_func;

    // Outputs depend on direction, therefore on direction change outputs need to be set as configured for the other direction.
    if (direction_change) {
        // In case of a direction change every output needs to be set according to configuration for the other direction
        set_outputs(active_functions);
    }
    else {
        // TODO: improve explanation here...
        // In case of a change in active functions only the function group corresponding to the instruction is considered
        // Check for changes and set outputs accordingly
        new_function_bitmask &= ~(clr_bit_arr[clr_bit_ind]);    // Clear every bit except bits corresponding to group of functions
        active_functions &= clr_bit_arr[clr_bit_ind];           // Clear bits corresponding to group of functions
        active_functions |= new_function_bitmask;               // Write bits corresponding to group of functions
        const uint32_t changes = active_functions ^ prev_active_func; // Checks for changes from previous state
        if (changes) {
            set_outputs(active_functions);
        }
        prev_active_func = active_functions;
    }
}


// Bitwise XOR for all Bytes -> when result is: "0b00000000" error check is passed.
bool error_detection(const int8_t number_of_bytes, const uint8_t *const byte_array) {
    uint8_t xor_byte = 0;
    for (int i = 0; i < number_of_bytes; i++) {
        xor_byte = xor_byte ^ byte_array[i];
    }
    return (0 == xor_byte);
}


// Check for long address. Function returns true for long address.
bool is_long_address(uint8_t const number_of_bytes, const uint8_t *const byte_array) {
    if ((byte_array[number_of_bytes - 1] >> 6) == 0b00000011) {
        return true;
    }
    return false;
}


// First checks for idle message and then evaluates address included in byte_array[] which contains the received DCC message
bool address_evaluation(const uint8_t number_of_bytes, const uint8_t *const byte_array) {
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


// Evaluate the type of instruction
void instruction_evaluation(const uint8_t number_of_bytes, const uint8_t *const byte_array) {
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
        //Feature Expansion Instruction 110X-XXXX
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


// Check for reset message - When reset message is found, stop the motor and disable all functions.
bool reset_message_check(const uint8_t number_of_bytes, const uint8_t *const byte_array) {
    if (byte_array[number_of_bytes - 1] == 0b00000000 && byte_array[number_of_bytes - 2] == 0b00000000) {
        const bool direction = get_direction_of_speed_step(speed_step_target);
        // Set previous speed step target to current speed step target
        speed_step_target_prev = speed_step_target;
        // Set speed step target to emergency stop speed step ( 1 for Reverse direction / 129 for forward direction)
        // for details see get_direction_of_speed_step() doxygen comment in "shared.h"
        speed_step_target = 1 + 128*direction;
        update_active_functions(0,0,0);  // Disables all functions
        return true;
    }
    return false;
}


// Checks for valid message - looking for correct preamble using bitmasks
// returns number of bytes if valid bit-pattern is found. Otherwise -1 is returned.
int8_t verify_dcc_message() {
    int8_t number_of_bytes = -1;
    const uint64_t masked_message_3_bytes = input_bit_buffer & MESSAGE_MASK_3_BYTES;
    if (masked_message_3_bytes == MESSAGE_3_BYTES) number_of_bytes = 3;
    const uint64_t masked_message_4_bytes = input_bit_buffer & MESSAGE_MASK_4_BYTES;
    if (masked_message_4_bytes == MESSAGE_4_BYTES) number_of_bytes = 4;
    const uint64_t masked_message_5_bytes = input_bit_buffer & MESSAGE_MASK_5_BYTES;
    if (masked_message_5_bytes == MESSAGE_5_BYTES) number_of_bytes = 5;
    return number_of_bytes;
}


//start of transmission -> byte_n(address byte) -> ... -> byte_0(error detection byte) -> end of transmission
void bits_to_byte_array(const int8_t number_of_bytes, uint8_t byte_array[]) {
    for (uint8_t i = 0; i < number_of_bytes; i++) {
        byte_array[i] = input_bit_buffer >> (i * 9 + 1);
    }
}


// Sequence of bits gets evaluated
void evaluate_message() {
    static bool reset_message_flag;
    // Check if input buffer contains a valid dcc message
    const int8_t number_of_bytes = verify_dcc_message();
    // number_of_bytes contains the amount of bytes when the message is valid, otherwise -1
    if (number_of_bytes != -1) {
        // Split data into array of bytes
        uint8_t byte_array[SIZE_BYTE_ARRAY] = {0};
        bits_to_byte_array(number_of_bytes, byte_array);
        // Check for errors
        if (error_detection(number_of_bytes, byte_array)) {
            // Check for matching address
            if (address_evaluation(number_of_bytes, byte_array)) {
                reset_message_flag = false;
                instruction_evaluation(number_of_bytes, byte_array);
            }
            else if (reset_message_flag) {
                // When previous packet was a reset message -> enter program mode
                program_mode(number_of_bytes, byte_array);
            }
            else {
                // Check for reset message and set flag when reset message is received
                reset_message_flag = reset_message_check(number_of_bytes, byte_array);
            }
        }
    }
}


// DCC-Signal (GPIO 21) rising edge interrupt
void track_signal_rise(const unsigned int gpio, const long unsigned int events) {
    // Save current timer value
    rising_edge_time = get_absolute_time();
    // Disable rising edge interrupt and enable falling edge interrupt
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_RISE, false, &track_signal_rise);
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_FALL, true, &track_signal_fall);
}


// DCC-Signal (GPIO 21) falling edge interrupt
void track_signal_fall( const unsigned int gpio, const long unsigned int events) {
    // Save current timer value
    falling_edge_time = get_absolute_time();
    // Time difference is equal to the time the signal was in a logical high state
    const int64_t time_logical_high = absolute_time_diff_us(rising_edge_time, falling_edge_time);
    // When logical high was longer than 87us write 0 bit into buffer otherwise 1
    bool bit;
    if (time_logical_high > 87) bit = 0;
    else bit = 1;
    input_bit_buffer <<= 1;
    input_bit_buffer |= bit;
    // evaluate sequence of bits saved in buffer
    evaluate_message();
    // Disable falling edge interrupt and enable rising edge interrupt
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_FALL, false, &track_signal_fall);
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_RISE, true, &track_signal_rise);
}

void init_outputs() {
    LOG(1, "Initializing Outputs...\n");
    gpio_init_mask(GPIO_ALLOWED_OUTPUTS);
    gpio_set_dir_out_masked(GPIO_ALLOWED_OUTPUTS);  // Note: This might also disable UART on GPIO0 & GPIO1
    const uint32_t PWM_enabled_outputs = get_32bit_CV(111) & GPIO_ALLOWED_OUTPUTS;
    uint32_t mask = 1;
    for (uint8_t i = 0; i < 32; ++i) {
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

void cv_setup_check() {
    LOG(1, "Checking CV array for factory state of flash or missing ADC offset setup...\n");
    // Check for flash factory setting and set CV_FLASH_ARRAY to default values when factory condition ("0xFF") is found.
    if (CV_ARRAY_FLASH[64] == 0xFF) {
        LOG(1, "Detected flash memory factory condidition (CV_65 == 0xFF), resetting all CVs to default values...\n");
        reset_cv_array_to_default();
    }

    // Check for existing ADC offset setup
    if (CV_ARRAY_FLASH[171] == 0xFF) {
        LOG(1, "Detected ADC offset factory condidition (CV_172 == 0xFF), running offset adjustment measurement function...\n");
        adc_offset_adjustment(ADC_CALIBRATION_ITERATIONS);
    }
    LOG(1, "CV check done!\n");
}


// Motor PWM initialization
void init_motor_pwm(const uint8_t gpio) {
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

void init_adc() {
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

void init_digital_inputs() {
    LOG(1, "Initializing digital inputs and track signal irq...\n");
    gpio_init(DCC_INPUT_PIN);
    gpio_set_dir(DCC_INPUT_PIN, GPIO_IN);
    gpio_pull_up(DCC_INPUT_PIN);
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_RISE, true, &track_signal_rise);
    LOG(1, "Digital inputs and track signal irq initialization done!\n");
}

void wait_for_input() {
    while (true) {
        watchdog_update();
        // Send a message to the user asking them to acknowledge it by sending a reply
        LOG(1, "Send any character to continue.\n");
        // Wait for a character
        int received_char = getchar_timeout_us((WATCHDOG_TIMER_IN_MS/2)*1000);
        if ((received_char != PICO_ERROR_TIMEOUT) && (received_char > 0)) {
            // If a character is received, exit the function
            LOG(1, "Character '%c' received. Continuing...\n", (char)received_char);
            return;
        }
        // If no character is received, retry...
    }
}


int main() {
    // Initialize LED GPIO pin
    gpio_init(RP2040_DECODER_DEFAULT_LED_PIN);
    gpio_set_dir(RP2040_DECODER_DEFAULT_LED_PIN, GPIO_OUT);

    // Initialize stdio
    if(stdio_init_all() != true){
        set_error(STDIO_INIT_FAILURE);
    }
    stdio_flush();

    // Wait for user input before continuing, this is only enabled when LOG_WAIT is set to 1 and logging is enabled (LOGLEVEL > 0)
    // Also disabled when no STDIO is configured as the user would not be able to send anything
    # if (LOGLEVEL > 0 && LOG_WAIT == 1) && (STDIO_UART_ENABLED || STDIO_USB_ENABLED)
        wait_for_input();
        printf("%u", STDIO_UART_ENABLED);
    #endif

    LOG(1, "core0 Initialization...\n");

    // Launch core1
    multicore_launch_core1(core1_entry);

    // Wait 10ms to give core1 some time to call flash_safe_execute_core_init()
    busy_wait_ms(10);

    // Check for reboot by watchdog and set error when true
    if (watchdog_caused_reboot()) {
        set_error(REBOOT_BY_WATCHDOG);
    }

    // Enable the watchdog, requiring the watchdog to be updated every 2000ms or the chip will reboot
    // second arg is pause on debug which means the watchdog will pause when stepping through code
    watchdog_enable(WATCHDOG_TIMER_IN_MS, 1);

    // Check for error calling flash_safe_execute_core_init() on core1
    if(get_error_state() & FLASH_SAFE_EXECUTE_CORE_INIT_FAILURE){
        panic("Error calling flash_safe_execute_core_init() on core1!");
    }

    // Initialize Motor PWM pins
    init_motor_pwm(MOTOR_FWD_PIN);
    init_motor_pwm(MOTOR_REV_PIN);

    // Initialize outputs
    init_outputs();

    // Initialize ADC
    init_adc();

    // Check CV array for factory state of flash or missing ADC offset setup
    cv_setup_check();

    // Initialize digital inputs and track signal irq
    init_digital_inputs();
    
    LOG(1, "core0 initialization done!\n");
    
    // Endless loop
    while (true) {
        sleep_ms(400);
        watchdog_update();
    }
}