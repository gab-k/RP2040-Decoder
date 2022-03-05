//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core0.c         //
//////////////////////////

#include "core0.h"
bool target_direction = true;   //Forward = true  -  Reverse = false
uint target_speed_step = 128;   //128 = Stop (Forward Direction)
const uint8_t *CV_ARRAY_FLASH = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
uint64_t last_bits = 0;
bool reset_package_flag = false;
absolute_time_t falling_edge_time,rising_edge_time;

uint16_t two_sigma(const uint16_t arr[], uint8_t length){
    //Calculate arithmetic average
    uint32_t sum = 0;
    for (uint i = 0; i < length; ++i) {
        sum += arr[i];
    }
    uint16_t x_avg = sum / length;
    //Calculate variance
    sum = 0;
    for (uint8_t i = 0; i < length; ++i) {
        sum += (arr[i]-x_avg)*(arr[i]-x_avg);
    }
    float variance = (float) sum/ (float)(length-1);
    //Calculate standard deviation
    uint16_t std_dev = (uint16_t)sqrtf(variance);
    //"Filter" every array element that deviates more than 2*(std_dev)
    sum = 0;
    uint8_t counter = 0;
    for (uint8_t i = 0; i < length; ++i) {
        uint16_t val = arr[i]-x_avg;
        if ( val < 2*std_dev || val > (0-2*std_dev) ){
            sum += arr[i];
            counter++;
        }
    }
    //Calculate and return "new" arithmetic average from "filtered" array
    return sum / counter;
}
void adc_offset_adjustment(uint8_t length){
    uint16_t offsets_fwd[length];
    uint16_t offsets_rev[length];
    //Set Motor PWM level to 0 just for good measure
    pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
    pwm_set_gpio_level(MOTOR_REV_PIN, 0);
    for (uint8_t i  = 0;  i < 100; i++) {
        offsets_fwd[i] = measure(true);
    }
    for (uint8_t i  = 0;  i < 100; i++) {
        offsets_rev[i] = measure(false);
    }
    uint8_t offsets_fwd_avg = two_sigma(offsets_fwd, length);
    uint8_t offsets_rev_avg = two_sigma(offsets_rev, length);
    uint8_t CV_ARRAY_TEMP[CV_ARRAY_SIZE];
    memcpy(CV_ARRAY_TEMP, CV_ARRAY_FLASH, sizeof(CV_ARRAY_TEMP));
    CV_ARRAY_TEMP[171] = offsets_fwd_avg;
    CV_ARRAY_TEMP[172] = offsets_rev_avg;
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, CV_ARRAY_TEMP, FLASH_PAGE_SIZE * 2);
    acknowledge();
}
uint16_t m_pwm_find_offset(uint16_t level, uint8_t step, uint8_t delay, uint32_t threshold, bool direction){
    uint8_t gpio = MOTOR_FWD_PIN * direction + MOTOR_REV_PIN * !direction;
    pwm_set_gpio_level(MOTOR_FWD_PIN,0);
    pwm_set_gpio_level(MOTOR_REV_PIN,0);
    while( measure(direction) < threshold ){
        pwm_set_gpio_level(gpio,level);
        busy_wait_ms(delay);
        level += step;
    }
    pwm_set_gpio_level(MOTOR_FWD_PIN,0);
    pwm_set_gpio_level(MOTOR_REV_PIN,0);
    return level;
}
void m_pwm_offset_adjustment(uint8_t length){
    uint16_t offsets_fwd[length];
    uint16_t offsets_rev[length];
    uint16_t max_lvl = _125M/(CV_ARRAY_FLASH[8]*100+10000);
    for (uint8_t i = 0; i < length; ++i) {
        offsets_fwd[i] = m_pwm_find_offset(0, max_lvl / 3000, 1, 30, true);
        busy_wait_ms(500);
        offsets_rev[i] = m_pwm_find_offset(0, max_lvl / 3000, 1, 30, false);
        busy_wait_ms(500);
    }
    uint16_t offsets_fwd_avg = two_sigma(offsets_fwd, length);
    uint16_t offsets_rev_avg = two_sigma(offsets_rev, length);
    uint8_t CV_ARRAY_TEMP[CV_ARRAY_SIZE];
    memcpy(CV_ARRAY_TEMP, CV_ARRAY_FLASH, sizeof(CV_ARRAY_TEMP));
    CV_ARRAY_TEMP[53] = offsets_fwd_avg;
    CV_ARRAY_TEMP[54] = offsets_fwd_avg>>8;
    CV_ARRAY_TEMP[55] = offsets_rev_avg;
    CV_ARRAY_TEMP[56] = offsets_rev_avg>>8;
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, CV_ARRAY_TEMP, FLASH_PAGE_SIZE * 2);
    acknowledge();
}
void acknowledge() {
    uint16_t max_lvl = _125M/(CV_ARRAY_FLASH[8]*100+10000);
    pwm_set_gpio_level(MOTOR_FWD_PIN, max_lvl);
    busy_wait_ms(3);
    pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
    pwm_set_gpio_level(MOTOR_REV_PIN, max_lvl);
    busy_wait_ms(3);
    pwm_set_gpio_level(MOTOR_REV_PIN, 0);
}
void verify_cv_bit(uint16_t cv_address,bool bit_val, uint8_t bit_pos) {
    uint8_t mask = 0b00000001;
    bool res = ( (CV_ARRAY_FLASH[cv_address] >> bit_pos) &mask ) == bit_val;
    if (res) {
        acknowledge();
    }
}
void verify_cv_byte(uint16_t cv_address, uint8_t cv_data){
    if (CV_ARRAY_FLASH[cv_address] == cv_data) acknowledge();
}
void write_cv_byte(uint16_t cv_index, uint8_t cv_data){
    //CV_7 & CV_8 are read-only
    //Reset all CVs to Default (CV_8; Value = 8)
    if(cv_index == 7 && cv_data == 8){
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
        flash_range_program(FLASH_TARGET_OFFSET, CV_ARRAY_DEFAULT, FLASH_PAGE_SIZE * 2);
        acknowledge();
    }
    //Motor PWM offset Adjustment (CV_7; Value = 7);
    else if(cv_index == 6 && cv_data == 7){
        m_pwm_offset_adjustment(CV_ARRAY_FLASH[57]);
    }
    //CV_1 = 0 is not permitted, therefore this is used to start the ADC offset adjustment routine
    else if( !( cv_index&cv_data) ){
        adc_offset_adjustment(CV_ARRAY_FLASH[173]);
    }
    else{
        uint8_t CV_ARRAY_TEMP[CV_ARRAY_SIZE];
        memcpy(CV_ARRAY_TEMP, CV_ARRAY_FLASH, sizeof(CV_ARRAY_TEMP));
        CV_ARRAY_TEMP[cv_index] = cv_data;
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
        flash_range_program(FLASH_TARGET_OFFSET, CV_ARRAY_TEMP, FLASH_PAGE_SIZE * 2);
        acknowledge();
    }
}
void program_mode(uint8_t number_of_bytes, const uint8_t byte_array[]){
    if( !((CV_ARRAY_FLASH[53])+(CV_ARRAY_FLASH[54]<<8)) || !((CV_ARRAY_FLASH[55])+(CV_ARRAY_FLASH[56]<<8)) ){
    }
    //Check for valid programming command ("address" 112-127)
    if (byte_array[number_of_bytes - 1]<128 && byte_array[number_of_bytes - 1]>111){
            uint8_t instruction_type_mask = 0b00001100;
            uint8_t instruction_type = instruction_type_mask & byte_array[number_of_bytes - 1];
            uint8_t cv_address_ms_bits_mask = 0b00000011;
            uint16_t cv_address_ms_bits = cv_address_ms_bits_mask & byte_array[number_of_bytes - 1];
            uint16_t cv_address = byte_array[number_of_bytes - 2] + (cv_address_ms_bits << 8);
            if(pid_control_timer.pool)alarm_pool_destroy(pid_control_timer.pool);
            if(speed_helper_timer.pool)alarm_pool_destroy(speed_helper_timer.pool);
            multicore_reset_core1();
            uint32_t saved_interrupts = save_and_disable_interrupts();
            if (instruction_type == 0b000001000) {
                uint8_t bit_pos_mask = 0b00000111;
                uint8_t bit_pos = bit_pos_mask&byte_array[number_of_bytes - 3];
                uint8_t bit_val_mask = 0b00000001;
                uint8_t bit_val_uint = (byte_array[number_of_bytes - 3]>>3) & bit_val_mask;
                bool bit_val = bit_val_uint;
                verify_cv_bit(cv_address, bit_val, bit_pos);
            }
            else if (instruction_type == 0b000000100) {
                uint8_t cv_data = byte_array[number_of_bytes - 3];
                verify_cv_byte(cv_address, cv_data);
            }
            else if (instruction_type == 0b000001100) {
                uint8_t cv_data = byte_array[number_of_bytes - 3];
                write_cv_byte(cv_address, cv_data);
            }
            restore_interrupts(saved_interrupts);
            multicore_launch_core1(core1_entry);
    }
}
uint32_t get_32bit_CV (uint16_t CV_start_index){
    uint8_t byte_0 = CV_ARRAY_FLASH[CV_start_index];
    uint8_t byte_1 = CV_ARRAY_FLASH[CV_start_index+1];
    uint8_t byte_2 = CV_ARRAY_FLASH[CV_start_index+2];
    uint8_t byte_3 = CV_ARRAY_FLASH[CV_start_index+3];
    return (byte_0) + (byte_1<<8) + (byte_2<<16) + (byte_3<<24);
}
uint16_t get_16bit_CV (uint16_t CV_start_index){
    uint8_t byte_0 = CV_ARRAY_FLASH[CV_start_index];
    uint8_t byte_1 = CV_ARRAY_FLASH[CV_start_index+1];
    return (byte_0) + (byte_1<<8);
}
void set_outputs(uint32_t active_functions) {
    uint32_t slice_mask = 0;
    uint32_t outputs_to_set = 0;
    uint32_t outputs_to_set_PWM = get_32bit_CV(111);
    for (uint8_t i = 0; i < 32; i++) {
        if ( (active_functions>>i) & 1 ){
            outputs_to_set |= get_32bit_CV(i * 8 + 260 - 4 * target_direction);
        }
    }
    outputs_to_set &= GPIO_ALLOWED_OUTPUTS;                     // Prevent illegal GPIO
    outputs_to_set_PWM &= outputs_to_set;                       // Outputs with PWM to be enabled
    outputs_to_set &= ~outputs_to_set_PWM;                      // Outputs without PWM to be enabled
    gpio_put_masked(GPIO_ALLOWED_OUTPUTS, outputs_to_set);
    uint32_t temp_mask = 1;
    for (uint8_t i = 0; i < 32; ++i) {
        if(temp_mask & outputs_to_set_PWM){
            uint32_t slice_num = pwm_gpio_to_slice_num(i);
            slice_mask |= (1<<slice_num);
        }
        temp_mask<<=1;
    }
    pwm_set_mask_enabled(slice_mask);
}
void update_active_functions(uint32_t function_byte, uint8_t clr_bit_ind) {
    static uint32_t active_functions;
    static uint32_t prev_active_func;
    function_byte &= ~(clr_bit_arr[clr_bit_ind]);
    active_functions &= clr_bit_arr[clr_bit_ind];
    active_functions |= function_byte;
    uint32_t changes = active_functions ^ prev_active_func;
    if(changes)set_outputs(active_functions);
    prev_active_func = active_functions;
}
bool error_detection(int8_t number_of_bytes, const uint8_t byte_array[]) {
    //Bitwise XOR for all Bytes -> Successful result is: "0000 0000"
    uint8_t xor_byte = 0;
    for (int i = 0; i < number_of_bytes; i++) {
        xor_byte = xor_byte ^ byte_array[i];
    }
    return (0 == xor_byte);
}
// Returns true for long address
bool is_long_address(uint8_t number_of_bytes, const uint8_t byte_array[]) {
    if ((byte_array[number_of_bytes - 1]>>6) == 0b00000011) return true;
    return false;
}
bool address_evaluation(uint8_t number_of_bytes,const uint8_t byte_array[]) {
    //Check for Idle Package
    if (byte_array[number_of_bytes - 1] == 255)
    {
        return false;
    }
    uint16_t read_address;
    //Long Address Package
    if (is_long_address(number_of_bytes, byte_array))
    {
        //start of transmission -> address_byte_1 -> address_byte_0 -> ... -> end of transmission
        uint16_t address_byte_1 = (byte_array[number_of_bytes - 1]) - 192;  //remove long address identifier bits
        uint16_t address_byte_0 = (byte_array[number_of_bytes - 2]);
        read_address = (address_byte_1 << 8) + address_byte_0;
        uint8_t long_address_mask = 0b00111111;
        uint16_t long_address = ((CV_ARRAY_FLASH[16]&long_address_mask)<<8)+CV_ARRAY_FLASH[17];
        return(long_address==read_address);
    }
    //Short Address Package
    else
    {
        //start of transmission ->  address_byte_0 -> ... -> end of transmission
        read_address = (byte_array[number_of_bytes - 1]);
        return (CV_ARRAY_FLASH[0] == read_address);
    }
}
void instruction_evaluation(uint8_t number_of_bytes,const uint8_t byte_array[]) {
    uint8_t command_byte_n;
    uint8_t command_byte_start_index;
    // start of transmission -> ... -> command_byte_n -> ... -> command_byte_0 -> ... -> end of transmission
    if (is_long_address(number_of_bytes,byte_array)) {
        command_byte_start_index = number_of_bytes - 3;
    }
    else {
        command_byte_start_index = number_of_bytes - 2;
    }
    command_byte_n = byte_array[command_byte_start_index];
    //0011-1111 (128 Speed Step Control) - 2 Byte length
    if (command_byte_n == 0b00111111){
        target_speed_step = byte_array[command_byte_start_index - 1];
        if(target_speed_step>127) {
            target_direction = true;        //Forward
        }
        else {
            target_direction = false;       //Reverse
        }
    }
    // 10XX-XXXX (Function Group Instruction)
    else if (command_byte_n >> 6 == 0b00000010){
        switch (command_byte_n >> 4) {
            case 0b00001011:    // F5-F8
                update_active_functions((command_byte_n << 5),1);
                break;
            case 0b00001010:    // F9-F12
                update_active_functions((command_byte_n << 9),2);
                break;
            default:            // F0-F4
                update_active_functions(((command_byte_n << 1) + (command_byte_n >> 4)),0);
                break;
        }
    }
    //Feature Expansion Instruction 110X-XXXX
    else if (command_byte_n >> 5 == 0b00000110) {
        switch (command_byte_n) {
            case 0b11011110: // F13-F20
                update_active_functions((byte_array[command_byte_start_index - 1] << 13),3);
                break;
            case 0b11011111: // F21-F28
                update_active_functions((byte_array[command_byte_start_index - 1] << 21),4);
                break;
            case 0b11011000: // F29-F31
                update_active_functions((byte_array[command_byte_start_index - 1] << 29),5);
                break;
            default:
                break;
        }
    }
}
bool reset_package_check(uint8_t number_of_bytes,const uint8_t byte_array[]){
    if (byte_array[number_of_bytes-1] == 0b00000000 && byte_array[number_of_bytes-2] == 0b00000000){
        target_speed_step = 1;
        //update_active_functions(0,0,37);
        return true;
    }
    else {
        return false;
    }
}
int8_t check_for_package() {  //returns number of bytes if valid bit-pattern is found. Otherwise -1 is returned.
    uint64_t package3Masked = last_bits & PACKAGE_MASK_3_BYTES;
    if (package3Masked == PACKAGE_3_BYTES) {
        return 3;
    }
    uint64_t package4Masked = last_bits & PACKAGE_MASK_4_BYTES;
    if (package4Masked == PACKAGE_4_BYTES) {
        return 4;
    }
    uint64_t package5Masked = last_bits & PACKAGE_MASK_5_BYTES;
    if (package5Masked == PACKAGE_5_BYTES) {
        return 5;
    }
    return -1;
}
void writeLastBit(bool bit) {
    last_bits <<= 1;
    last_bits |= bit;
}
//start of transmission -> byte_n(address byte) -> ... -> byte_0(error detection byte) -> end of transmission
void bits_to_byte_array(int8_t number_of_bytes,uint8_t byte_array[]) {
    for (uint8_t i = 0; i < number_of_bytes; i++) {
        byte_array[i] = last_bits >> (i * 9 + 1);
    }
}
void evaluation(){
    int8_t number_of_bytes = check_for_package();
    if (number_of_bytes != -1) {
        uint8_t byte_array[SIZE_BYTE_ARRAY] = {0};
        bits_to_byte_array(number_of_bytes,byte_array);
        if(byte_array[number_of_bytes-3]==239 && byte_array[number_of_bytes-4]==151){
        }
        if (error_detection(number_of_bytes,byte_array)) {
            if (address_evaluation(number_of_bytes,byte_array)) {
                reset_package_flag = false;
                instruction_evaluation(number_of_bytes,byte_array);
            }
            else if (reset_package_flag) {
                program_mode(number_of_bytes,byte_array);
            }
            else {
                reset_package_flag = reset_package_check(number_of_bytes,byte_array);
            }
        }
    }
}
void track_signal_rise(unsigned int gpio, long unsigned int events) {
    rising_edge_time = get_absolute_time();
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_RISE, false, &track_signal_rise);
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_FALL, true, &track_signal_fall);
}
void track_signal_fall(unsigned int gpio, long unsigned int events) {
    falling_edge_time = get_absolute_time();
    int64_t time_logical_high  = absolute_time_diff_us(rising_edge_time,falling_edge_time);
    if(time_logical_high > 87) {
        writeLastBit(0);
    }
    else {
        writeLastBit(1);
    }
    evaluation();
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_FALL, false, &track_signal_fall);
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_RISE, true, &track_signal_rise);
}
void init_outputs(){
    gpio_init_mask(GPIO_ALLOWED_OUTPUTS);
    gpio_set_dir_out_masked(GPIO_ALLOWED_OUTPUTS);
    uint32_t Outputs_with_PWM_enabled = get_32bit_CV(111);
    uint32_t slice, channel;
    uint16_t wrap, level;
    uint32_t temp_mask = 1;
    for (uint8_t i = 0; i < 32; ++i) {
        if (Outputs_with_PWM_enabled&temp_mask){
            slice = pwm_gpio_to_slice_num(i);
            channel = pwm_gpio_to_channel(i);   //0 for A, 1 for B ?
            wrap = get_16bit_CV(115 + slice*7);
            level = get_16bit_CV(118 + 7*slice+ 2*channel);
            gpio_set_function(i,GPIO_FUNC_PWM);
            pwm_set_wrap(slice,wrap);
            pwm_set_gpio_level(i,level);
            pwm_set_clkdiv_int_frac(slice,CV_ARRAY_FLASH[117+7*slice],0);
        }
        temp_mask<<=1;
    }
}
void cv_setup_check(){
    //Check for flash factory setting and set CV_FLASH_ARRAY to default values when factory condition is found.
    if (CV_ARRAY_FLASH[64]){
        uint8_t arr[4] = {125,8,7,114};
        program_mode(4,arr);        //reset to CV_ARRAY_DEFAULT (CV_8; Value = 8)
    }
    //Check for adc offset setup
    if ( (CV_ARRAY_FLASH[171] & CV_ARRAY_FLASH[172]) == 0xFF ){
        //adc offset routine...
        uint8_t arr[4] = {125,0,0,125};
        program_mode(4,arr);        //reset to CV_ARRAY_DEFAULT (CV_1; Value = 0)
    }
    //Check for motor duty cycle offset setup
    uint sum = (CV_ARRAY_FLASH[53]) + (CV_ARRAY_FLASH[54]) + (CV_ARRAY_FLASH[55]) + (CV_ARRAY_FLASH[56]);
    if( !sum && target_speed_step != 128 && target_speed_step) {
        uint8_t arr[4] = {125,7,6,124};
        program_mode(4,arr);        //Re-setup offset Adjustment (CV_7; Value = 7);
    }
}
int main() {
    stdio_init_all();
    printf("core0 init...\n");
    cv_setup_check();
    init_outputs();
    multicore_launch_core1(core1_entry);
    gpio_init(DCC_INPUT_PIN);
    gpio_set_dir(DCC_INPUT_PIN, GPIO_IN);
    gpio_pull_up(DCC_INPUT_PIN);
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_RISE, true, &track_signal_rise);
    printf("core0 done\n");
    while (1);
}