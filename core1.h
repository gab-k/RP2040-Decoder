//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core1.h         //
//////////////////////////

#pragma once
#include "shared.h"
#define V_EMF_ADC_PIN_FORWARD 26
#define V_EMF_ADC_PIN_REVERSE 27

#define MEASURE_CORRECTION_THRESHOLD 1695
#define L1 0.001611328125f
#define C0 26.18270299751651f
#define C1 -0.04661614287233649f
#define C2 3.29589342261872e-05f
#define C3 -1.000062393830345e-08f
#define C4 1.15408454807169e-12f

void adjust_pwm_level(uint16_t level);
uint16_t calc_end_target();
bool speed_helper();
void init_pid();
void init_pwm_core1(uint gpio);
bool pid_control(struct repeating_timer *t);
void set_pid_params(float v);
void print_cv_array(const uint8_t *buf, size_t len);
