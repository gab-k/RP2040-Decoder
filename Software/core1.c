//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core1.c         //
//////////////////////////

#include "core1.h"

struct repeating_timer pid_control_timer, speed_helper_timer;
bool controller_flag, speed_helper_flag = false;


// Measures Back-EMF voltage (proportional to the rotational speed of the motor) on GPIO 28 and GPIO 29 respectively (depending on direction)
float measure(uint8_t total_iterations,
              uint8_t measurement_delay_us,
              uint8_t l_side_arr_cutoff,
              uint8_t r_side_arr_cutoff,
              direction_t direction){
    uint32_t sum = 0;
    uint16_t adc_val[total_iterations];
    int32_t j = 0;
    uint16_t key;
    pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
    pwm_set_gpio_level(MOTOR_REV_PIN, 0);
    if (direction == DIRECTION_FORWARD) {
        adc_select_input(FWD_V_EMF_ADC_CHANNEL);
    } else if(direction == DIRECTION_REVERSE) {
        adc_select_input(REV_V_EMF_ADC_CHANNEL);
    } else{
        set_error(INVALID_DIRECTION);
    }
    busy_wait_us(measurement_delay_us);
    adc_run(true);
    for (int32_t i = 0; i < total_iterations; ++i) {
        //Take adc value out of fifo when available, then do one step of insertion sort.
        adc_val[i] = adc_fifo_get_blocking();
        key = adc_val[i];
        j = i - 1;
        while (j >= 0 && adc_val[j] > key) {
            adc_val[j + 1] = adc_val[j];
            j--;
        }
        adc_val[j + 1] = key;
    }
    adc_run(false);
    //adc_fifo_drain();
    //discard x entries beginning from the lowest entry of the array (counting up); x = msr.l_side_arr_cutoff
    //discard y entries beginning from the highest index of the array (counting down); y = msr.r_side_arr_cutoff
    for (int32_t i = l_side_arr_cutoff; i < total_iterations - r_side_arr_cutoff ; ++i) {
        sum += adc_val[i];
    }
    float res = (float)sum / (float)( total_iterations - (l_side_arr_cutoff + r_side_arr_cutoff) );
    // return true to keep the repeating timer active
    return res;
}

// Get speed_step_table_index depending on speed_step
uint8_t get_speed_step_table_index_of_speed_step(uint8_t speed_step) {
    // Clear direction information in bit7
    speed_step = speed_step & 0b01111111;
    // Check for Stop or Emergency Stop speed step
    if (speed_step == 0 || speed_step == 1) {
        return 0;
    }
    // In any other case i.e. speed steps 1 to 126:
    return speed_step-1;
}


// This function gets called every x milliseconds where x is CV_175.
// The purpose of this function is to implement a time delay in acceleration or deceleration
void speed_helper(controller_parameter_t * ctrl_par) {
    // ctrl_par->setpoint only gets adjusted when the speed_helper_counter is equal to the accel_rate/decel_rate
    // -> Time for 1 Speed Step := (speed_helper timer delay)*(accel_rate) or CV_175*CV_3 or CV_175*CV_4
    const uint8_t accel_rate = CV_ARRAY_FLASH[2];
    const uint8_t decel_rate = CV_ARRAY_FLASH[3];
    static uint8_t speed_helper_counter;
    static uint8_t speed_table_index;

    // Get speed_step_table_end_index corresponding to current speed_step_target
    const uint8_t speed_step_table_end_index = get_speed_step_table_index_of_speed_step(speed_step_target);
    const bool direction_changed = get_direction_of_speed_step(speed_step_target) !=
                                   get_direction_of_speed_step(speed_step_target_prev);

    if (speed_step_target == SPEED_STEP_FORWARD_EMERGENCY_STOP || speed_step_target == SPEED_STEP_REVERSE_EMERGENCY_STOP) {
        // Emergency Stop
        speed_table_index = 0;
        speed_helper_counter = 0;
        ctrl_par->setpoint = ctrl_par->speed_table[speed_table_index];
    }
    else if (speed_step_table_end_index > speed_table_index && speed_helper_counter == accel_rate) {
        // Acceleration
        if (!accel_rate) speed_table_index = speed_step_table_end_index; // directly accelerate to end target speed
        else speed_table_index++;
        ctrl_par->setpoint = ctrl_par->speed_table[speed_table_index];
        speed_helper_counter = 0;
    }
    else if (speed_step_table_end_index < speed_table_index && speed_helper_counter == decel_rate) {
        // Deceleration
        if (!decel_rate) speed_table_index = speed_step_table_end_index; // directly decelerate to end target speed
        else speed_table_index--;
        ctrl_par->setpoint = ctrl_par->speed_table[speed_table_index];
        speed_helper_counter = 0;
    }
    else if (direction_changed) {
        // Change of direction while decelerating -> jump to most recent speed_step without delay (other direction)
        speed_table_index = speed_step_table_end_index;
        ctrl_par->setpoint = ctrl_par->speed_table[speed_table_index];
        speed_helper_counter = 0;
    }
    else if (speed_step_table_end_index != speed_table_index) {
        // Acceleration/Deceleration "scheduled"
        speed_helper_counter++;
    }
    else {
        // Target reached -> Reset Counter
        speed_helper_counter = 0;
    }
}

