//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core0.c         //
//////////////////////////

#include "core0.h"
// XIP_BASE + FLASH_TARGET_OFFSET = "0x101FF000"
const uint8_t *CV_ARRAY_FLASH = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
uint64_t input_bit_buffer = 0;
uint16_t level_table[32] = {0};
absolute_time_t falling_edge_time,rising_edge_time;


// Function returns average of values deviating less than twice the standard deviation from original average
float two_std_dev(const float arr[], uint32_t length){
    // Calculate arithmetic average
    float sum = 0;
    for (uint32_t i = 0; i < length; ++i) {
        sum += arr[i];
    }
    float x_avg = sum / (float)length;

    // Calculate sample variance and sampled standard deviation
    sum = 0;
    for (uint32_t i = 0; i < length; ++i) {
        sum += (arr[i]-x_avg)*(arr[i]-x_avg);
    }
    float variance = sum / (float)(length-1);
    float std_dev = sqrtf(variance);

    // Sum every element that is within two times the sampled standard deviation and compute new average
    sum = 0;
    uint32_t counter = 0;
    for (uint32_t i = 0; i < length; ++i) {
        float diff = fabsf(arr[i]-x_avg);
        if ( diff < std_dev*2.0f ){
            sum += arr[i];
            counter++;
        }
    }
    return sum / (float)counter;
}


// Measures constant adc offset and programs the offset into flash
void adc_offset_adjustment(uint32_t n){
    float offsets_fwd[n];
    float offsets_rev[n];
    // Set PWM level to 0 just to be sure
    pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
    pwm_set_gpio_level(MOTOR_REV_PIN, 0);
    busy_wait_ms(1000);

    // Measure n times in "reverse direction" (GPIO 28)
    adc_select_input(2);
    adc_run(true);
    for (uint32_t i  = 0;  i < n; i++) {
        offsets_rev[i] = (float)adc_fifo_get_blocking();
    }
    adc_run(false);
    adc_fifo_drain();
    busy_wait_ms(1000);

    // Measure n times in "forward direction" (GPIO 29)
    adc_select_input(3);
    adc_run(true);
    for (uint32_t i  = 0;  i < n; i++) {
        offsets_fwd[i] = (float)adc_fifo_get_blocking();
    }
    adc_run(false);
    adc_fifo_drain();

    // Find overall average discarding outliers in measurement
    float offset_avg_fwd = two_std_dev(offsets_fwd, n);
    float offset_avg_rev = two_std_dev(offsets_rev, n);
    float overall_avg = (offset_avg_fwd+offset_avg_rev)/2;
    uint8_t offset = (uint8_t)roundf(overall_avg);

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
    uint16_t max_lvl = _125M/(CV_ARRAY_FLASH[8]*100+10000);
    pwm_set_gpio_level(MOTOR_FWD_PIN, max_lvl);
    busy_wait_ms(3);
    pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
    pwm_set_gpio_level(MOTOR_REV_PIN, max_lvl);
    busy_wait_ms(3);
    pwm_set_gpio_level(MOTOR_REV_PIN, 0);
}


// When bit matches sent byte -> acknowledge()
void verify_cv_bit(uint16_t cv_address,bool bit_val, uint8_t bit_pos) {
    uint8_t mask = 0b00000001;
    bool res = ( (CV_ARRAY_FLASH[cv_address] >> bit_pos) &mask ) == bit_val;
    if (res) {
        acknowledge();
    }
}


// When byte matches sent byte -> acknowledge()
void verify_cv_byte(uint16_t cv_address, uint8_t cv_data){
    if (CV_ARRAY_FLASH[cv_address] == cv_data) acknowledge();
}


// CV writing function first erases necessary amount of  blocks in flash and then rewrites to flash
void regular_cv_write(uint16_t cv_index, uint8_t cv_data){
    uint8_t CV_ARRAY_TEMP[CV_ARRAY_SIZE];
    memcpy(CV_ARRAY_TEMP, CV_ARRAY_FLASH, sizeof(CV_ARRAY_TEMP));
    CV_ARRAY_TEMP[cv_index] = cv_data;
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, CV_ARRAY_TEMP, FLASH_PAGE_SIZE * 2);
    acknowledge();
}


