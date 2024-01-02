//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core0.c         //
//////////////////////////

#include "core0.h"

// XIP_BASE + FLASH_TARGET_OFFSET = "0x101FF000" <- This address need to be smaller than __flash_binary_end stored in RP2040-Decoder.elf.map
const uint8_t *CV_ARRAY_FLASH = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
uint64_t input_bit_buffer = 0;
uint16_t level_table[32] = {0};
absolute_time_t falling_edge_time, rising_edge_time;


// Function returns average of values deviating less than twice the standard deviation from original average
float two_std_dev(const float arr[], const uint32_t length) {
    // Calculate arithmetic average
    float sum = 0;
    for (uint32_t i = 0; i < length; ++i) {
        sum += arr[i];
    }
    const float x_avg = sum / (float)length;

    // Calculate sample variance and sampled standard deviation
    sum = 0;
    for (uint32_t i = 0; i < length; ++i) {
        sum += (arr[i] - x_avg) * (arr[i] - x_avg);
    }
    const float variance = sum / (float)(length-1);
    const float std_dev = sqrtf(variance);

    // Sum every element that is within two times the sampled standard deviation and compute new average
    sum = 0;
    uint32_t counter = 0;
    for (uint32_t i = 0; i < length; ++i) {
        const float diff = fabsf(arr[i] - x_avg);
        if ( diff < std_dev * 2.0f ){
            sum += arr[i];
            counter++;
        }
    }
    return sum / (float) counter;
}


// Measures constant adc offset and programs the offset into flash
void adc_offset_adjustment(const uint32_t n) {
    LOG(1, "adc_offset_adjustment");

    float offsets_fwd[n];
    float offsets_rev[n];
    // Set PWM level to 0 just to be sure
    pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
    pwm_set_gpio_level(MOTOR_REV_PIN, 0);
    busy_wait_ms(1000);

    // Measure n times in "reverse direction" (GPIO 28)
    adc_select_input(2);
    adc_run(true);
    for (uint32_t i = 0; i < n; i++) {
        offsets_rev[i] = (float) adc_fifo_get_blocking();
    }
    adc_run(false);
    adc_fifo_drain();
    busy_wait_ms(1000);

    // Measure n times in "forward direction" (GPIO 29)
    adc_select_input(3);
    adc_run(true);
    for (uint32_t i = 0; i < n; i++) {
        offsets_fwd[i] = (float) adc_fifo_get_blocking();
    }
    adc_run(false);
    adc_fifo_drain();

    // Find overall average discarding outliers in measurement
    const float offset_avg_fwd = two_std_dev(offsets_fwd, n);
    const float offset_avg_rev = two_std_dev(offsets_rev, n);
    const float overall_avg = (offset_avg_fwd + offset_avg_rev) / 2;
    const uint8_t offset = (uint8_t) roundf(overall_avg);

    LOG(1, "new adc offset CV[171] (%f): (uint8_t)%d\n", overall_avg, offset);

    // Create temporary array -> change CV 172 in temp array -> erase flash -> write temp array to flash
    uint8_t CV_ARRAY_TEMP[CV_ARRAY_SIZE];
    memcpy(CV_ARRAY_TEMP, CV_ARRAY_FLASH, sizeof(CV_ARRAY_TEMP));
    CV_ARRAY_TEMP[171] = offset;
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, CV_ARRAY_TEMP, FLASH_PAGE_SIZE * 2);
}


// Confirmation after writing or verifying a CV
// Works by creating a spike in current consumption, this is detected by the command station
void acknowledge() {
    const uint16_t max_lvl = _125M / (CV_ARRAY_FLASH[8] * 100 + 10000);
    pwm_set_gpio_level(MOTOR_FWD_PIN, max_lvl);
    busy_wait_ms(3);
    pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
    pwm_set_gpio_level(MOTOR_REV_PIN, max_lvl);
    busy_wait_ms(3);
    pwm_set_gpio_level(MOTOR_REV_PIN, 0);
}


// When bit matches sent byte -> acknowledge()
void verify_cv_bit(const uint16_t cv_address, const bool bit_val, const uint8_t bit_pos) {
    const uint8_t mask = 0b00000001;
    const bool res = ((CV_ARRAY_FLASH[cv_address] >> bit_pos) & mask) == bit_val;
    if (res) {
        acknowledge();
    }
}


// When byte matches sent byte -> acknowledge()
void verify_cv_byte(const uint16_t cv_address, const uint8_t cv_data) {
    if (CV_ARRAY_FLASH[cv_address] == cv_data) acknowledge();
}


