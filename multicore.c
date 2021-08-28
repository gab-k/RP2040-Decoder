#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "pwm.pio.h"
#include "automat.h"
#include "CV.h"
#include "multicore.h"
#define SIZE_BYTE_ARRAY 5
#define SIZE_ACTIVE_FUNCTIONS 69
bool active_functions[SIZE_ACTIVE_FUNCTIONS] = {false};
bool direction = 1;

void set_speed(uint8_t speed) {
    if (speed == 255) //Emergency Break
    {
        // printf("EMERGENCY BREAK");
    } else //normal speed control ranges from 0 to 126
    {
        // printf("Speed: %u  -  Direction: %u\n", speed, direction);
    }
}

void set_outputs() {
    uint32_t outputs_to_be_set = 0;
    for (uint8_t i = 0; i < SIZE_ACTIVE_FUNCTIONS; i++) {
        if (active_functions[i]) {
            // printf("F%u == 1\n",i);
            uint8_t func_cv_0 = CV_FUNCTION_ARRAY[4 + i * 8 - 4 * direction];
            uint8_t func_cv_1 = CV_FUNCTION_ARRAY[5 + i * 8 - 4 * direction];
            uint8_t func_cv_2 = CV_FUNCTION_ARRAY[6 + i * 8 - 4 * direction];
            uint8_t func_cv_3 = CV_FUNCTION_ARRAY[7 + i * 8 - 4 * direction];
            uint32_t func_cv = (func_cv_0) + (func_cv_1 << 8) + (func_cv_2 << 16) + (func_cv_3 << 24);
            outputs_to_be_set = outputs_to_be_set | func_cv;
            /*uint32_t mask = 1;
            or (uint8_t j = 0; j < 32; j++)
            {
                uint32_t bit_value =  (func_cv&mask)>>j;
                printf("GPIO: %u, set to: %d\n",j,bit_value);
                //gpio_put(j,bit_value);
                mask = mask<<1;
            }*/
            // printf("func_cv: %u\n",func_cv);
            // printf("func: F%u, direction: %u\n",func,direction);
            //printf("func_cv_0_index: %d  func_cv_1_index: %d  func_cv_2_index: %d  func_cv_3_index: %d  \n",4+257+i*8-4*direction,4+258+i*8-4*direction,4+259+i*8-4*direction,4+260+i*8-4*direction);
        }
    }
    // printf("outputs_to_bet_set: %u\n",outputs_to_be_set);
}

bool readBit() {
    busy_wait_us_32(87);
    return !gpio_get(17);
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
    if (byte_array[number_of_bytes - 1] >> 7) //most significant bit of first received byte
    {
        if (byte_array[number_of_bytes - 1] >> 6) //second most significant bit of first received byte
        {
            return true;
        }
    }
    return false;
}

