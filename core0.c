//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core0.c         //
//////////////////////////

#include "CV.h"
#include "core0.h"
#include "shared.h"

bool target_direction = true;
uint target_speed_step = 128;
const uint8_t *CV_ARRAY_FLASH = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
bool active_functions[SIZE_ACTIVE_FUNCTIONS] = {false};
uint64_t last_bits = 0;
bool reset_package_flag = false;


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
int64_t readBit_alarm_callback(alarm_id_t id, void *user_data) {
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
void adjust_pwm_level(uint16_t level) {
    if (target_direction) {
        pwm_set_gpio_level(MOTOR_PWM_REVERSE,0);
        pwm_set_gpio_level(MOTOR_PWM_FORWARD,level);
    }
    //Reverse
    else {
        pwm_set_gpio_level(MOTOR_PWM_FORWARD,0);
        pwm_set_gpio_level(MOTOR_PWM_REVERSE,level);
    }
}
void acknowledge() {
    target_direction = false;
    adjust_pwm_level(5000);
    busy_wait_ms(6);
    adjust_pwm_level(0);
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
    if (cv_index == 6 || cv_index == 7){
        //Reset all CVs to Default (CV_8; Value = 8)
        if (cv_data == 8){
            flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
            flash_range_program(FLASH_TARGET_OFFSET, CV_ARRAY_DEFAULT, FLASH_PAGE_SIZE * 2);
            acknowledge();
        }
    }
    else if(cv_index == 0 && cv_data == 0); //CV_1 = 0 is not permitted
    else{
            uint8_t CV_ARRAY_TEMP[512] = {0};
            memcpy(CV_ARRAY_TEMP, CV_ARRAY_FLASH, sizeof(CV_ARRAY_TEMP));
            CV_ARRAY_TEMP[cv_index] = cv_data;
            flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
            flash_range_program(FLASH_TARGET_OFFSET, CV_ARRAY_TEMP, FLASH_PAGE_SIZE * 2);
            acknowledge();
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

void set_outputs() {
    uint32_t GPIO_to_be_set = 0;
    for (uint8_t i = 0; i < 32; i++) {
        if (active_functions[i]) {
            uint8_t func_cv_0 = CV_ARRAY_FLASH[260 + i * 8 - 4 * target_direction];
            uint8_t func_cv_1 = CV_ARRAY_FLASH[261 + i * 8 - 4 * target_direction];
            uint8_t func_cv_2 = CV_ARRAY_FLASH[262 + i * 8 - 4 * target_direction];
            uint8_t func_cv_3 = CV_ARRAY_FLASH[263 + i * 8 - 4 * target_direction];
            uint32_t func_cv = (func_cv_0) + (func_cv_1 << 8) + (func_cv_2 << 16) + (func_cv_3 << 24);
            GPIO_to_be_set |= func_cv;
        }
    }
    GPIO_to_be_set &= ALLOWED_GPIO_MASK;
    gpio_put_masked(ALLOWED_GPIO_MASK,GPIO_to_be_set);
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
        if(target_speed_step>127) {
            target_direction = true;
        }
        else {
            target_direction = false;
        }
    }
    // 10XX-XXXX (Function Group Instruction)
    else if (command_byte_n >> 6 == 0b00000010){
        if (command_byte_n >> 5 == 0b00000100) // Functions F0-F4
        {
            update_active_functions(0, command_byte_n >> 4, 1);             //F0
            update_active_functions(1, command_byte_n, 4);                           //F1-F4
        } else {
            switch (command_byte_n >> 4) {
                case 0b00001011: // F5-F8
                    update_active_functions(5, command_byte_n, 4);
                    break;
                case 0b00001010: // F9-F12
                    update_active_functions(9, command_byte_n, 4);
                    break;
                default:
                    break;
            }
        }
    }
    //Feature Expansion Instruction 110X-XXXX
    else if (command_byte_n >> 5 == 0b00000110)
    {
        switch (command_byte_n) {
            case 0b11011110: // F13-F20
                update_active_functions(13, byte_array[command_byte_start_index - 1], 8);
                break;
            case 0b11011111: // F21-F28
                update_active_functions(21, byte_array[command_byte_start_index - 1], 8);
                break;
            case 0b11011000: // F29-F31
                update_active_functions(29, byte_array[command_byte_start_index - 1], 3);
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

int main() {
    busy_wait_ms(2000);
    stdio_init_all();
    printf("Launched successfully2.\n");
//    gpio_init(10);
//    gpio_set_dir(10,GPIO_OUT);
//    gpio_put(10,true);
    gpio_init(DCC_INPUT_PIN);
    gpio_set_dir(DCC_INPUT_PIN, GPIO_IN);
    gpio_init_mask(ALLOWED_GPIO_MASK);
    gpio_set_dir_out_masked(ALLOWED_GPIO_MASK);
    multicore_launch_core1(core1_entry);
    gpio_set_irq_enabled_with_callback(DCC_INPUT_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback_rise);
    printf("Launched successfully.3\n");
    while (1) ;
}