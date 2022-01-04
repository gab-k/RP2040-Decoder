////////////////////////////////////
//        RP2040-Decoder          //
// created by Gabriel Koppenstein //
////////////////////////////////////

#pragma once
#define SIZE_BYTE_ARRAY 5
#define SIZE_ACTIVE_FUNCTIONS 69
#define DCC_INPUT_PIN 11
#define MOTOR_PWM_FORWARD 12
#define MOTOR_PWM_REVERSE 13
#define V_EMF_ADC_PIN_FORWARD 27
#define V_EMF_ADC_PIN_REVERSE 26
#define PACKAGE_3_BYTES 0b11111111110000000000000000000000000001
#define PACKAGE_MASK_3_BYTES 0b11111111111000000001000000001000000001
#define PACKAGE_4_BYTES 0b11111111110000000000000000000000000000000000001
#define PACKAGE_MASK_4_BYTES 0b11111111111000000001000000001000000001000000001
#define PACKAGE_5_BYTES 0b11111111110000000000000000000000000000000000000000000001
#define PACKAGE_MASK_5_BYTES 0b11111111111000000001000000001000000001000000001000000001
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES-FLASH_SECTOR_SIZE) //This Offset should be far enough away from program data
void core1_entry();
void init_speed_helper();
void adjust_pwm_level(uint16_t level);
void gpio_callback_rise(unsigned int gpio, long unsigned int events);
void evaluation();