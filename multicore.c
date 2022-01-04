////////////////////////////////////
//        RP2040-Decoder          //
// created by Gabriel Koppenstein //
////////////////////////////////////

#include <stdio.h>
#include "string.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/flash.h"
#include "hardware/irq.h"
#include "CV.h"
#include "multicore.h"
#include "inttypes.h"
const uint8_t *CV_ARRAY_FLASH = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
bool active_functions[SIZE_ACTIVE_FUNCTIONS] = {false};
uint64_t last_bits = 0;
bool target_direction = true;
uint target_speed_step = 128;
uint current_v_emf_target = 300;
uint measurement = 0;
bool reset_package_flag = false;
typedef struct pid_params{
    float k_p;
    float k_i;
    float k_d;
    float t_s;
    int sum_limit_max;
    int sum_limit_min;
}pid_params;
pid_params pid_config;
uint measurement_delay_in_us;
uint measurement_iterations;
struct repeating_timer speed_helper_timer;
struct repeating_timer pid_control_timer;

int8_t check_for_package()  //function returns number of bytes if valid bit-pattern is found. Otherwise -1 is returned
{
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
int64_t readBit_alarm_callback(alarm_id_t id, void *user_data){
    writeLastBit(!gpio_get(DCC_INPUT_PIN));
    evaluation();
    return 0;
}

//start of transmission -> byte_n(address byte) -> ... -> byte_0(error detection byte) -> end of transmission
void bits_to_byte_array(int8_t number_of_bytes,uint8_t byte_array[]) {
    for (uint8_t i = 0; i < number_of_bytes; i++) {
        byte_array[i] = last_bits >> (i * 9 + 1);
    }
}


void acknowledge(){
    target_direction = false;
    adjust_pwm_level(5000);
    busy_wait_ms(6);
    adjust_pwm_level(0);
}

void verify_cv_bit(uint16_t cv_address,bool bit_val, uint8_t bit_pos){
    uint8_t mask = 0b00000001;
    bool res = ( (CV_ARRAY_FLASH[cv_address] >> bit_pos) &mask ) == bit_val;
    if (res) {
        acknowledge();
    }
}
void verify_cv_byte(uint16_t cv_address, uint8_t cv_data){
    if (CV_ARRAY_FLASH[cv_address] == cv_data) acknowledge();
}
void write_cv_byte(uint16_t cv_address, uint8_t cv_data){
    //CV_7 read-only
    if (cv_address == 6){
    }
    else{
        //Reset all CVs to Default (CV_8; Value = 8)
        if (cv_address == 7 && cv_data == 8){
            flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
            flash_range_program(FLASH_TARGET_OFFSET, CV_ARRAY_DEFAULT, FLASH_PAGE_SIZE * 2);
            acknowledge();
        }
        else{
            uint8_t CV_ARRAY_TEMP[512] = {0};
            memcpy(CV_ARRAY_TEMP, CV_ARRAY_FLASH, sizeof(CV_ARRAY_TEMP));
            CV_ARRAY_TEMP[cv_address] = cv_data;
            flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
            flash_range_program(FLASH_TARGET_OFFSET, CV_ARRAY_TEMP, FLASH_PAGE_SIZE * 2);
            acknowledge();
        }

    }
}

bool reset_package_check(uint8_t number_of_bytes,const uint8_t byte_array[]){
    if (byte_array[number_of_bytes-1] == 0b00000000 && byte_array[number_of_bytes-2] == 0b00000000){
        target_speed_step = 1;
        //update_active_functions(0,0,37);
        return true;
    }else {
        return false;
    }
}
void program_mode(uint8_t number_of_bytes, const uint8_t byte_array[]){
    //Check for valid programming command ("address" 112-127)
    if (byte_array[number_of_bytes - 1]<128 && byte_array[number_of_bytes - 1]>111){
            uint8_t instruction_type_mask = 0b00001100;
            uint8_t instruction_type = instruction_type_mask & byte_array[number_of_bytes - 1];
            uint8_t cv_address_ms_bits_mask = 0b00000011;
            uint16_t cv_address_ms_bits = cv_address_ms_bits_mask & byte_array[number_of_bytes - 1];
            uint16_t cv_address = byte_array[number_of_bytes - 2] + (cv_address_ms_bits << 8);
            alarm_pool_destroy(pid_control_timer.pool);
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

void set_outputs() {
    uint32_t GPIO_to_be_set = 0;
    //ensures that GPIO's that are used for inputs or PWM (motor) cannot be set HIGH
    uint32_t filter_forbidden_GPIO = 0b00000000000111111111111111111110;
    for (uint8_t i = 0; i < SIZE_ACTIVE_FUNCTIONS; i++) {
        if (active_functions[i]) {
            printf("F%u == 1\n",i);
            uint8_t func_cv_0 = CV_ARRAY_FLASH[260 + i * 8 - 4 * target_direction];
            uint8_t func_cv_1 = CV_ARRAY_FLASH[261 + i * 8 - 4 * target_direction];
            uint8_t func_cv_2 = CV_ARRAY_FLASH[262 + i * 8 - 4 * target_direction];
            uint8_t func_cv_3 = CV_ARRAY_FLASH[263 + i * 8 - 4 * target_direction];
            uint32_t func_cv = (func_cv_0) + (func_cv_1 << 8) + (func_cv_2 << 16) + (func_cv_3 << 24);
            GPIO_to_be_set = (GPIO_to_be_set | func_cv) & filter_forbidden_GPIO;
            uint32_t mask = 1;
            for (uint8_t j = 0; j < 32; j++)
            {
                uint32_t bit_value =  (GPIO_to_be_set&mask)>>j;
                //printf("GPIO: %u, set to: %d\n",j,bit_value);
                //gpio_put(j,bit_value);
                mask = mask<<1;
            }
            // printf("func_cv: %u\n",func_cv);
            // printf("func: F%u, direction: %u\n",func,direction);
            //printf("func_cv_0_index: %d  func_cv_1_index: %d  func_cv_2_index: %d  func_cv_3_index: %d  \n",4+257+i*8-4*direction,4+258+i*8-4*direction,4+259+i*8-4*direction,4+260+i*8-4*direction);
        }
    }
}

void update_active_functions(uint8_t function_number, uint8_t input_byte, uint8_t count) {
    uint8_t mask = 0b00000001;
    for (uint8_t i = 0; i < count; i++) {
        active_functions[i + function_number] = (input_byte & mask) == 0 ? 0 : 1;
        mask = mask << 1;
    }
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
    } else {
        command_byte_start_index = number_of_bytes - 2;
    }
    command_byte_n = byte_array[command_byte_start_index];
    //0011-1111 (128 Speed Step Control) - 2 Byte length
    if (command_byte_n == 0b00111111){
        target_speed_step = byte_array[command_byte_start_index - 1];
        if(target_speed_step>127) target_direction = true;
        else target_direction = false;
        init_speed_helper();
    }
    // 10XX-XXXX (Function Group Instruction)
    if (command_byte_n >> 6 == 0b00000010){
        if (command_byte_n >> 5 == 0b00000100) // Functions F0-F4
        {
            update_active_functions(0, command_byte_n >> 4, 1); //F0
            update_active_functions(1, command_byte_n, 4);      //F1-F4
        } else {
            switch (command_byte_n >> 4) {
                // Functions F5-F8
                case 0b00001011:
                    update_active_functions(5, command_byte_n, 4);
                    break;
                // Functions F9-F12
                case 0b00001010:
                    update_active_functions(9, command_byte_n, 4);
                    break;
                default:
                    break;
            }
        }
    }
    //Feature Expansion Instruction 110X-XXXX
    if (command_byte_n >> 5 == 0b00000110)
    {
        switch (command_byte_n) {
            case 0b11011110: // F13-F20
                // printf("Functions F13-F20 Instruction\n");
                update_active_functions(13, byte_array[command_byte_start_index - 1], 8);
                break;
            case 0b11011111: // F21-F28
                // printf("Functions F21-F28 Instruction\n");
                update_active_functions(21, byte_array[command_byte_start_index - 1], 8);
                break;
            case 0b11011000: // F29-F36
                // printf("Functions F29-F36 Instruction\n");
                update_active_functions(29, byte_array[command_byte_start_index - 1], 8);
                break;
            default:
                break;
        }
    }
    set_outputs();
}

//Interrupt handler for DCC Logic Signal
void gpio_callback_rise(unsigned int gpio, long unsigned int events) {
    add_alarm_in_us(87, &readBit_alarm_callback, NULL, true);
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
            else if (reset_package_flag){
                program_mode(number_of_bytes,byte_array);
            }
            else{
                reset_package_flag = reset_package_check(number_of_bytes,byte_array);
            }
        }
    }
}