// There are some exceptions to writing CVs - e.g. writing to a read only CV is not valid - this gets handled here
void write_cv_handler(uint16_t cv_index, uint8_t cv_data){
    switch (cv_index) {
        case 0: //CV_1
            // CV_1 => Value = 0 is not allowed
            if (cv_data == 0 || cv_data > 127) break;
        case 6: //CV_7
            // Read only (CV_7 - Version no.)
            // ADC offset Adjustment (CV_7; Value = 7);
            if (cv_data == 7) adc_offset_adjustment(8192);
            break;
        case 7: //CV_8
            // Read only (CV_8 - Manufacturer ID)
            // Reset all CVs to Default (CV_8; Value = 8)
            if (cv_data == 8){
                flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
                flash_range_program(FLASH_TARGET_OFFSET, CV_ARRAY_DEFAULT, FLASH_PAGE_SIZE * 2);
            }
            break;
        // CV_17 must have a value between 11000000->(192dec) and 11100111->(231dec) inclusive
        // CV_17 == 192 && CV_18 == 0 -> Address = 0 is also not valid and therefore shall not be set
        case 16:    //CV_17
            if ( (cv_data < 192 || cv_data > 231) || (CV_ARRAY_FLASH[17] == 0 && cv_data == 192) )  break;
            else regular_cv_write(cv_index,cv_data);
            break;
        case 17:    //CV_18
            if(CV_ARRAY_FLASH[16] == 192 && cv_data == 0) break;
            else regular_cv_write(cv_index,cv_data);
            break;
        // CV_31 & CV_32 Index high byte isn't implemented and shall not be set
        case 30:    //CV_31
        case 31:    //CV_32
            break;
        // Regular CV write is carried out
        default:
            regular_cv_write(cv_index,cv_data);
    }
}


// CV Programming function resets core1 and evaluates the type of programming instruction (e.g. write byte)
void program_mode(uint8_t number_of_bytes, const uint8_t * const byte_array){
    //First check for valid programming command ("address" 112-127)
    if (byte_array[number_of_bytes - 1]<128 && byte_array[number_of_bytes - 1]>111){
            uint8_t instruction_type_mask = 0b00001100;
            uint8_t instruction_type = instruction_type_mask & byte_array[number_of_bytes - 1];
            uint8_t cv_address_ms_bits_mask = 0b00000011;
            uint16_t cv_address_ms_bits = cv_address_ms_bits_mask & byte_array[number_of_bytes - 1];
            uint16_t cv_address = byte_array[number_of_bytes - 2] + (cv_address_ms_bits << 8);

            // Before accessing flash, timers and interrupts need to be disabled and core1 needs to be shut down.
            if(pid_control_timer.pool)alarm_pool_destroy(pid_control_timer.pool);
            if(speed_helper_timer.pool)alarm_pool_destroy(speed_helper_timer.pool);
            multicore_reset_core1();
            uint32_t saved_interrupts = save_and_disable_interrupts();

            // Verify CV bit instruction
            if (instruction_type == 0b000001000) {
                uint8_t bit_pos_mask = 0b00000111;
                uint8_t bit_pos = bit_pos_mask&byte_array[number_of_bytes - 3];
                uint8_t bit_val_mask = 0b00000001;
                uint8_t bit_val_uint = (byte_array[number_of_bytes - 3]>>3) & bit_val_mask;
                bool bit_val = bit_val_uint;
                verify_cv_bit(cv_address, bit_val, bit_pos);
            }

            // Verify CV byte instruction
            else if (instruction_type == 0b000000100) {
                uint8_t cv_data = byte_array[number_of_bytes - 3];
                verify_cv_byte(cv_address, cv_data);
            }

            // Write CV byte instruction
            else if (instruction_type == 0b000001100) {
                uint8_t cv_data = byte_array[number_of_bytes - 3];
                write_cv_handler(cv_address, cv_data);
            }

            // Restore interrupts
            restore_interrupts(saved_interrupts);
            busy_wait_ms(5);
    }
}