// Helper function to adjust pwm level/duty cycle.
void adjust_pwm_level(const uint16_t level) {
    direction_t dir =  get_direction_of_speed_step(speed_step_target);
    if (dir == DIRECTION_FORWARD) {
        // Forward
        pwm_set_gpio_level(MOTOR_REV_PIN, 0);
        pwm_set_gpio_level(MOTOR_FWD_PIN, level);
    }
    else if (dir == DIRECTION_REVERSE){
        // Reverse
        pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
        pwm_set_gpio_level(MOTOR_REV_PIN, level);
    }
}


// Returns proportional gain value corresponding to current setpoint
float get_kp(const controller_parameter_t *const ctrl_par) {
    const float sp = (float) ctrl_par->setpoint;
    if (sp < ctrl_par->pid.k_p_x_1) {
        return ctrl_par->pid.k_p_m_1 * sp + ctrl_par->pid.k_p_y_0;
    }
    return ctrl_par->pid.k_p_m_2 * (sp - ctrl_par->pid.k_p_x_1) + ctrl_par->pid.k_p_y_1;
}


uint16_t get_initial_level(controller_parameter_t *const ctrl_par){
    uint32_t sum = 0;
    uint32_t i = 0;
    while (i < BASE_PWM_ARR_LEN) {
        if (ctrl_par->startup.base_pwm_arr[i] > 0) {
            sum += ctrl_par->startup.base_pwm_arr[i];
            i++;
        }
        else {
            break;
        }
    }
    return  ((sum / i)*2)/3;
}

// Controller - Startup mode
void controller_startup_mode(controller_parameter_t *const ctrl_par) {
    if(ctrl_par->startup.level == 0) {
        ctrl_par->startup.level = get_initial_level(ctrl_par);
    }
    if (ctrl_par->measurement_corrected < 7.5f){
        const uint16_t max_level = _125M / (CV_ARRAY_FLASH[8] * 100 + 10000);
        adjust_pwm_level(ctrl_par->startup.level);
        ctrl_par->startup.level += max_level / 250;
        if (ctrl_par->startup.level > max_level) {
            // Try again with half value...
            ctrl_par->startup.level = get_initial_level(ctrl_par)/2;
        }
    }
    else {
        // Save level value in array
        ctrl_par->startup.base_pwm_arr[ctrl_par->startup.base_pwm_arr_i] = ctrl_par->startup.level;
        // Update index
        ctrl_par->startup.base_pwm_arr_i = (ctrl_par->startup.base_pwm_arr_i+1)%BASE_PWM_ARR_LEN;
        // Set ramp_up_mode flag to true
        ctrl_par->mode = PID_MODE;
        // multiply with constant value of 0.9 and set as current feed forward value for pid mode
        ctrl_par->feed_fwd = 0.9f * (float)ctrl_par->startup.level;
    }
}

// Controller - PID control mode
void controller_pid_mode(controller_parameter_t *const ctrl_par) {
    ctrl_par->pid.k_p = get_kp(ctrl_par);
    // Proportional part, integral part and derivative part (including digital low-pass-filter with time constant tau)
    float p = ctrl_par->pid.k_p * ctrl_par->pid.e;
    float i = ctrl_par->pid.ci_0 * (ctrl_par->pid.e + ctrl_par->pid.e_prev) + ctrl_par->pid.i_prev;
    float d = ctrl_par->pid.cd_0 * (ctrl_par->measurement - ctrl_par->measurement_prev) + ctrl_par->pid.cd_1 * ctrl_par->pid.d_prev;

    // Check for limits on the integral part
    if (i > ctrl_par->pid.int_lim_max) {
        i = ctrl_par->pid.int_lim_max;
    }
    else if (i < ctrl_par->pid.int_lim_min) {
        i = ctrl_par->pid.int_lim_min;
    }

    // Sum feed forward + all controller terms (p+i+d). Limit Output from 0% to 100% duty cycle
    float output_f = ctrl_par->feed_fwd + p + i + d;
    if (output_f < 0) {
        output_f = 0.0f;
    }
    else if (output_f > ctrl_par->pid.max_output) {
        output_f = ctrl_par->pid.max_output;
    }

    if (LOGLEVEL >= 1) {
        static int counter = 0;
        counter++;
        if (counter > 200) {
            LOG(1, "ctrl_par error(%f) output(%f)\n", ctrl_par->pid.e, output_f);
            counter = 0;
        }
    }

    // Set PWM Level and discard results in adc fifo
    //adjust_pwm_level((uint16_t) roundf(output_f));
    adjust_pwm_level((uint16_t) output_f);
    // Save previous error, integrator, differentiator values
    ctrl_par->pid.e_prev = ctrl_par->pid.e;
    ctrl_par->pid.i_prev = i;
    ctrl_par->pid.d_prev = d;
}

