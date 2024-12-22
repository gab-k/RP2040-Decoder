/*!
*
 * \file shared.h
 * Used by both cores
 *
 */

#pragma once
#include "string.h"
#include "pico/float.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/printf.h"
#include "pico/flash.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/flash.h"
#include "hardware/gpio.h"

typedef enum {
   FLASH_SAFE_EXECUTE_CORE_INIT_FAILURE = (1<<0),
   FLASH_SAFE_EXECUTE_ERASE_FAILURE = (1<<1),
   FLASH_SAFE_EXECUTE_PROGRAM_FAILURE = (1<<2),
   FLASH_SIZE_READBACK_FAILURE = (1<<3),
   STDIO_INIT_FAILURE = (1<<4),
} error_t;


#define LOG(level, ...) { \
    if (level <= LOGLEVEL) { \
        printf("[Core %u] ", get_core_num()); \
        printf(__VA_ARGS__); \
    } \
}

// Constant Value of 125 x 10^6
#define _125M 125000000


extern repeating_timer_t pid_control_timer;
extern repeating_timer_t speed_helper_timer;
extern const uint8_t *CV_ARRAY_FLASH;
extern uint8_t speed_step_target;
extern uint8_t speed_step_target_prev;


void core1_entry();
void write_cv_handler(uint16_t cv_index, uint8_t cv_data);
float measure(uint8_t total_iterations,
              uint8_t measurement_delay_us,
              uint8_t l_side_arr_cutoff,
              uint8_t r_side_arr_cutoff,
              bool direction);
uint32_t get_32bit_CV (uint16_t CV_start_index);
uint16_t get_16bit_CV (uint16_t CV_start_index);
/*!
 * @brief Calculates the direction based on a speed step byte.
 *
 * This function calculates the direction of movement based on the value of 
 * the `speed_step` byte. It refers to the "128 Speed Control" standard as outlined 
 * in NMRA Standard 9.2.1 section 2.3.2.1 or RCN-212 section 2.2.2.
 *
 * The following speed steps are defined:
 *
 * | DEC | BIN       | DIRECTION | DEFINITION           |
 * |-----|-----------|-----------|----------------------|
 * | 000 | 0000 0000 | Reverse   | STOP                 |
 * | 001 | 0000 0001 | Reverse   | EMERGENCY STOP       |
 * | 002 | 0000 0010 | Reverse   | Speed step 001       |
 * | 003 | 0000 0011 | Reverse   | Speed step 002       |
 * | 004 | 0000 0100 | Reverse   | Speed step 003       |
 * | ... | .... .... | Reverse   | ...                  |
 * | 126 | 0111 1110 | Reverse   | Speed step 125       |
 * | 127 | 0111 1111 | Reverse   | Speed step 126       |
 * | 128 | 1000 0000 | Forward   | STOP                 |
 * | 129 | 1000 0001 | Forward   | EMERGENCY STOP       |
 * | 130 | 1000 0010 | Forward   | Speed step 001       |
 * | 131 | 1000 0011 | Forward   | Speed step 002       |
 * | 132 | 1000 0100 | Forward   | Speed step 003       |
 * | ... | .... .... | Forward   | ...                  |
 * | 254 | 1111 1110 | Forward   | Speed step 125       |
 * | 255 | 1111 1111 | Forward   | Speed step 126       |
 *
 * @param speed_step The speed step byte.
 * @return `true` for forward direction, `false` for reverse direction.
 */
bool get_direction_of_speed_step(uint8_t speed_step);

void set_error(error_t err);

void clear_error(error_t err);

uint32_t get_error_state();
