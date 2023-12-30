//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core1.c         //
//////////////////////////

#include "core1.h"
struct repeating_timer pid_control_timer,speed_helper_timer;


// Update speed_step_target, end_target_index and direction on receiving data from core0
void update_speed_dir(pid_params *pid, uint32_t speed_step_target){
    pid->speed_step_target = speed_step_target;
    // When CV_29 Bit 0 is set direction is reversed
    bool reverse_dir = CV_ARRAY_FLASH[28] & 0b00000001;
    // Forward
    if(speed_step_target > 127) {
        pid->direction = true^reverse_dir;
        if (pid->speed_step_target == 128) 
            pid->end_target_index = 0;
        else 
            pid->end_target_index = (pid->speed_step_target-129);
    }
    // Reverse
    else {
        pid->direction = false^reverse_dir;
        if (pid->speed_step_target == 0) 
            pid->end_target_index = 0;
        else 
            pid->end_target_index = (pid->speed_step_target-1);
    }
}


// This function gets called every x milliseconds where x is CV_175.
// The purpose of this function is to implement a time delay in acceleration or deceleration
bool speed_helper(struct repeating_timer *t) {
    // pid->setpoint only gets adjusted when the speed_helper_counter is equal to the accel_rate/decel_rate
    // -> Time for 1 Speed Step := (speed_helper timer delay)*(accel_rate) or CV_175*CV_3 or CV_175*CV_4
    pid_params *pid = (pid_params *)(t->user_data);
    uint8_t accel_rate = CV_ARRAY_FLASH[2];
    uint8_t decel_rate = CV_ARRAY_FLASH[3];
    static bool prev_dir;
    static uint8_t speed_helper_counter;
    static uint8_t speed_table_index;

    // Emergency Stop
    if (pid->speed_step_target == 129 || pid->speed_step_target==1) {
        speed_table_index = 0;
        speed_helper_counter = 0;
        pid->setpoint = pid->speed_table[speed_table_index];
    }

    // Acceleration
    else if (pid->end_target_index > speed_table_index && speed_helper_counter == accel_rate) {
        if(!accel_rate) speed_table_index = pid->end_target_index;  // directly accelerate to end target speed
        else speed_table_index++;
        pid->setpoint = pid->speed_table[speed_table_index];
        speed_helper_counter = 0;
    }

    // Deceleration
    else if (pid->end_target_index < speed_table_index && speed_helper_counter == decel_rate) {
        if(!decel_rate) speed_table_index = pid->end_target_index;    // directly decelerate to end target speed
        else speed_table_index--;
        pid->setpoint = pid->speed_table[speed_table_index];
        speed_helper_counter = 0;
    }

    // Change of direction while decelerating -> jump to most recent speed_step without delay (other direction)
    else if (pid->direction != prev_dir){
        speed_table_index = pid->end_target_index;
        pid->setpoint = pid->speed_table[speed_table_index];
        speed_helper_counter = 0;
    }

    // Acceleration/Deceleration "scheduled"
    else if (pid->end_target_index != speed_table_index) speed_helper_counter++;

    // Target reached -> Reset Counter
    else speed_helper_counter = 0;

    // Save prev speed_helper direction - not to be confused with pid->prev_direction
    prev_dir = pid->direction;

    // return true to schedule the repeating timer again
    return true;
}


// Helper function to adjust pwm level/duty cycle.
void adjust_pwm_level(uint16_t level, pid_params *pid) {
    // Forward
    if (pid->direction) {
        pwm_set_gpio_level(MOTOR_REV_PIN, 0);
        pwm_set_gpio_level(MOTOR_FWD_PIN, level);
    }
    // Reverse
    else {
        pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
        pwm_set_gpio_level(MOTOR_REV_PIN, level);
    }
}


// Update feed-forward slope/gradient for both directions
void update_m(pid_params *pid){
    pid->m_fwd = (pid->y_2_fwd-pid->y_1_fwd)/(pid->x_2_fwd-pid->x_1_fwd);
    pid->m_rev = (pid->y_2_rev-pid->y_1_rev)/(pid->x_2_rev-pid->x_1_rev);
}


// Update feedforward function value y_2 in both directions
void update_y(pid_params *pid,float i){
    // Forward
    if(pid->direction) {
        pid->y_2_fwd += i*pid->k_ff;
        // Limit y_2_fwd
        if(pid->y_2_fwd > pid->max_output) pid->y_2_fwd = pid->max_output;
        else if(pid->y_2_fwd < pid->y_1_fwd) pid->y_2_fwd = pid->y_1_fwd;
    }
    // Reverse
    else{
        pid->y_2_rev += i*pid->k_ff;
        // Limit y_2_rev
        if(pid->y_2_rev > pid->max_output) pid->y_2_rev = pid->max_output;
        else if(pid->y_2_rev < pid->y_1_rev) pid->y_2_rev = pid->y_1_rev;
    }

    update_m(pid);
}