// Set outputs according to configuration (PWM on/off, PWM duty cycle/level, ...)
void set_outputs(uint32_t functions_to_set_bitmask) {
    // Get outputs with pwm enabled and preset outputs_to_set_PWM variable with resulting GPIO Bitmask
    uint32_t PWM_enabled_outputs = get_32bit_CV(111);
    uint32_t outputs_to_set_PWM = PWM_enabled_outputs;

    // Get enabled output configuration corresponding to set functions and direction
    uint32_t outputs_to_set = 0;
    uint32_t temp_mask = 1;
    for (uint8_t i = 0; i < 32; i++) {
        if (temp_mask & functions_to_set_bitmask ){
            outputs_to_set |= get_32bit_CV(i * 8 + 260 - 4 * get_direction(0));
        }
        temp_mask<<=1;
    }
    outputs_to_set_PWM &= outputs_to_set;                       // Outputs with PWM to be enabled
    outputs_to_set &= ~outputs_to_set_PWM;                      // Outputs without PWM to be enabled

    // Set regular outputs without PWM, preventing illegal GPIO by masking with GPIO_ALLOWED_OUTPUTS
    gpio_put_masked(GPIO_ALLOWED_OUTPUTS, outputs_to_set);

    // Set PWM enabled outputs to desired level
    temp_mask = 1;
    for (uint8_t i = 0; i < 32; ++i) {
        if(temp_mask & outputs_to_set_PWM){
            pwm_set_gpio_level(i, level_table[i]);
            pwm_gpio_to_channel(i);
            pwm_gpio_to_slice_num(i);
        }
        else if (temp_mask & PWM_enabled_outputs){
            pwm_set_gpio_level(i,0);
        }
        temp_mask<<=1;
    }
}


// Update functions F0 - F31 when function command is received or in case of direction change
void update_active_functions(uint32_t new_function_bitmask, uint8_t clr_bit_ind, bool direction_change) {
    static uint32_t active_functions;
    static uint32_t prev_active_func;

    // Outputs depend on direction
    // In case of a direction change every output needs to be set according to configuration for the other direction
    if (direction_change) set_outputs(active_functions);

    // In case of a change in active functions only the function group corresponding to the instruction is considered
    // Check for changes and set outputs accordingly
    else{
        new_function_bitmask &= ~(clr_bit_arr[clr_bit_ind]);    // Clear every bit except bits corresponding to group of functions
        active_functions &= clr_bit_arr[clr_bit_ind];           // Clear bits corresponding to group of functions
        active_functions |= new_function_bitmask;               // Write bits corresponding to group of functions
        uint32_t changes = active_functions ^ prev_active_func; // Checks for changes from previous state
        if(changes) set_outputs(active_functions);
        prev_active_func = active_functions;
    }
}


// Bitwise XOR for all Bytes -> Successful result is: "0000 0000"
bool error_detection(int8_t number_of_bytes, const uint8_t * const byte_array) {
    uint8_t xor_byte = 0;
    for (int i = 0; i < number_of_bytes; i++) {
        xor_byte = xor_byte ^ byte_array[i];
    }
    return (0 == xor_byte);
}


// Check for long address. Function returns true for long address.
bool is_long_address(uint8_t number_of_bytes, const uint8_t * const byte_array) {
    if ((byte_array[number_of_bytes - 1]>>6) == 0b00000011) return true;
    return false;
}


// First checks for idle package and then evaluates address included in byte_array[]
bool address_evaluation(uint8_t number_of_bytes,const uint8_t * const byte_array) {
    // Check for Idle Package
    if (byte_array[number_of_bytes - 1] == 255)
    {
        return false;
    }

    // Long/extended address decoder configuration (CV_29 Bit5)
    bool CV_29_bit5 = (CV_ARRAY_FLASH[28]&0b00100000)>>5;
    uint16_t read_address;
    // Decoder configured to receive long/extended address
    if (CV_29_bit5){
        // Package received does contain a long address -> Check for accordance
        if (is_long_address(number_of_bytes, byte_array))
        {
            // start of transmission -> address_byte_1 -> address_byte_0 -> ... -> end of transmission
            uint16_t address_byte_1 = (byte_array[number_of_bytes - 1]) - 192;  // Remove long address identifier bits
            uint16_t address_byte_0 = (byte_array[number_of_bytes - 2]);
            read_address = (address_byte_1 << 8) + address_byte_0;
            uint16_t long_address = get_16bit_CV(16) & 0x3FFF;      // &Mask to remove long address identifier bits
            return (long_address == read_address);
        }
        // Package received does not contain a long address
        else return false;
    }


    // Short address decoder configuration (CV_29 Bit5)
    else
    {
        // Short Address Package
        // start of transmission ->  address_byte_0 -> ... -> end of transmission
        read_address = (byte_array[number_of_bytes - 1]);
        return (CV_ARRAY_FLASH[0] == read_address);
    }
}


