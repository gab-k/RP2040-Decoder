//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core1.h         //
//////////////////////////


#ifndef RP2040_DECODER_CORE1_H
#define RP2040_DECODER_CORE1_H

#endif //RP2040_DECODER_CORE1_H
#define V_EMF_ADC_PIN_FORWARD 26
#define V_EMF_ADC_PIN_REVERSE 27
#include <stdio.h>
#include "string.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/flash.h"
#include "hardware/irq.h"
#include "stdint.h"
void quicksort(uint16_t number[], int first, int last);
uint calc_end_target();
bool speed_helper();
int measure();
void init_pid();
void init_pwm(uint gpio);
void print_cv_array(const uint8_t *buf, size_t len);
