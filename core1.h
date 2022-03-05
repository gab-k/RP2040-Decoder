//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core1.h         //
//////////////////////////

#pragma once
#include "shared.h"
uint16_t calc_end_target();
bool speed_helper();
void adjust_pwm_level(uint16_t level);
void init_offsets(uint16_t lvl_max);
uint32_t offset();
bool pid_control(struct repeating_timer *t);
void init_pid();
void init_motor_pwm(uint8_t gpio);