// General controller function gets called every x milliseconds where x is CV_49 i.e. sampling time (pid->t)
// TODO: Consider fixed point arithmetic to improve performance...
void controller_general(controller_parameter_t * ctrl_par) {
    // Change in direction -> reset previous derivative, error and integral parts and pwm_base_done
    const bool direction_changed = get_direction_of_speed_step(speed_step_target) !=
                                   get_direction_of_speed_step(speed_step_target_prev);
    // When setpoint == 0 there is nothing to control -> set output to 0, reset values and return
    if (!ctrl_par->setpoint || direction_changed) {
        ctrl_par->pid.d_prev = 0.0f;
        ctrl_par->pid.i_prev = 0.0f;
        ctrl_par->pid.e_prev = 0.0f;
        ctrl_par->mode = STARTUP_MODE;
        ctrl_par->startup.level = 0;
        adjust_pwm_level(0);
        return;
    }

    // Measure BEMF voltage and compute error
    ctrl_par->measurement = measure(ctrl_par->msr_total_iterations,
                                    ctrl_par->msr_delay_in_us,
                                    ctrl_par->l_side_arr_cutoff,
                                    ctrl_par->r_side_arr_cutoff,
                                    get_direction_of_speed_step(speed_step_target));
    ctrl_par->measurement_corrected = ctrl_par->measurement - ctrl_par->adc_offset;
    ctrl_par->pid.e = (float) ctrl_par->setpoint - ctrl_par->measurement_corrected;

    switch (ctrl_par->mode){
        case STARTUP_MODE:
            controller_startup_mode(ctrl_par);
            break;
        case PID_MODE:
            controller_pid_mode(ctrl_par);
            break;
    }

    adc_fifo_drain();

    // Save measurement value
    ctrl_par->measurement_prev = ctrl_par->measurement;
}


