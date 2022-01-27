//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core1.h         //
//////////////////////////

#pragma once
#include "shared.h"
#define V_EMF_ADC_PIN_FORWARD 26
#define V_EMF_ADC_PIN_REVERSE 27
#define MEASURE_CORRECTION_THRESHOLD 2271
void quicksort(uint16_t number[], int first, int last);
uint calc_end_target();
bool speed_helper();
int measure();
void init_pid();
void init_pwm(uint gpio);
void print_cv_array(const uint8_t *buf, size_t len);