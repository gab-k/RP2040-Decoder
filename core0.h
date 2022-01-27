//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core0.h         //
//////////////////////////

#pragma once
#include "shared.h"
#include "CV.h"
#define SIZE_BYTE_ARRAY 5
#define SIZE_ACTIVE_FUNCTIONS 32
#define DCC_INPUT_PIN 11 //Custom: 11  Default: 14
//Default
//#define ALLOWED_GPIO_MASK 0b00000000000000000000111111111100      //prevents setting GPIO's that are not allowed (ADC, PWM, ...)
//Custom
#define ALLOWED_GPIO_MASK 0b00000000000000000000001111111100        //prevents setting GPIO's that are not allowed (ADC, PWM, ...)
#define PACKAGE_3_BYTES 0b11111111110000000000000000000000000001
#define PACKAGE_MASK_3_BYTES 0b11111111111000000001000000001000000001
#define PACKAGE_4_BYTES 0b11111111110000000000000000000000000000000000001
#define PACKAGE_MASK_4_BYTES 0b11111111111000000001000000001000000001000000001
#define PACKAGE_5_BYTES 0b11111111110000000000000000000000000000000000000000000001
#define PACKAGE_MASK_5_BYTES 0b11111111111000000001000000001000000001000000001000000001
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES-FLASH_SECTOR_SIZE) //This Offset should be far enough away from program data
int8_t check_for_package();
void writeLastBit(bool bit) ;
int64_t readBit_alarm_callback(alarm_id_t id, void *user_data);
void bits_to_byte_array(int8_t number_of_bytes,uint8_t byte_array[]) ;
void acknowledge();
void verify_cv_bit(uint16_t cv_address,bool bit_val, uint8_t bit_pos);
void verify_cv_byte(uint16_t cv_address, uint8_t cv_data);
void write_cv_byte(uint16_t cv_index, uint8_t cv_data);
bool reset_package_check(uint8_t number_of_bytes,const uint8_t byte_array[]);
void program_mode(uint8_t number_of_bytes, const uint8_t byte_array[]);
void set_outputs() ;
void update_active_functions(uint8_t function_number, uint8_t input_byte, uint8_t count) ;
bool error_detection(int8_t number_of_bytes, const uint8_t byte_array[]) ;
bool is_long_address(uint8_t number_of_bytes, const uint8_t byte_array[]) ;
bool address_evaluation(uint8_t number_of_bytes,const uint8_t byte_array[]) ;
void instruction_evaluation(uint8_t number_of_bytes,const uint8_t byte_array[]);
void gpio_callback_rise(unsigned int gpio, long unsigned int events) ;
void evaluation();