bool address_evaluation(uint8_t number_of_bytes,const uint8_t byte_array[]) {
    uint16_t read_address;
    if (is_long_address(number_of_bytes, byte_array)) {
        //start of transmission -> address_byte_1 -> address_byte_0 -> ... -> end of transmission
        uint8_t address_byte_1 = (byte_array[number_of_bytes - 1]) - 192; //remove long address identifier bits
        uint8_t address_byte_0 = (byte_array[number_of_bytes - 2]);
        read_address = (address_byte_1 << 8) + address_byte_0;
        // printf("long address: %u, was read. \n", read_address);
    } else {
        //start of transmission ->  address_byte_0 -> ... -> end of transmission
        read_address = (byte_array[number_of_bytes - 1]);
        // printf("short address: %u, was read. \n",read_address);
    }
    if (CV_1 == read_address)
        return true;
    return false;
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

    if (command_byte_n >> 6 == 0b00000001) // 01XX-XXXX (Basic Speed and Direction Instruction)
    {
        if (command_byte_n << 4 >> 4 == 0b00000001) // 01XX-0001 (Emergency Break)
        {
            set_speed(255);
        }
        if (command_byte_n << 4 >> 4 == 0b00000000) // 01XX-0000 (Normal Break)
        {
            set_speed(0);
        } else {
            // printf("01XX-XXXX - Basic Speed \n");   //01XX-XXXX - Basic Speed (26 Steps)
            uint8_t mask = 0b01111111;
            uint8_t speed =
                    ((command_byte_n & mask) - 1) * 4; //Max speed step is equivalent to step 104 of 128 speed step
            mask = 0b00100000;
            direction = (command_byte_n & mask) >> 5;
            set_speed(speed);
        }
    }
    if (command_byte_n == 0b00111111) //0011-1111 (128 Speed Step Control) - 2 Byte length
    {
        // printf("0011-1111 (128 Speed Step Control) Instruction\n");
        direction = (byte_array[command_byte_start_index - 1]) >> 7;
        uint8_t command_byte_n_minus1 = byte_array[command_byte_start_index - 1];
        uint8_t mask = 0b01111111;
        uint8_t speed = (command_byte_n_minus1 & mask);
        if (speed == 0b00000001) // 01XX-0001 (Emergency Break)
        {
            set_speed(255);
        }
        if (speed == 0b00000000) // 01XX-0000 (Normal Break)
        {
            set_speed(0);
        } else {
            set_speed(speed - 1);
        }
    }
    if (command_byte_n >> 6 == 0b00000010) // 10XX-XXXX (Function Group Instruction)
    {
        if (command_byte_n >> 5 == 0b00000100) // Functions F0-F4
        {
            update_active_functions(0, command_byte_n >> 4, 1); //F0
            update_active_functions(1, command_byte_n, 4);      //F1-F4
            // printf("Functions F0-F4 Instruction\n");
        } else {
            switch (command_byte_n >> 4) {
                case 0b00001011: // Functions F5-F8
                    // printf("Functions F5-F8 Instruction\n");
                    update_active_functions(5, command_byte_n, 4);
                    break;
                case 0b00001010: // Functions F9-F12
                    // printf("Functions F9-F12 Instruction\n");
                    update_active_functions(9, command_byte_n, 4);
                    break;
                default:
                    break;
            }
        }
    }
    if (command_byte_n >> 5 == 0b00000110) //Feature Expansion Instruction 110X-XXXX
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

uint64_t last_bits = 0;
#define PACKAGE_3_BYTES 0b11111111110000000000000000000000000001
#define PACKAGEMASK_3_BYTES 0b11111111111000000001000000001000000001
#define PACKAGE_4_BYTES 0b11111111110000000000000000000000000000000000001
#define PACKAGEMASK_4_BYTES 0b11111111111000000001000000001000000001000000001
#define PACKAGE_5_BYTES 0b11111111110000000000000000000000000000000000000000000001
#define PACKAGEMASK_5_BYTES 0b11111111111000000001000000001000000001000000001000000001

int8_t check_for_package() {
    uint64_t basepacketMasked = last_bits & PACKAGEMASK_3_BYTES;
    if (basepacketMasked == PACKAGE_3_BYTES) {
        return 3;
    }
    uint64_t package4Masked = last_bits & PACKAGEMASK_4_BYTES;
    if (package4Masked == PACKAGE_4_BYTES) {
        return 4;
    }
    uint64_t package5Masked = last_bits & PACKAGEMASK_5_BYTES;
    if (package5Masked == PACKAGE_5_BYTES) {
        return 5;
    }
    return -1;
}

void writeLastBit(bool bit) {
    last_bits <<= 1;
    last_bits |= bit;
}

//start of transmission -> byte_n(addressbyte) -> ... -> byte_0(error detection byte) -> end of transmission
void bits_to_byte_array(int8_t number_of_bytes,uint8_t byte_array[]) {
    for (uint8_t i = 0; i < number_of_bytes; i++) {
        byte_array[i] = last_bits >> (i * 9 + 1);
    }

}

void gpio_callback_rise(unsigned int gpio, long unsigned int events) {
    writeLastBit(readBit());
    absolute_time_t from = get_absolute_time();
    int8_t number_of_bytes = check_for_package();
    if (number_of_bytes != -1) {
        uint8_t byte_array[SIZE_BYTE_ARRAY] = {0};
        bits_to_byte_array(number_of_bytes,byte_array);
        // convert_array(number_of_bytes); //converts bit_array to byte_array for easier handling
        if (error_detection(number_of_bytes,byte_array)) { //Returns true if the XOR verification was error-free
            // printf("no error :-)\n");
            if (address_evaluation(number_of_bytes,byte_array)) { //Returns true if address matches with CV
                // printf("address matches :-)\n");
                instruction_evaluation(number_of_bytes,byte_array);
            }
            // else  printf("ADDRESS DOES NOT MATCH!\n");
        }
        // else  printf("ERROR DETECTED!\n");
        absolute_time_t to = get_absolute_time();
        int64_t diff = absolute_time_diff_us(from, to);
        printf("%"PRId64"\n", diff);
        for (int i = 0; i < number_of_bytes; i++) {
            printf("BYTE_%u:" BYTE_TO_BINARY_PATTERN "\n", i, BYTE_TO_BINARY(byte_array[i]));
        }
        printf("Paketgroesse: %u\n", number_of_bytes);
        printf("\n");
        // printf(BYTE_TO_BINARY_PATTERN"\n",BYTE_TO_BINARY(error_detection(number_of_bytes, index_new)));
    }
}

int main() {
    stdio_init_all();
    gpio_init(17);
    gpio_set_dir(17, GPIO_IN);
    gpio_set_irq_enabled_with_callback(17, GPIO_IRQ_EDGE_RISE, true, &gpio_callback_rise);
    while (1);
}
