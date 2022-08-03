//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      shared.h        //
//////////////////////////

#pragma once
#include "string.h"
#include "pico/float.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/printf.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/flash.h"
#include "hardware/irq.h"

// Constant Value of 125 x 10⁶
#define _125M 125000000

// GPIO used for DCC-Signal, Motor PWM or ADC
#define DCC_INPUT_PIN 21u
#define MOTOR_FWD_PIN 23u
#define MOTOR_REV_PIN 22u
#define FWD_V_EMF_ADC_PIN 28u
#define REV_V_EMF_ADC_PIN 29u

// GPIO used directly (GPIO 0-5 incl.) as outputs or to switch auxiliary output transistors (GPIO 24-27 incl.)
#define GPIO_OUTPUT_PIN_MASK (1u<<24) | (1u<<25) | (1u<<26) | (1u<<27) | (1u<<5) | (1u<<4) | (1u<<3) | (1u<<2) | (1u<<1) | (1u<<0)
// GPIO pin mask to prevent setting illegal GPIOs (ADC, Motor, DCC Input Pin)
#define GPIO_ILLEGAL_MASK (1u<<DCC_INPUT_PIN) | (1u<<MOTOR_FWD_PIN) | (1u<<MOTOR_REV_PIN) | (1u<<FWD_V_EMF_ADC_PIN) | (1u<<REV_V_EMF_ADC_PIN)

// GPIO pin mask with allowed outputs (AUX & GPIO (configured as outputs))
#define GPIO_ALLOWED_OUTPUTS (GPIO_OUTPUT_PIN_MASK) & ~(GPIO_ILLEGAL_MASK)

// Offset from base address used for saving CV_ARRAY_FLASH
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES-FLASH_SECTOR_SIZE)

extern repeating_timer_t pid_control_timer;
extern repeating_timer_t speed_helper_timer;
extern const uint8_t *CV_ARRAY_FLASH;
void core1_entry();
float measure(uint8_t total_iterations,
              uint8_t measurement_delay_us,
              uint8_t l_side_arr_cutoff,
              uint8_t r_side_arr_cutoff,
              bool direction);
uint32_t get_32bit_CV (uint16_t CV_start_index);
uint16_t get_16bit_CV (uint16_t CV_start_index);