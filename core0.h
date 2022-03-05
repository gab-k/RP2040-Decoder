//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core0.h         //
//////////////////////////

#pragma once
#include "shared.h"
#include "CV.h"
#include "pico/float.h"
#define SIZE_BYTE_ARRAY 5
#define SIZE_ACTIVE_FUNCTIONS 32
#define PACKAGE_3_BYTES 0b11111111110000000000000000000000000001
#define PACKAGE_MASK_3_BYTES 0b11111111111000000001000000001000000001
#define PACKAGE_4_BYTES 0b11111111110000000000000000000000000000000000001
#define PACKAGE_MASK_4_BYTES 0b11111111111000000001000000001000000001000000001
#define PACKAGE_5_BYTES 0b11111111110000000000000000000000000000000000000000000001
#define PACKAGE_MASK_5_BYTES 0b11111111111000000001000000001000000001000000001000000001
const uint32_t clr_bit_arr[6] = {
        0b11111111111111111111111111100000, // Clear Bits 0-4 (F0-F4)
        0b11111111111111111111111000011111, // Clear Bits 5-8 (F5-F8)
        0b11111111111111111110000111111111, // Clear Bits 9-12 (F9-F12)
        0b11111111111000000001111111111111, // Clear Bits 13-20 (F13-F20)
        0b11100000000111111111111111111111, // Clear Bits 21-28 (F21-F28)
        0b00011111111111111111111111111111, // Clear Bits 29-31 (F29-F31)
};
int8_t check_for_package();
void writeLastBit(bool bit) ;
void bits_to_byte_array(int8_t number_of_bytes,uint8_t byte_array[]) ;
void acknowledge();
void verify_cv_bit(uint16_t cv_address,bool bit_val, uint8_t bit_pos);
void verify_cv_byte(uint16_t cv_address, uint8_t cv_data);
void write_cv_byte(uint16_t cv_index, uint8_t cv_data);
bool reset_package_check(uint8_t number_of_bytes,const uint8_t byte_array[]);
void program_mode(uint8_t number_of_bytes, const uint8_t byte_array[]);
void set_outputs(uint32_t active_functions);
void update_active_functions(uint32_t byte, uint8_t clr_bit_ind) ;
bool error_detection(int8_t number_of_bytes, const uint8_t byte_array[]) ;
bool is_long_address(uint8_t number_of_bytes, const uint8_t byte_array[]) ;
bool address_evaluation(uint8_t number_of_bytes,const uint8_t byte_array[]) ;
void instruction_evaluation(uint8_t number_of_bytes,const uint8_t byte_array[]);
void track_signal_rise(unsigned int gpio, long unsigned int events);
void track_signal_fall(unsigned int gpio, long unsigned int events);
void evaluation();
uint16_t find_offset(uint16_t level, uint8_t step, uint8_t delay, uint32_t threshold, bool direction);
uint16_t two_sigma(const uint16_t arr[], uint8_t length);
void setup_offsets(uint8_t length);