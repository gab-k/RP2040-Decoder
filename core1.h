//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core1.h         //
//////////////////////////

#pragma once
#include "shared.h"

typedef struct pid_params{
    // Controller variables - won't change after init
    float k_i;                      // Integral gain
    float k_d;                      // Derivative gain
    float t;                        // Sampling time
    float tau;                      // Low-pass-filter time constant
    float k_ff;                     // Feed-Forward gain
    float ci_0;                     // (k_i*t)/2
    float cd_0;                     // (k_d*2)/(2*tau+t)
    float cd_1;                     // (2*tau-t)/(2*tau+t)
    float int_lim_max;              // max limit for integrator
    float int_lim_min;              // min limit for integrator
    float max_output;               // max possible pwm output value
    float k_p_x_1_shift;
    float k_p_x_1;
    float k_p_x_2;
    float k_p_y_0;
    float k_p_y_1;
    float k_p_y_2;
    float k_p_m_1;
    float k_p_m_2;
    // Controller variables - might change on runtime
    float k_p;                      // Proportional gain
    float e_prev;                   // Previous Error
    float i_prev;                   // Previous Integral Value
    float d_prev;                   // Previous Derivative Value
    float measurement_prev;         // Previous Measurement
    float x_1_fwd;                  // Base Speed; forward direction
    float x_2_fwd;                  // High Speed; forward direction
    float y_1_fwd;                  // Base PWM; forward direction
    float y_2_fwd;                  // High PWM; forward direction
    float x_1_rev;                  // Base Speed; reverse direction
    float x_2_rev;                  // High Speed; reverse direction
    float y_1_rev;                  // Base PWM; reverse direction
    float y_2_rev;                  // High PWM; reverse direction
    float m_fwd;                    // Slope forward direction
    float m_rev;                    // Slope reverse direction
    uint16_t speed_table[127];      // Array with setpoint values corresponding to every speed step
    uint32_t speed_step_target;     // End target speed step
    uint16_t end_target_index;      // Index for speed table array corresponding to end target speed step
    uint32_t setpoint;              // Current setpoint
    uint32_t setpoint_prev;         // Previous setpoint
    bool direction;                 // Current Direction
    bool direction_prev;            // Previous Direction
    // Measurement Variables
    float measurement;              // Newest Measurement value
    float adc_offset;               // ADC offset value
    uint8_t msr_delay_in_us;        // Delay before VEMF Measurement
    uint8_t msr_total_iterations;   // Amount of samples
    uint8_t l_side_arr_cutoff;      // Discarded samples (left side)
    uint8_t r_side_arr_cutoff;      // Discarded samples (right side)
}pid_params;

void update_speed_dir(pid_params *pid, uint32_t speed_step_target);
bool speed_helper(struct repeating_timer *t);
void adjust_pwm_level(uint16_t level, const pid_params *pid);
void update_m(pid_params *pid);
void update_y(pid_params *pid,float i);
float get_ff_val(const pid_params *pid);
float get_kp(const pid_params *pid);
bool pid_control(struct repeating_timer *t);
void init_pid( pid_params * pid);