// PID controller and measurement parameter, and speed_table initialization
void init_controller(controller_parameter_t *const ctrl_par) {
    LOG(1, "Motor controller initialization...\n")
    // General controller variables
    ctrl_par->mode = STARTUP_MODE;
    ctrl_par->feed_fwd = 0;

    // Startup controller variables
    ctrl_par->startup.level = 0;
    ctrl_par->startup.base_pwm_arr_i = 0;
    for (int i = 0; i < BASE_PWM_ARR_LEN; ++i) {
        ctrl_par->startup.base_pwm_arr[i] = 0;
    }

    // Measurement variables initialization
    ctrl_par->adc_offset = (float) CV_ARRAY_FLASH[171];
    ctrl_par->msr_total_iterations = CV_ARRAY_FLASH[60];
    ctrl_par->msr_delay_in_us = CV_ARRAY_FLASH[61];
    ctrl_par->l_side_arr_cutoff = CV_ARRAY_FLASH[62];
    ctrl_par->r_side_arr_cutoff = CV_ARRAY_FLASH[63];

    // Speed table initialization
    // Calculate speed setpoint table according to V_min, V_max and V_mid CVs
    const double v_min = (double) CV_ARRAY_FLASH[1];
    const double v_mid = (double) CV_ARRAY_FLASH[5] * 16;
    const double v_max = (double) CV_ARRAY_FLASH[4] * 16;

    const double delta_x = 63;
    const double m_1 = (v_mid - v_min) / delta_x;
    const double m_2 = (v_max - v_mid) / delta_x;
    ctrl_par->speed_table[0] = 0;
    for (uint8_t i = 1; i < 64; ++i) {
        ctrl_par->speed_table[i] = (uint16_t) lround(m_1 * (i - 1) + v_min);
    }
    for (uint8_t i = 64; i < 127; ++i) {
        ctrl_par->speed_table[i] = (uint16_t) lround(m_2 * (i - 63) + v_mid);
    }

    // PID Controller initialization
    ctrl_par->pid.k_i = (float) CV_ARRAY_FLASH[49] / 10;
    ctrl_par->pid.k_d = (float) CV_ARRAY_FLASH[50] / 10000;
    ctrl_par->pid.tau = (float) CV_ARRAY_FLASH[47] / 1000;
    ctrl_par->pid.t = (float) CV_ARRAY_FLASH[48] / 1000;
    ctrl_par->pid.ci_0 = (ctrl_par->pid.k_i * ctrl_par->pid.t) / 2;
    ctrl_par->pid.cd_0 = -(2 * ctrl_par->pid.k_d) / (2 * ctrl_par->pid.tau + ctrl_par->pid.t);
    ctrl_par->pid.cd_1 = (2 * ctrl_par->pid.tau - ctrl_par->pid.t) / (2 * ctrl_par->pid.tau + ctrl_par->pid.t);
    ctrl_par->pid.int_lim_max = 10 * (float) CV_ARRAY_FLASH[51];
    ctrl_par->pid.int_lim_min = -10 * (float) CV_ARRAY_FLASH[52];
    ctrl_par->pid.max_output = (float) (_125M / (CV_ARRAY_FLASH[8] * 100 + 10000));
    ctrl_par->pid.e_prev = 0.0f;
    ctrl_par->pid.i_prev = 0.0f;
    ctrl_par->pid.d_prev = 0.0f;
    ctrl_par->pid.k_p_x_1_shift = (float) CV_ARRAY_FLASH[59] / 255.0f;
    ctrl_par->pid.k_p_x_1 = (float) ctrl_par->speed_table[126] * ctrl_par->pid.k_p_x_1_shift;
    ctrl_par->pid.k_p_x_2 = (float) ctrl_par->speed_table[126] * (1.0f - ctrl_par->pid.k_p_x_1_shift);
    ctrl_par->pid.k_p_y_0 = (float) get_16bit_CV(53) / 100.0f;
    ctrl_par->pid.k_p_y_1 = (float) get_16bit_CV(55) / 100.0f;
    ctrl_par->pid.k_p_y_2 = (float) get_16bit_CV(57) / 100.0f;
    ctrl_par->pid.k_p_m_1 = (ctrl_par->pid.k_p_y_1 - ctrl_par->pid.k_p_y_0) / ctrl_par->pid.k_p_x_1;
    ctrl_par->pid.k_p_m_2 = (ctrl_par->pid.k_p_y_2 - ctrl_par->pid.k_p_y_1) / ctrl_par->pid.k_p_x_2;

    // General Controller initialization
    ctrl_par->measurement = 0.0f;
    ctrl_par->measurement_prev = 0.0f;
    ctrl_par->measurement_corrected = 0.0f;
    ctrl_par->setpoint = 0;
    LOG(1, "Motor controller initialization done!\n")
}

bool controller_timer_callback(__unused struct repeating_timer *t) {
    controller_flag = true;
    return true;
}

bool speed_helper_timer_callback(__unused struct repeating_timer *t) {
    speed_helper_flag = true;
    return true;
}


void core1_entry() {
    LOG(1, "core1 Initialization...\n");

    flash_safe_execute_core_init_done = flash_safe_execute_core_init();
    if (flash_safe_execute_core_init_done != true){
        set_error(FLASH_SAFE_EXECUTE_CORE_INIT_FAILURE);
        return;
    }

    // Wait for core0 to finish CV check (see cv_setup_check)
    LOG(1, "Waiting for CV check flag...\n");
    while(!cv_setup_check_done){
        watchdog_update(); // Update watchdog while waiting on core0
    }

    controller_parameter_t control_parameter;
    controller_parameter_t *ctrl_par = &control_parameter;
    init_controller(ctrl_par);
    
    struct repeating_timer timer_controller, timer_speed_helper;
    add_repeating_timer_ms(-CV_ARRAY_FLASH[48], controller_timer_callback, NULL, &timer_controller);
    add_repeating_timer_ms(-CV_ARRAY_FLASH[174], speed_helper_timer_callback, NULL, &timer_speed_helper);

    LOG(1, "core1 initialization done!\n");

    // Endless loop
    while (true) {
        if (controller_flag) {
            controller_general(ctrl_par);
            controller_flag = false;
        }
        else if (speed_helper_flag) {
            speed_helper(ctrl_par);
            speed_helper_flag = false;
        }
        else {
            watchdog_update();
        }
        
    }
}
