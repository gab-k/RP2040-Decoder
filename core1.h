/*!
*
 * \file core1.h
 * Used exclusively by core1
 *
 */

#pragma once
#include "shared.h"

#define BASE_PWM_ARR_LEN 16

typedef enum {
    STARTUP_MODE,
    PID_MODE,
} controller_mode_t;

typedef struct pid_params{
    // General Controller variables
    controller_mode_t mode;

    // Startup mode variables
    float feed_fwd;
    uint16_t base_pwm_arr[BASE_PWM_ARR_LEN];
    uint16_t base_pwm_arr_i;


    // PID Controller variables
    float k_i;                      // Integral gain
    float k_d;                      // Derivative gain
    float t;                        // Sampling time
    float tau;                      // Low-pass-filter time constant
    float ci_0;                     // (k_i*t)/2
    float cd_0;                     // (k_d*2)/(2*tau+t)
    float cd_1;                     // (2*tau-t)/(2*tau+t)
    float int_lim_max;              // max limit for integrator
    float int_lim_min;              // min limit for integrator
    float max_output;               // max possible pwm output value
    float k_p;                      // Proportional gain
    float e;                        // Error
    float e_prev;                   // Previous Error
    float i_prev;                   // Previous Integral Value
    float d_prev;                   // Previous Derivative Value
    uint32_t setpoint;              // Current setpoint
    uint16_t output;
    uint16_t speed_table[127];      // Array with setpoint values corresponding to every speed step

    // Measurement Variables
    float measurement;              // Newest Measurement value
    float measurement_prev;         // Previous Measurement
    float measurement_corrected;
    float adc_offset;               // ADC offset value
    uint8_t msr_delay_in_us;        // Delay before VEMF Measurement
    uint8_t msr_total_iterations;   // Amount of samples
    uint8_t l_side_arr_cutoff;      // Discarded samples (left side)
    uint8_t r_side_arr_cutoff;      // Discarded samples (right side)

    // Variable gain variables
    float k_p_x_1_shift;
    float k_p_x_1;
    float k_p_x_2;
    float k_p_y_0;
    float k_p_y_1;
    float k_p_y_2;
    float k_p_m_1;
    float k_p_m_2;
} controller_parameter_t;


uint8_t get_speed_step_table_index_of_speed_step(uint8_t speed_step);
bool speed_helper(struct repeating_timer *t);
void adjust_pwm_level(uint16_t level);
float get_kp(const controller_parameter_t *pid);
uint16_t get_initial_level(controller_parameter_t * ctrl_par);
void controller_startup_mode(controller_parameter_t * ctrl_par);
bool controller_general(struct repeating_timer * t);
void init_controller(controller_parameter_t * ctrl_par);