void adjust_pwm_level(uint16_t level)
{
    //absolute_time_t from = get_absolute_time();
    //Forward
    if(target_direction){
        pwm_set_gpio_level(MOTOR_PWM_REVERSE,0);
        pwm_set_gpio_level(MOTOR_PWM_FORWARD,level);
    }
    //Reverse
    else{
        pwm_set_gpio_level(MOTOR_PWM_FORWARD,0);
        pwm_set_gpio_level(MOTOR_PWM_REVERSE,level);
    }
    //absolute_time_t to = get_absolute_time();
    //int64_t zeit = absolute_time_diff_us(from,to);
    //printf("zeit:%lld\n",zeit);
}

void measure(){
    pwm_set_gpio_level(MOTOR_PWM_FORWARD,0);
    pwm_set_gpio_level(MOTOR_PWM_REVERSE,0);
    adc_select_input(target_direction);
    busy_wait_us(measurement_delay_in_us);
    uint sum = 0;
    for (int i = 0; i < measurement_iterations; ++i) {
        sum = sum+adc_read();
    }
    measurement = (int)sum / measurement_iterations;
}

bool pid_control(struct repeating_timer *t){
    static float output;
    static int error;
    static int e_old;
    static int e_sum;
    measure();
    error = (int)current_v_emf_target - (int)measurement;
    e_sum = e_sum + error;
    if (e_sum > pid_config.sum_limit_max) e_sum = pid_config.sum_limit_max;
    if (e_sum < pid_config.sum_limit_min) e_sum = pid_config.sum_limit_min;
     output += (pid_config.k_p * (float)error) +
              (pid_config.k_i * pid_config.t_s * (float)e_sum) +
              (pid_config.k_d/pid_config.t_s) * (float)(error - e_old);
    e_old = error;
    if (output > 5000) output = 5000;
    if (output < 1400) output = 1400;
    if (current_v_emf_target == 0) output = 0;
    adjust_pwm_level((uint16_t )output);
    return true;
}

