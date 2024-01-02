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
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/flash.h"


// here you can enable printf debugging (needs extra soldering and you have to enable uart in the cmake configuration)
// please note
// - if it's disabled it will be compiled completely out
// - be careful that printf debugging can change your timing behavior, for example in the pid controller limit the outputs
//   like already done (only print every 100th iteration for example!)
// - also note that with logging enabled GPIO0/1 are DISABLED from decoder functions because they are used for uart to the
//   outside world (this is configure automatically for you below!)
// - IMPORTANT: you have to enable uart in the cmakelists.txt to enable logging!
#define LOGLEVEL 0
#define LOG(level, ...) { if(level <= LOGLEVEL) {printf(__VA_ARGS__);}}

// Constant Value of 125 x 10⁶
#define _125M 125000000

// GPIO used for DCC-Signal, Motor PWM or ADC
#define DCC_INPUT_PIN 21u
#define MOTOR_FWD_PIN 23u
#define MOTOR_REV_PIN 22u
#define FWD_V_EMF_ADC_PIN 28u
#define REV_V_EMF_ADC_PIN 29u

// if logging is enabled do NOT use gpio0/1 for functions in the decoder
#if LOGLEVEL != 0
   // GPIO used directly (GPIO 2-5 incl.) as outputs or to switch auxiliary output transistors (GPIO 24-27 incl.)
   #define GPIO_OUTPUT_PIN_MASK (1u<<24) | (1u<<25) | (1u<<26) | (1u<<27) | (1u<<5) | (1u<<4) | (1u<<3) | (1u<<2)
#else
   // GPIO used directly (GPIO 0-5 incl.) as outputs or to switch auxiliary output transistors (GPIO 24-27 incl.)
   #define GPIO_OUTPUT_PIN_MASK (1u<<24) | (1u<<25) | (1u<<26) | (1u<<27) | (1u<<5) | (1u<<4) | (1u<<3) | (1u<<2) | (1u<<1) | (1u<<0)
#endif

// GPIO pin mask to prevent setting illegal GPIOs (ADC, Motor, DCC Input Pin)
#define GPIO_ILLEGAL_MASK (1u<<DCC_INPUT_PIN) | (1u<<MOTOR_FWD_PIN) | (1u<<MOTOR_REV_PIN) | (1u<<FWD_V_EMF_ADC_PIN) | (1u<<REV_V_EMF_ADC_PIN)

// GPIO pin mask with allowed outputs (AUX & GPIO (configured as outputs))
#define GPIO_ALLOWED_OUTPUTS (GPIO_OUTPUT_PIN_MASK) & ~(GPIO_ILLEGAL_MASK)

// Offset from base address used for saving CV_ARRAY_FLASH
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES-FLASH_SECTOR_SIZE)

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
 * @brief Get direction
 * Function calculates direction.Direction is based on speed_step byte. <br>
 * Refers to "128 Speed Control" - NMRA Standard 9.2.1 section 2.3.2.1 or RCN-212 section 2.2.2. <br>
 *
 * The following speed steps exist:
 * * DEC | BIN	   | DIRECTION | DEFINITION
 * * 000 | 0000 0000 | Reverse   | STOP
 * * 001 | 0000 0001 | Reverse   | EMERGENCY STOP
 * * 002 | 0000 0010 | Reverse   | Speed step 001 (Lowest speed)
 * * 003 | 0000 0011 | Reverse   | Speed step 002
 * * 004 | 0000 0100 | Reverse   | Speed step 003
 * * ... | .... .... | Reverse   | .....
 * * ... | .... .... | Reverse   | .....
 * * ... | .... .... | Reverse   | .....
 * * 126 | 0111 1110 | Reverse   | Speed step 125
 * * 127 | 0111 1111 | Reverse   | Speed step 126 (Full speed)
 * * 128 | 1000 0000 | Forward   | STOP
 * * 129 | 1000 0001 | Forward   | EMERGENCY STOP
 * * 130 | 1000 0010 | Forward   | Speed step 001 (Lowest speed)
 * * 131 | 1000 0011 | Forward   | Speed step 002
 * * 132 | 1000 0100 | Forward   | Speed step 003
 * * ... | .... .... | Forward   | .....
 * * ... | .... .... | Forward   | .....
 * * ... | .... .... | Forward   | .....
 * * 254 | 1111 1110 | Forward   | Speed step 125
 * * 255 | 1111 1111 | Forward   | Speed step 126 (Full speed)
 *
 * @param speed_step speed step byte
 * @return direction; false for reverse; true for forward;
 */
bool get_direction_of_speed_step(uint8_t speed_step);