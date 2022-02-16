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
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/flash.h"
#include "hardware/irq.h"
#include "stdint.h"
#define MOTOR_PWM_FORWARD 22
#define MOTOR_PWM_REVERSE 23
extern repeating_timer_t pid_control_timer;
extern repeating_timer_t speed_helper_timer;
extern bool target_direction;
extern uint target_speed_step;
extern const uint8_t *CV_ARRAY_FLASH;
void adjust_pwm_level(uint16_t level);
void core1_entry();