// Returns feedforward value corresponding to current setpoint
float get_ff_val(pid_params *pid){
    float val;
    // Forward
    if(pid->direction){
        val = pid->m_fwd*((float)pid->setpoint-pid->x_1_fwd)+pid->y_1_fwd;
    }
    // Reverse
    else{
        val = pid->m_rev*((float)pid->setpoint-pid->x_1_rev)+pid->y_1_rev;
    }
    return val;
}


// Returns proportional gain value corresponding to current setpoint
float get_kp(pid_params *pid){
    float sp = (float)pid->setpoint;
    if (sp < pid->k_p_x_1){
        return pid->k_p_m_1 * sp + pid->k_p_y_0;
    }
    else{
        return pid->k_p_m_2 * (sp - pid->k_p_x_1) + pid->k_p_y_1;
    }
}


// PID controller function gets called every x milliseconds where x is CV_49 aka sampling time (pid->t)
bool pid_control(struct repeating_timer *t){
    // Cast (void *) to (pid_params *)
    pid_params *pid = (pid_params *)(t->user_data);

    // Acceleration from standstill or change in direction -> reset previous derivative, error and integral parts
    if((!pid->setpoint_prev && !pid->setpoint) || (pid->direction != pid->direction_prev)){
        pid->d_prev = 0.0f;
        pid->i_prev = 0.0f;
        pid->e_prev = 0.0f;
    }

    // Get feed-forward and proportional gain values
    float feed_fwd = get_ff_val(pid);
    pid->k_p = get_kp(pid);

    // Measure BEMF voltage and compute error
    pid->measurement = measure(pid->msr_total_iterations,
                               pid->msr_delay_in_us,
                               pid->l_side_arr_cutoff,
                               pid->r_side_arr_cutoff,
                               pid->direction);
    float e = (float)pid->setpoint - (pid->measurement - pid->adc_offset);

    // Proportional part, integral part and derivative part (including digital low-pass-filter with time constant tau)
    float p = pid->k_p * e;
    float i = pid->ci_0*(e + pid->e_prev) + pid->i_prev;
    float d = pid->cd_0*(pid->measurement-pid->measurement_prev)+pid->cd_1*pid->d_prev;

    // Check for limits on the integral part
    if (i > pid->int_lim_max)i = pid->int_lim_max;
    else if (i < pid->int_lim_min)i = pid->int_lim_min;

    // Sum feed forward + all controller terms (p+i+d). Limit Output from 0% to 100% duty cycle
    float output = feed_fwd+p+i+d;
    if (output < 0 || !pid->setpoint) output = 0.0f;
    else if (output > pid->max_output) output = pid->max_output;

    if(LOGLEVEL >= 1) {
        static int counter = 0;
        counter++;
        if(counter > 100) {
            LOG(1, "pid error(%f) output(%f)\n", e, output);
            counter = 0;            
        }
    }

    // Set PWM Level and discard results in adc fifo
    adjust_pwm_level((uint16_t)roundf(output),pid);
    adc_fifo_drain();

    // Check for High or Low Integrator values -> Update x,y values of feedforward function
    if( (output < pid->max_output && output > pid->max_output/2) && ((i > pid->int_lim_max/2) || (i < pid->int_lim_min/2)) ) update_y(pid,i);

    // Save previous error, integrator, differentiator, direction, setpoint and measured value also sum error to e_sum
    pid->e_prev = e;
    pid->i_prev = i;
    pid->d_prev = d;
    pid->direction_prev = pid->direction;
    pid->setpoint_prev = pid->setpoint;
    pid->measurement_prev = pid->measurement;

    // return true to schedule the repeating timer again
    return true;
}