// Evaluate the type of instruction
void instruction_evaluation(uint8_t number_of_bytes,const uint8_t * const byte_array) {
    // start of transmission -> ... -> command_byte_n -> ... -> command_byte_0 -> ... -> end of transmission
    // The position of command bytes depend on whether the address is long or not
    uint8_t command_byte_n;
    uint8_t command_byte_start_index;
    if (is_long_address(number_of_bytes,byte_array)) {
        command_byte_start_index = number_of_bytes - 3;
    }
    else {
        command_byte_start_index = number_of_bytes - 2;
    }
    command_byte_n = byte_array[command_byte_start_index];

    //0011-1111 (128 Speed Step Control) - 2 Byte length
    if (command_byte_n == 0b00111111){
        static bool prev_dir;
        uint32_t speed_step = byte_array[command_byte_start_index - 1];
        multicore_fifo_push_blocking(speed_step);
        // In case of a direction change, functions need to be updated because functions might depend on direction state
        if (get_direction(0) != prev_dir) {
            update_active_functions(0,0,true);
            prev_dir = get_direction(0);
        }
    }

    // 10XX-XXXX (Function Group Instruction)
    else if (command_byte_n >> 6 == 0b00000010){
        switch (command_byte_n >> 4) {
            case 0b00001011:    // F5-F8
                update_active_functions((command_byte_n << 5),1,false);
                break;
            case 0b00001010:    // F9-F12
                update_active_functions((command_byte_n << 9),2,false);
                break;
            default:            // F0-F4
                update_active_functions(((command_byte_n << 1) + (command_byte_n >> 4)),0,false);
                break;
        }
    }

    //Feature Expansion Instruction 110X-XXXX
    else if (command_byte_n >> 5 == 0b00000110) {
        switch (command_byte_n) {
            case 0b11011110: // F13-F20
                update_active_functions((byte_array[command_byte_start_index - 1] << 13),3,false);
                break;
            case 0b11011111: // F21-F28
                update_active_functions((byte_array[command_byte_start_index - 1] << 21),4,false);
                break;
            case 0b11011000: // F29-F31
                update_active_functions((byte_array[command_byte_start_index - 1] << 29),5,false);
                break;
            default:
                break;
        }
    }
}


// Check for reset package - When reset package is found stop the motor and disable all functions
bool reset_package_check(uint8_t number_of_bytes,const uint8_t * const byte_array){
    if (byte_array[number_of_bytes-1] == 0b00000000 && byte_array[number_of_bytes-2] == 0b00000000){
        uint32_t speed_step = 1;
        multicore_fifo_push_blocking(speed_step);                           // Emergency Stop
        update_active_functions(0,0,0);  // Disables all functions
        return true;
    }
    else return false;
}


// Checks for valid package - looking for correct preamble
int8_t check_for_package() {  //returns number of bytes if valid bit-pattern is found. Otherwise -1 is returned.
    uint64_t package3Masked = input_bit_buffer & PACKAGE_MASK_3_BYTES;
    if (package3Masked == PACKAGE_3_BYTES) return 3;
    uint64_t package4Masked = input_bit_buffer & PACKAGE_MASK_4_BYTES;
    if (package4Masked == PACKAGE_4_BYTES) return 4;
    uint64_t package5Masked = input_bit_buffer & PACKAGE_MASK_5_BYTES;
    if (package5Masked == PACKAGE_5_BYTES) return 5;
    return -1;
}


//start of transmission -> byte_n(address byte) -> ... -> byte_0(error detection byte) -> end of transmission
void bits_to_byte_array(int8_t number_of_bytes,uint8_t byte_array[]) {
    for (uint8_t i = 0; i < number_of_bytes; i++) {
        byte_array[i] = input_bit_buffer >> (i * 9 + 1);
    }
}


