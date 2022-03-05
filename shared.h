//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      shared.h        //
//////////////////////////

#pragma once
#include <stdio.h>
#include "string.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/flash.h"
#include "hardware/irq.h"
#define _125M 125000000
#define ADC_OFFSET 13
#define DCC_INPUT_PIN 4u
#define MOTOR_FWD_PIN 22u
#define MOTOR_REV_PIN 23u
#define FWD_V_EMF_ADC_PIN 26u
#define REV_V_EMF_ADC_PIN 27u
//Aux output pins
#define AUX_PIN_MASK 0b00000000000000000011110000000000
//GPIO pins configured as inputs (Input functionality not implemented yet)
#define GPIO_INPUT_MASK 0b00000000000000000000000000000000
//GPIO pins configured as outputs
#define GPIO_OUTPUT_MASK 0b00000000000000000000000000111111
//GPIO pin mask to prevent setting illegal GPIOs (ADC, Motor, DCC Input Pin)
#define GPIO_ILLEGAL_MASK (1u<<DCC_INPUT_PIN) | (1u<<MOTOR_FWD_PIN) | (1u<<MOTOR_REV_PIN) | (1u<<FWD_V_EMF_ADC_PIN) | (1u<<REV_V_EMF_ADC_PIN)
//GPIO pin mask with allowed outputs (AUX & GPIO (configured as outputs))
#define GPIO_ALLOWED_OUTPUTS (GPIO_OUTPUT_MASK|AUX_PIN_MASK) & ~(GPIO_ILLEGAL_MASK)
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES-FLASH_SECTOR_SIZE) //This Offset should be far enough away from program data
extern repeating_timer_t pid_control_timer;
extern repeating_timer_t speed_helper_timer;
extern bool target_direction;
extern uint target_speed_step;
extern const uint8_t *CV_ARRAY_FLASH;
void core1_entry();
uint32_t measure(bool direction);