// PID controller and measurement parameter, and speed_table initialization
void init_pid(pid_params *pid){
    // Measurement variables initialization
    pid->adc_offset = (float)CV_ARRAY_FLASH[171];
    pid->msr_total_iterations = CV_ARRAY_FLASH[60];
    pid->msr_delay_in_us = CV_ARRAY_FLASH[61];
    pid->l_side_arr_cutoff = CV_ARRAY_FLASH[62];
    pid->r_side_arr_cutoff = CV_ARRAY_FLASH[63];

    // Speed table initialization
    // Calculate speed setpoint table according to V_min, V_max and V_mid CVs
    double v_min = (double)CV_ARRAY_FLASH[1];
    double v_mid = (double)CV_ARRAY_FLASH[5]*16;
    double v_max = (double)CV_ARRAY_FLASH[4]*16;

    LOG(1, "speedtable min(%f) mid(%f) max(%f)\n", v_min, v_mid, v_max);

    double delta_x = 63;
    double m_1 = (v_mid-v_min)/delta_x;
    double m_2 = (v_max-v_mid)/delta_x;
    pid->speed_table[0] = 0;
    for (uint8_t i = 1; i < 64; ++i) {
        pid->speed_table[i] = (uint16_t)lround(m_1*(i-1)+v_min);
    }
    for (uint8_t i = 64; i < 127; ++i) {
        pid->speed_table[i] = (uint16_t)lround(m_2*(i-63)+v_mid);
    }

    // Controller constants initialization
    pid->k_i = (float)CV_ARRAY_FLASH[49]/10;
    pid->k_d = (float)CV_ARRAY_FLASH[50]/10000;
    pid->tau = (float)CV_ARRAY_FLASH[47]/1000;
    pid->t = (float)CV_ARRAY_FLASH[48]/1000;
    pid->ci_0 = (pid->k_i*pid->t)/2;
    pid->cd_0 = -(2*pid->k_d)/(2*pid->tau+pid->t);
    pid->cd_1 = (2*pid->tau-pid->t)/(2*pid->tau+pid->t);
    pid->int_lim_max = 10*(float)CV_ARRAY_FLASH[51];
    pid->int_lim_min = -10*(float)CV_ARRAY_FLASH[52];
    pid->k_ff = (float)CV_ARRAY_FLASH[46]/10000;
    pid->max_output = (float)(_125M/(CV_ARRAY_FLASH[8]*100+10000));
    pid->x_1_rev = 0.0f;
    pid->x_1_fwd = 0.0f;
    pid->y_1_fwd = (float)get_16bit_CV(175);
    pid->y_1_rev = (float)get_16bit_CV(177);
    pid->x_2_rev = (float)pid->speed_table[126];
    pid->x_2_fwd = (float)pid->speed_table[126];

    // Controller variables initialization
    pid->y_2_fwd = pid->max_output;
    pid->y_2_rev = pid->max_output;
    pid->measurement = 0.0f;
    pid->measurement_prev = 0.0f;
    pid->e_prev = 0.0f;
    pid->i_prev = 0.0f;
    pid->d_prev = 0.0f;
    pid->direction = false;
    pid->direction_prev = false;
    pid->speed_step_target = 0;
    pid->setpoint = 0;

    // Gain scheduling initialization
    pid->k_p_x_1_shift = (float)CV_ARRAY_FLASH[59] / 255.0f;
    pid->k_p_x_1 = (float)pid->speed_table[126] * pid->k_p_x_1_shift;
    pid->k_p_x_2 = (float)pid->speed_table[126] * (1.0f - pid->k_p_x_1_shift);
    pid->k_p_y_0 = (float)get_16bit_CV(53) / 100.0f;
    pid->k_p_y_1 = (float)get_16bit_CV(55) / 100.0f;
    pid->k_p_y_2 = (float)get_16bit_CV(57) / 100.0f;
    pid->k_p_m_1 = (pid->k_p_y_1 - pid->k_p_y_0) / pid->k_p_x_1;
    pid->k_p_m_2 = (pid->k_p_y_2 - pid->k_p_y_1) / pid->k_p_x_2;

    update_m(pid);
}


void core1_entry() {
    LOG(1, "core1 init...\n");
    pid_params pid_parameters;
    pid_params *pid = &pid_parameters;
    init_pid(pid);
    alarm_pool_add_repeating_timer_ms(alarm_pool_create(0,1),
                                      CV_ARRAY_FLASH[48],
                                      pid_control,
                                      pid,
                                      &pid_control_timer);
    irq_set_priority(0,0x00); // 0x00 = highest priority
    alarm_pool_add_repeating_timer_ms(alarm_pool_create(1,1),
                                      CV_ARRAY_FLASH[174],
                                      speed_helper,
                                      pid,
                                      &speed_helper_timer);
    // Send direction pointer for access and to let core0 know that core1 is done with initialization,
    // Note: Pointer will be cast into uint32_t and then back into bool * on core0 (Both 32-Bit Datatype).
    // This is done only to accommodate the correct data type associated with multicore_fifo_push_blocking().
    multicore_fifo_push_blocking((uint32_t) &(pid->direction));
    uint64_t packet_timeout_threshold_in_us = CV_ARRAY_FLASH[10]*1000000;
    absolute_time_t time_last_update = get_absolute_time();
//    printf("core1 done\n");
    while (1){
        // Check for time difference between last packet and current time (Packet Timeout)
        absolute_time_t current_time = get_absolute_time();
        if (absolute_time_diff_us(time_last_update,current_time) > packet_timeout_threshold_in_us){
            update_speed_dir(pid,1 + 128*pid->direction);   // Emergency stop on timeout
        }
        // If core0 sends new speed data to core1 speed_step_target and direction variables get updated
        if(multicore_fifo_rvalid()) {
            update_speed_dir(pid, multicore_fifo_pop_blocking());
            time_last_update = get_absolute_time();
        }
    }
}