// Sequence of bits gets evaluated
void evaluation(){
    static bool core1_launched = false;
    static bool reset_package_flag;
    int8_t number_of_bytes = check_for_package();
    // number_of_bytes contains the amount of bytes when the package is valid, otherwise -1
    if (number_of_bytes != -1) {
        // Split data into 8Bit array
        uint8_t byte_array[SIZE_BYTE_ARRAY] = {0};
        bits_to_byte_array(number_of_bytes,byte_array);
        // Check for errors
        if (error_detection(number_of_bytes,byte_array)) {
            // Check for matching address
            if (address_evaluation(number_of_bytes,byte_array)) {
                reset_package_flag = false;
                if (!core1_launched){
                    multicore_launch_core1(core1_entry);
                    // Wait for pid.direction pointer via multicore fifo also signalizing that core1 initialization is finished.
                    get_direction((bool *) multicore_fifo_pop_blocking());
                    core1_launched = true;
                }
                instruction_evaluation(number_of_bytes,byte_array);
            }
            // When previous packet was a reset package -> enter program mode
            else if (reset_package_flag) {
                program_mode(number_of_bytes,byte_array);
            }
            // Check for reset package and set flag when reset package is received
            else {
                reset_package_flag = reset_package_check(number_of_bytes,byte_array);
            }
        }
    }
}


// DCC-Signal (GPIO 21) rising edge interrupt
void track_signal_rise(unsigned int gpio, long unsigned int events) {
    // Save current timer value
    rising_edge_time = get_absolute_time();
    // Disable rising edge interrupt and enable falling edge interrupt
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_RISE, false, &track_signal_rise);
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_FALL, true, &track_signal_fall);
}


// DCC-Signal (GPIO 21) falling edge interrupt
void track_signal_fall(unsigned int gpio, long unsigned int events) {
    // Save current timer value
    falling_edge_time = get_absolute_time();
    // Time difference is equal to the time the signal was in a logical high state
    int64_t time_logical_high  = absolute_time_diff_us(rising_edge_time,falling_edge_time);
    // When logical high was longer than 87us write 0 bit into buffer otherwise 1
    bool bit;
    if(time_logical_high > 87) bit = 0;
    else bit = 1;
    input_bit_buffer <<= 1;
    input_bit_buffer |= bit;
    // evaluate sequence of bits saved in buffer
    evaluation();
    // Disable falling edge interrupt and enable rising edge interrupt
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_FALL, false, &track_signal_fall);
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_RISE, true, &track_signal_rise);
}


// Output initialization function - Configures pwm frequency and clock divider according to CV_111 - CV_171
void init_outputs(){
    gpio_init_mask(GPIO_ALLOWED_OUTPUTS);
    gpio_set_dir_out_masked(GPIO_ALLOWED_OUTPUTS);  // Note: This might also disable UART on GPIO0 & GPIO1
    uint32_t PWM_enabled_outputs = get_32bit_CV(111) & GPIO_ALLOWED_OUTPUTS;
    uint32_t slice, channel;
    uint16_t wrap, level;
    uint8_t clock_divider;
    uint32_t mask = 1;
    for (uint8_t i = 0; i < 32; ++i) {
        if (PWM_enabled_outputs & mask){
            slice = pwm_gpio_to_slice_num(i);
            channel = pwm_gpio_to_channel(i);   // Channel A = "0"; Channel B = "1"
            wrap = get_16bit_CV(115 + 7*slice);
            level = get_16bit_CV(118 + 7*slice + 2*channel);
            clock_divider = CV_ARRAY_FLASH[117+7*slice] + 1;
            gpio_set_function(i,GPIO_FUNC_PWM);
            pwm_set_wrap(slice,wrap);
            pwm_set_gpio_level(i,0);
            level_table[i] = level;
            pwm_set_clkdiv_int_frac(slice,clock_divider,0);
            pwm_set_enabled(slice,true);
        }
        mask<<=1;
    }
}


// Measures "base" pwm duty cycle needed to overcome friction
uint16_t measure_base_pwm(bool direction, uint8_t iterations){
    uint gpio;
    if (direction) gpio = MOTOR_FWD_PIN;
    else gpio = MOTOR_REV_PIN;
    float level_arr[iterations];
    for (int i = 0; i < iterations; ++i) {
        uint16_t max_level = (_125M/(CV_ARRAY_FLASH[8]*100+10000));
        uint16_t level = max_level/20;
        float measurement;
        do {
            pwm_set_gpio_level(gpio, level);
            busy_wait_ms(15);
            level += max_level/500;
            measurement = measure(CV_ARRAY_FLASH[60],
                                  CV_ARRAY_FLASH[61],
                                  CV_ARRAY_FLASH[62],
                                  CV_ARRAY_FLASH[63],
                                  direction);
        }
        while((measurement - (float)CV_ARRAY_FLASH[171]) < 5.0f);
        level_arr[i] = (float)level;
        busy_wait_ms(100);
    }
    // Find and return overall average discarding outliers in measurement - multiply with 0.9
    return (uint16_t)(0.9f*two_std_dev(level_arr,5));
}