// CV writing function first erases necessary amount of  blocks in flash and then rewrites to flash
void regular_cv_write(const uint16_t cv_index, const uint8_t cv_data) {
    uint8_t CV_ARRAY_TEMP[CV_ARRAY_SIZE];
    memcpy(CV_ARRAY_TEMP, CV_ARRAY_FLASH, sizeof(CV_ARRAY_TEMP));
    CV_ARRAY_TEMP[cv_index] = cv_data;
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, CV_ARRAY_TEMP, FLASH_PAGE_SIZE * 2);
    acknowledge();
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
                regular_cv_write(cv_index, cv_data);
            }
            break;
        case 6: //CV_7
            // Read only (CV_7 - Version no.)
            // ADC offset Adjustment (CV_7; Value = 7);
            if (cv_data == 7) {
                LOG(1, "trigger adc offset adjustment via cv7 => 7\n");
                adc_offset_adjustment(8192);
            }
            break;
        case 7: //CV_8
            // Read only (CV_8 - Manufacturer ID)
            // Reset all CVs to Default (CV_8; Value = 8)
            if (cv_data == 8) {
                LOG(1, "reset of flash triggered via cv8 => 8\n");
                flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
                flash_range_program(FLASH_TARGET_OFFSET, CV_ARRAY_DEFAULT, FLASH_PAGE_SIZE * 2);
            }
            break;
        // CV_17 must have a value between 11000000->(192dec) and 11100111->(231dec) inclusive
        // CV_17 == 192 && CV_18 == 0 -> Address = 0 is also not valid and therefore shall not be set
        case 16: //CV_17
            if ((cv_data < 192 || cv_data > 231) || (CV_ARRAY_FLASH[17] == 0 && cv_data == 192)) {
                break;
            }
            regular_cv_write(cv_index, cv_data);
            break;
        case 17: //CV_18
            if(CV_ARRAY_FLASH[16] == 192 && cv_data == 0){
                break;
            }
            regular_cv_write(cv_index, cv_data);
            break;
            // CV_31 & CV_32 Index high byte isn't implemented and shall not be set
        case 30:    //CV_31
        case 31:    //CV_32
            break;
            // Regular CV write is carried out
        default:
            regular_cv_write(cv_index, cv_data);
    }
}


// CV Programming function resets core1 and evaluates the type of programming instruction (e.g. write byte)
void program_mode(const uint8_t number_of_bytes, const uint8_t *const byte_array) {
    //First check for valid programming command ("address" 112-127)
    if (byte_array[number_of_bytes - 1] < 128 && byte_array[number_of_bytes - 1] > 111) {
        const uint8_t instruction_type_mask = 0b00001100;
        const uint8_t instruction_type = instruction_type_mask & byte_array[number_of_bytes - 1];
        const uint8_t cv_address_ms_bits_mask = 0b00000011;
        const uint16_t cv_address_ms_bits = cv_address_ms_bits_mask & byte_array[number_of_bytes - 1];
        const uint16_t cv_address = byte_array[number_of_bytes - 2] + (cv_address_ms_bits << 8);

        // Before accessing flash, timers and interrupts need to be disabled and core1 needs to be shut down.
        if (pid_control_timer.pool) {
            alarm_pool_destroy(pid_control_timer.pool);
        }
        if (speed_helper_timer.pool) {
            alarm_pool_destroy(speed_helper_timer.pool);
        }

        multicore_reset_core1();

        const uint32_t saved_interrupts = save_and_disable_interrupts();

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

        // Restore interrupts
        restore_interrupts(saved_interrupts);
        // TODO: verify core1 launch...
        multicore_launch_core1(core1_entry);
        // TODO: Check busy wait necessity
        busy_wait_ms(5);
    }
}


// Set outputs according to configuration (PWM on/off, PWM duty cycle/level, ...)
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


// Bitwise XOR for all Bytes -> when result is: "0000 0000" error check is passed.
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
        // Split data into 8Bit array
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


// Output initialization function - Configures pwm frequency and clock divider according to CV_111 - CV_171
void init_outputs() {
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
        }
        mask <<= 1;
    }
}

// Measures "base" pwm duty cycle needed to overcome friction
uint16_t measure_base_pwm(const bool direction, const uint8_t iterations) {
    uint gpio;
    if (direction) gpio = MOTOR_FWD_PIN;
    else gpio = MOTOR_REV_PIN;
    float level_arr[iterations];
    for (int i = 0; i < iterations; ++i) {
        const uint16_t max_level = _125M / (CV_ARRAY_FLASH[8] * 100 + 10000);
        uint16_t level = max_level / 20;
        LOG(3, "iteration %d: maxlevel %d level %d\n", i, max_level, level);
        float measurement;
        do {
            pwm_set_gpio_level(gpio, level);
            busy_wait_ms(30);
            level += max_level / 500;
            measurement = measure(CV_ARRAY_FLASH[60],
                                  CV_ARRAY_FLASH[61],
                                  CV_ARRAY_FLASH[62],
                                  CV_ARRAY_FLASH[63],
                                  direction);
            LOG(3, "level %d measurement %f loop-cond: %f\n", level, measurement, measurement - (float) CV_ARRAY_FLASH[171]);
            if (level > max_level) {
                // Abort measurement and write default value of 0 to flash
                LOG(1, "measure_base_pwm: abort measurement and return 0\n");
                return 0;
            }
        } while ((measurement - (float) CV_ARRAY_FLASH[171]) < 5.0f);
        LOG(3, "level_arr[%d] = %d\n", i, level);
        level_arr[i] = (float) level;
        busy_wait_ms(100);
    }
    // Find and return overall average discarding outliers in measurement - multiply with 0.9
    const float retVal = 0.9f * two_std_dev(level_arr, 5);
    LOG(1, "measure_base_pwm: %d(%f)\n", (uint16_t) retVal, retVal);
    return (uint16_t) (retVal);
}