void init_pid(){
    pid_config.k_d = (float)CV_ARRAY_FLASH[48]/1024;
    pid_config.k_i = (float)CV_ARRAY_FLASH[49]/1024;
    pid_config.k_d = (float)CV_ARRAY_FLASH[50]/16384;
    pid_config.t_s = (float)CV_ARRAY_FLASH[47]/1000;
    pid_config.sum_limit_max = CV_ARRAY_FLASH[51]*10;
    pid_config.sum_limit_min = CV_ARRAY_FLASH[52]*(-10);
    measurement_delay_in_us = CV_ARRAY_FLASH[46];
    measurement_iterations = CV_ARRAY_FLASH[45];
    //printf("PID variables:\nk_p = %f;\nk_i = %f;\nk_d = %f;\nt_s = %f;\n",pid_config.k_p,pid_config.k_i,pid_config.k_d,pid_config.t_s);
}

void init_pwm(uint gpio) {
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint32_t wrap_counter = 5000;                  // 5000 Cycles @ 125MHz for 1 Period -> 25kHz
    pwm_set_wrap(slice_num, wrap_counter);
    pwm_set_gpio_level(gpio,0);
    pwm_set_clkdiv(slice_num,1);
    pwm_set_enabled(slice_num, true);
    //printf("PWM on GPIO %u was initialized.\n",gpio);
}

uint calc_end_v_emf_target(){
    //Forward
    if(target_direction){
        if (target_speed_step == 128) return 0;
        else return (target_speed_step - 129) * 16;
    }
    //Reverse
    else {
        if (target_speed_step == 0) return 0;
        else return (target_speed_step-1) * 16;
    }
}
bool speed_helper(struct repeating_timer *t) {
    //Emergency Stop
    if (target_speed_step == 129 || target_speed_step==1) { current_v_emf_target = 0; return false; }
    else{
        uint end_v_emf_target = calc_end_v_emf_target();
        if (end_v_emf_target > current_v_emf_target)  current_v_emf_target += 16;
        else if(end_v_emf_target < current_v_emf_target)  current_v_emf_target -= 16;
        else return false;
    }
    return true;
}

void init_speed_helper(){
    uint end_v_emf_target = calc_end_v_emf_target();
    //Acceleration
    if(end_v_emf_target > current_v_emf_target){
        int64_t acceleration_rate = -CV_ARRAY_FLASH[2]*7111;
        //Timer already running
        if (speed_helper_timer.alarm_id != 0) speed_helper_timer.delay_us = acceleration_rate;
        //Timer not running
        else alarm_pool_add_repeating_timer_us(alarm_pool_create(1,1),acceleration_rate, speed_helper, NULL, &speed_helper_timer);
    }
    //Deceleration
    if(end_v_emf_target < current_v_emf_target){
        int64_t deceleration_rate = -CV_ARRAY_FLASH[3]*7111;
        //Timer already running
        if (speed_helper_timer.alarm_id != 0) speed_helper_timer.delay_us = deceleration_rate;
        //Timer not running
        else alarm_pool_add_repeating_timer_us(alarm_pool_create(1,1),deceleration_rate, speed_helper, NULL, &speed_helper_timer);
    }
}
void print_cv_array(const uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        printf("CV_%u: %02x",i+1, buf[i]);
        if (i % 4 == 3)
            printf("\n");
        else
            printf(" ");
    }
}

void core1_entry() {
    if(gpio_get_function(MOTOR_PWM_FORWARD)!=4){
        gpio_set_function(MOTOR_PWM_FORWARD, GPIO_FUNC_PWM);
        gpio_set_function(MOTOR_PWM_REVERSE, GPIO_FUNC_PWM);
        adc_init();
        adc_gpio_init(V_EMF_ADC_PIN_FORWARD);
        adc_gpio_init(V_EMF_ADC_PIN_REVERSE);
        init_pwm(MOTOR_PWM_FORWARD);
        init_pwm(MOTOR_PWM_REVERSE);
        init_pid();
        init_speed_helper();
    }
    alarm_pool_add_repeating_timer_ms(alarm_pool_create(0,1),
                                      -CV_ARRAY_FLASH[47],
                                      pid_control,
                                      NULL,
                                      &pid_control_timer);
    while (1);
}

int main() {
    sleep_ms(1000);
    stdio_init_all();
    gpio_init(DCC_INPUT_PIN);
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(DCC_INPUT_PIN, GPIO_IN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    multicore_launch_core1(core1_entry);
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback_rise);
    printf("Launched successfully.\n");
    while (1);
}