// Check for flash memory factory condition and adc offset setup
// When factory condition is found the CV_ARRAY_DEFAULT will be written into flash
// When ADC Setup is not configured run adc offset adjustment function and write adc offset into flash
void cv_setup_check(){

    // Check for flash factory setting and set CV_FLASH_ARRAY to default values when factory condition is found.
    if ( CV_ARRAY_FLASH[64] ){
        uint8_t arr[4] = {125,8,7,124};
        program_mode(4,arr);        //reset to CV_ARRAY_DEFAULT (write CV_8 = 8)
    }

    // Check for adc offset setup
    if ( CV_ARRAY_FLASH[171]  == 0xFF ){
        uint8_t arr[4] = {125,7,6,124};
        program_mode(4,arr);        //ADC offset adjustment  (write CV_7 = 7)
    }

    // Check for base PWM configuration - used for feed-forward
    // Forward Direction
    if (get_16bit_CV(175) == 0){
        uint16_t base_pwm_fwd = measure_base_pwm(true,10);
        uint8_t base_pwm_fwd_high_byte = base_pwm_fwd>>8;
        uint8_t base_pwm_fwd_low_byte = base_pwm_fwd&0x00FF;
        uint8_t arr0[4] = {125,base_pwm_fwd_high_byte,175,124};
        program_mode(4,arr0);
        uint8_t arr1[4] = {125,base_pwm_fwd_low_byte,176,124};
        program_mode(4,arr1);
    }
    // Reverse Direction
    if (get_16bit_CV(177) == 0){
        uint16_t base_pwm_rev = measure_base_pwm(false,10);
        uint8_t base_pwm_rev_high_byte = base_pwm_rev>>8;
        uint8_t base_pwm_rev_low_byte = base_pwm_rev&0x00FF;
        uint8_t arr0[4] = {125,base_pwm_rev_high_byte,177,124};
        program_mode(4,arr0);
        uint8_t arr1[4] = {125,base_pwm_rev_low_byte,178,124};
        program_mode(4,arr1);
    }
}


// This function ensures that the direction pointer can only be written once.
// After first initialization this function can be called with arbitrary input to get the current direction
bool get_direction(bool *direction_ptr){
    static bool set_flag;
    static bool *dir_ptr;
    if(!set_flag) {
        dir_ptr = direction_ptr;
        set_flag = true;
    }
    return *dir_ptr;
}


// Motor PWM initialization
void init_motor_pwm(uint8_t gpio) {
    uint16_t wrap_counter = _125M/(CV_ARRAY_FLASH[8]*100+10000);    // 125MHz / f_pwm
    uint32_t slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_clkdiv_int_frac(slice_num,CV_ARRAY_FLASH[173],0);
    pwm_set_wrap(slice_num, wrap_counter);
    pwm_set_gpio_level(gpio,0);
    pwm_set_enabled(slice_num, true);
}


// Main initialization  -  GPIO Config / ADC Config
void init_main(){
    if(gpio_get_function(MOTOR_FWD_PIN) != 4){
        gpio_set_function(MOTOR_FWD_PIN, GPIO_FUNC_PWM);
        gpio_set_function(MOTOR_REV_PIN, GPIO_FUNC_PWM);
        adc_init();
        adc_gpio_init(FWD_V_EMF_ADC_PIN);
        adc_gpio_init(REV_V_EMF_ADC_PIN);
        adc_fifo_setup(true,false,0,false,false);
    }
}


int main() {
    stdio_init_all();
    printf("core0 init...\n");
    init_main();
    init_motor_pwm(MOTOR_FWD_PIN);
    init_motor_pwm(MOTOR_REV_PIN);
    cv_setup_check();
    init_outputs();
    gpio_init(DCC_INPUT_PIN);
    gpio_set_dir(DCC_INPUT_PIN, GPIO_IN);
    gpio_pull_up(DCC_INPUT_PIN);
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_RISE, true, &track_signal_rise);
    printf("core0 done\n");
    while (1);
}