// Check for flash memory factory condition and adc offset setup
// When factory condition is found the CV_ARRAY_DEFAULT will be written into flash
// When ADC Setup is not configured run adc offset adjustment function and write adc offset into flash
void cv_setup_check() {
    // Check for flash factory setting and set CV_FLASH_ARRAY to default values when factory condition ("0xFF") is found.
    if (CV_ARRAY_FLASH[64] == 0xFF) {
        LOG(1, "found cv[64] equals to 0xff, reseting CVs (and CV[8] = 8)\n");
        const uint8_t arr[4] = {125, 8, 7, 124};
        program_mode(4, arr);        //reset to CV_ARRAY_DEFAULT (write CV_8 = 8)
    }

    // Check for adc offset setup
    if (CV_ARRAY_FLASH[171] == 0xFF) {
        LOG(1, "found cv[171] equals to 0xff, adc offset adjstments (and cr[7] = 7)\n");
        const uint8_t arr[4] = {125, 7, 6, 124};
        program_mode(4, arr); //ADC offset adjustment  (write CV_7 = 7)
    }

    // Check for base PWM configuration - used for feed-forward
    // Forward Direction
    if (get_16bit_CV(175) == 0) {
        LOG(1, "found cv[175] equals to 0, forward direction\n");
        const uint16_t base_pwm_fwd = measure_base_pwm(true, 10);
        const uint8_t base_pwm_fwd_high_byte = base_pwm_fwd >> 8;
        const uint8_t base_pwm_fwd_low_byte = base_pwm_fwd & 0x00FF;
        const uint8_t arr0[4] = {125, base_pwm_fwd_high_byte, 175, 124};
        program_mode(4, arr0);
        const uint8_t arr1[4] = {125, base_pwm_fwd_low_byte, 176, 124};
        program_mode(4, arr1);
    }
    // Reverse Direction
    if (get_16bit_CV(177) == 0) {
        LOG(1, "found cv[177] equals to 0, reverse direction\n");
        const uint16_t base_pwm_rev = measure_base_pwm(false, 10);
        const uint8_t base_pwm_rev_high_byte = base_pwm_rev >> 8;
        const uint8_t base_pwm_rev_low_byte = base_pwm_rev & 0x00FF;
        const uint8_t arr0[4] = {125, base_pwm_rev_high_byte, 177, 124};
        program_mode(4, arr0);
        const uint8_t arr1[4] = {125, base_pwm_rev_low_byte, 178, 124};
        program_mode(4, arr1);
    }
    LOG(1, "int_lim_max %d\n", CV_ARRAY_FLASH[51]);
    LOG(1, "int_lim_min %d\n", CV_ARRAY_FLASH[52]);
}


// Motor PWM initialization
void init_motor_pwm(const uint8_t gpio) {
    const uint16_t wrap_counter = (_125M / (CV_ARRAY_FLASH[8] * 100 + 10000)) - 1;
    const uint32_t slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_clkdiv_int_frac(slice_num, CV_ARRAY_FLASH[173], 0);
    pwm_set_wrap(slice_num, wrap_counter);
    pwm_set_gpio_level(gpio, 0);
    pwm_set_enabled(slice_num, true);

    LOG(2, "init motor(%d): wrapCounter %d clkdiv %d\n", gpio, wrap_counter, CV_ARRAY_FLASH[173]);
}


// Main initialization  -  GPIO Config / ADC Config
void init_gpio_adc() {
    if (gpio_get_function(MOTOR_FWD_PIN) != 4) {
        gpio_set_function(MOTOR_FWD_PIN, GPIO_FUNC_PWM);
        gpio_set_function(MOTOR_REV_PIN, GPIO_FUNC_PWM);
        adc_init();
        adc_gpio_init(FWD_V_EMF_ADC_PIN);
        adc_gpio_init(REV_V_EMF_ADC_PIN);
        adc_fifo_setup(true, false, 0, false, false);
    }
}


int main() {
    stdio_init_all();
    LOG(1, "\n\n======\ncore0 init\n");
    init_gpio_adc();
    LOG(1, "init motor pwms\n");
    init_motor_pwm(MOTOR_FWD_PIN);
    init_motor_pwm(MOTOR_REV_PIN);
    LOG(1, "check cvs\n");
    cv_setup_check();
    LOG(1, "init outputs\n");
    init_outputs();
    LOG(1, "init gpios\n");
    gpio_init(DCC_INPUT_PIN);
    gpio_set_dir(DCC_INPUT_PIN, GPIO_IN);
    gpio_pull_up(DCC_INPUT_PIN);
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_RISE, true, &track_signal_rise);
    LOG(1, "core0 done\n");
    busy_wait_ms(100);
    multicore_launch_core1(core1_entry);
    while (true);
}