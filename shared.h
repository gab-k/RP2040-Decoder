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
#define GPIO_USED 0b00001100110000000011111000011111u
#define GPIO_ILLEGAL (DCC_INPUT_PIN | MOTOR_FWD_PIN | MOTOR_REV_PIN | FWD_V_EMF_ADC_PIN | REV_V_EMF_ADC_PIN)
#define GPIO_ALLOWED GPIO_USED & ~GPIO_ILLEGAL  //GPIO mask to prevent setting illegal GPIOs (ADC, Motor, DCC Input Pin)
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES-FLASH_SECTOR_SIZE) //This Offset should be far enough away from program data
extern repeating_timer_t pid_control_timer;
extern repeating_timer_t speed_helper_timer;
extern bool target_direction;
extern uint target_speed_step;
extern const uint8_t *CV_ARRAY_FLASH;
void core1_entry();
uint32_t measure(bool direction);