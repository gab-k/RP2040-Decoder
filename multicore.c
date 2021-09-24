#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/flash.h"
#include "CV.h"
#include "multicore.h"
const uint8_t *CV_ARRAY_FLASH = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
uint64_t last_bits = 0;
bool active_functions[SIZE_ACTIVE_FUNCTIONS] = {false};
uint32_t target_speed_step = 128;
bool target_direction = true;
int error = 0;
int e_old = 0;
int e_sum = 0;
float t_s = 0.01f;
int sum_limit_max = 1000;
int sum_limit_min = -1000;
float k_p = 0.09f;
float k_i = 0.075f;
float k_d = 0.008f;
float output = 0;
uint target_v_emf = 0;
uint measurement;
bool program_mode_flag = false;

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

bool readBit() {
    busy_wait_us_32(87);
    return !gpio_get(DCC_INPUT_PIN);
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

void interp_speed_step(uint8_t speed_byte){
    target_speed_step = speed_byte;
    //Forward
    if(speed_byte>127){
        target_v_emf = (speed_byte-129)*32;
        target_direction = true;
    }
    //Reverse
    if(speed_byte<128) {
        target_v_emf = (speed_byte-1)*32;
        target_direction = false;
    }
    if (speed_byte == 128 || speed_byte==0) target_v_emf = 0;
}

void program_mode(){

}

void set_outputs() {
    uint32_t GPIO_to_be_set = 0;
    //ensures that GPIO's that are used for inputs or PWM (motor) cannot be set HIGH
    uint32_t filter_forbidden_GPIO = 0b00000000000111111111111111111110;
    for (uint8_t i = 0; i < SIZE_ACTIVE_FUNCTIONS; i++) {
        if (active_functions[i]) {
            printf("F%u == 1\n",i);
            uint8_t func_cv_0 = CV_ARRAY[4 + i * 8 - 4 * target_direction];
            uint8_t func_cv_1 = CV_ARRAY[5 + i * 8 - 4 * target_direction];
            uint8_t func_cv_2 = CV_ARRAY[6 + i * 8 - 4 * target_direction];
            uint8_t func_cv_3 = CV_ARRAY[7 + i * 8 - 4 * target_direction];
            uint32_t func_cv = (func_cv_0) + (func_cv_1 << 8) + (func_cv_2 << 16) + (func_cv_3 << 24);
            GPIO_to_be_set = (GPIO_to_be_set | func_cv) & filter_forbidden_GPIO;
            uint32_t mask = 1;
            for (uint8_t j = 0; j < 32; j++)
            {
                uint32_t bit_value =  (GPIO_to_be_set&mask)>>j;
                printf("GPIO: %u, set to: %d\n",j,bit_value);
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
    uint16_t read_address;
    //Enters Programming Mode
    if (program_mode_flag && byte_array[number_of_bytes - 1]<128 && byte_array[number_of_bytes - 1]>111){
        program_mode();
    }
    program_mode_flag = false;
    //Check for Idle Package
    if (byte_array[number_of_bytes - 1] == 255)
    {
        return false;
    }
    //Check for Broadcast Package
    if (byte_array[number_of_bytes-1] == 0)
    {
        printf("Broadcast-Package found!\n");
        return true;
    }
    //Long Address Package
    if (is_long_address(number_of_bytes, byte_array))
    {
        //start of transmission -> address_byte_1 -> address_byte_0 -> ... -> end of transmission
        uint16_t address_byte_1 = (byte_array[number_of_bytes - 1]) - 192;  //remove long address identifier bits
        uint16_t address_byte_0 = (byte_array[number_of_bytes - 2]);
        read_address = (address_byte_1 << 8) + address_byte_0;
        //printf("long address: %u, was read. \n", read_address);
    }
    //Short Address Package
    else
    {
        //start of transmission ->  address_byte_0 -> ... -> end of transmission
        read_address = (byte_array[number_of_bytes - 1]);
        //printf("short address: %u, was read. \n",read_address);
    }
    return (CV_ARRAY[0] == read_address);    //returns true if Address Matches
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
    //0000-0000  -  Reset Package
    if (command_byte_n == 0b00000000){
        target_speed_step = 1;
        update_active_functions(0,0,37);
        program_mode_flag = true;
    }
    //0011-1111 (128 Speed Step Control) - 2 Byte length
    if (command_byte_n == 0b00111111)
    {
        //printf("0011-1111 (128 Speed Step Control) Instruction\n");
        uint8_t command_byte_n_minus1 = byte_array[command_byte_start_index - 1];
        if (command_byte_n_minus1 != target_speed_step) interp_speed_step(command_byte_n_minus1);
    }
    // 10XX-XXXX (Function Group Instruction)
    if (command_byte_n >> 6 == 0b00000010)
    {
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
    writeLastBit(readBit());
    int8_t number_of_bytes = check_for_package();
    if (number_of_bytes != -1) {
            uint8_t byte_array[SIZE_BYTE_ARRAY] = {0};
            //uint8_t byte_array[SIZE_BYTE_ARRAY] = {(5^128^63),128,0b00111111,5,0};
            bits_to_byte_array(number_of_bytes,byte_array);
        if (error_detection(number_of_bytes,byte_array)) { //Returns true if the XOR verification was error-free
            if (address_evaluation(number_of_bytes,byte_array)) { //Returns true if address matches with CV
                instruction_evaluation(number_of_bytes,byte_array);
            }
        }
    }
}
void adjust_pwm_level(uint16_t level)
{
    if(target_direction){
        pwm_set_gpio_level(MOTOR_PWM_REVERSE,0);
        pwm_set_gpio_level(MOTOR_PWM_FORWARD,level);
    }
    else{
        pwm_set_gpio_level(MOTOR_PWM_FORWARD,0);
        pwm_set_gpio_level(MOTOR_PWM_REVERSE,level);
    }
}

void measure(){
    pwm_set_gpio_level(MOTOR_PWM_FORWARD,0);
    pwm_set_gpio_level(MOTOR_PWM_REVERSE,0);
    busy_wait_us(100);
    uint sum = 0;
    for (int i = 0; i < N_MEASUREMENTS; ++i) {
        sum = sum+adc_read();
    }
    measurement = (int)sum / N_MEASUREMENTS;
}

void pid_control(){
    measure();
    error =  (int)target_v_emf - (int)measurement;
    e_sum = e_sum + error;
    if (e_sum > sum_limit_max) e_sum = sum_limit_max;
    if (e_sum < sum_limit_min) e_sum = sum_limit_min;
    output =  output+ (k_p * (float)error) + (k_i * t_s * (float)e_sum) + ((k_d/t_s) * (float)(error - e_old));
    e_old = error;
    if (output > 5000) output = 5000;
    if (output < 1400) output = 1400;
    if (target_v_emf == 0) output = 0;
    adjust_pwm_level((u_int16_t )output);
    busy_wait_us(10000);
}

void init_pwm(uint gpio) {
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint32_t wrap_counter = 5000;                  // 5000 Cycles @ 125MHz for 1 Period -> 25kHz
    pwm_set_wrap(slice_num, wrap_counter);
    pwm_set_gpio_level(gpio,0);
    pwm_set_clkdiv(slice_num,1);
    pwm_set_enabled(slice_num, true);
    printf("PWM  on GPIO %u was initialized.\n",gpio);
}

void init_adc(uint gpio){
    adc_init();
    adc_gpio_init(gpio);
    adc_select_input(0);
    printf("ADC  on GPIO %d was initialized.\n",gpio);
}

void core1_entry() {
    gpio_set_function(MOTOR_PWM_FORWARD, GPIO_FUNC_PWM);
    gpio_set_function(MOTOR_PWM_REVERSE, GPIO_FUNC_PWM);
    init_adc(V_EMF_ADC_PIN);
    init_pwm(MOTOR_PWM_FORWARD);
    init_pwm(MOTOR_PWM_REVERSE);
    printf("Core 1 was initialized.\n");
    while (1) pid_control();
}


void print_buf(const uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        printf("CV_%u: %02x",i+1, buf[i]);
        if (i % 8 == 7)
            printf("\n");
        else
            printf(" ");
    }
}

void write_updated_cvs_to_flash(){
    multicore_reset_core1();
    uint32_t ints = save_and_disable_interrupts();
    printf("Erasing CV's...\n");
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE*2);
    printf("Erasing Done.\n");
    printf("Programming CV's...\n");
    flash_range_program(FLASH_TARGET_OFFSET, CV_ARRAY, FLASH_PAGE_SIZE * 2);
    printf("Programming Done. Read back CV's:\n");
    print_buf(CV_ARRAY_FLASH, FLASH_PAGE_SIZE * 2);
    restore_interrupts(ints);
    multicore_launch_core1(core1_entry);
}

int main() {
    sleep_ms(1000);
    stdio_init_all();
    gpio_init(DCC_INPUT_PIN);
    gpio_set_dir(DCC_INPUT_PIN, GPIO_IN);
    multicore_launch_core1(core1_entry);
    busy_wait_ms(1000); //This delay is necessary to catch the breakpoint
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback_rise);
    busy_wait_ms(200); //This delay is necessary to catch the breakpoint
    //gpio_callback_rise(0,0);
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, true);
    while (1);
}

