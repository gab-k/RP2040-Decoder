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
    // Forward
    if(speed_step_target > 127) {
        pid->direction = true;
        if (pid->speed_step_target == 128) pid->end_target_index = 0;
        else pid->end_target_index = (pid->speed_step_target-129);
    }
        // Reverse
    else {
        pid->direction = false;
        if (pid->speed_step_target == 0) pid->end_target_index = 0;
        else pid->end_target_index = (pid->speed_step_target-1);
    }
}

bool speed_helper(struct repeating_timer *t) {
    pid_params *pid = (pid_params *)(t->user_data);
    uint8_t accel_rate = CV_ARRAY_FLASH[2];
    uint8_t decel_rate = CV_ARRAY_FLASH[3];
    static uint8_t speed_helper_counter;
    static uint8_t speed_table_index;
    //latest_setpoint only gets adjusted when the counter equals the acc/dec rate
    //-> Time for 1 Speed Step := (speed_helper timer delay)*(accel_rate)
    //Emergency Stop
    if (pid->speed_step_target == 129 || pid->speed_step_target==1) {
        speed_table_index = 0;
        speed_helper_counter = 0;
        pid->setpoint = pid->speed_table[speed_table_index];
    }
    //Acceleration
    else if (pid->end_target_index > speed_table_index && speed_helper_counter == accel_rate) {
        if(!accel_rate) speed_table_index = pid->end_target_index;  // directly accelerate to end target speed
        else speed_table_index++;
        pid->setpoint = pid->speed_table[speed_table_index];
        speed_helper_counter = 0;
    }
    //Deceleration
    else if (pid->end_target_index < speed_table_index && speed_helper_counter == decel_rate) {
        if(!decel_rate) speed_table_index = pid->end_target_index;    // directly decelerate to end target speed
        else speed_table_index--;
        pid->setpoint = pid->speed_table[speed_table_index];
        speed_helper_counter = 0;
    }
    //Acceleration/Deceleration "scheduled"
    else if (pid->end_target_index != speed_table_index) speed_helper_counter++;
    //Target reached -> Reset Counter
    else speed_helper_counter = 0;
    return true;
}
float measure(pid_params *pid, bool direction){
    uint32_t sum = 0;
    uint16_t adc_val[100 /*msr.total_iterations*/];
    int32_t j = 0;
    uint16_t key;
    pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
    pwm_set_gpio_level(MOTOR_REV_PIN, 0);
    adc_select_input((!direction)+2);
    busy_wait_us(pid->msr_delay_in_us);
    adc_run(true);
    for (int32_t i = 0; i < pid->msr_total_iterations; ++i) {
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
    //discard x entries beginning from the lowest entry of the array (counting up); x = msr.l_side_arr_cutoff
    //discard y entries beginning from the highest index of the array (counting down); y = msr.r_side_arr_cutoff
    for (int32_t i = pid->l_side_arr_cutoff; i < pid->msr_total_iterations - pid->r_side_arr_cutoff ; ++i) {
        sum += adc_val[i];
    }
    float res = (float)sum / (float)( pid->msr_total_iterations - (pid->l_side_arr_cutoff + pid->r_side_arr_cutoff) );
    // return true to keep the repeating timer active
    return res;
}
void adjust_pwm_level(uint16_t level, pid_params *pid) {
    if (pid->direction) {
        pwm_set_gpio_level(MOTOR_REV_PIN, 0);
        pwm_set_gpio_level(MOTOR_FWD_PIN, level);
    }
        //Reverse
    else {
        pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
        pwm_set_gpio_level(MOTOR_REV_PIN, level);
    }
}
void update_m(pid_params *pid){
    pid->m_fwd = (pid->y_2_fwd-pid->y_1_fwd)/(pid->x_2_fwd-pid->x_1_fwd);
    pid->m_rev = (pid->y_2_rev-pid->y_1_rev)/(pid->x_2_rev-pid->x_1_rev);
}
void update_y(pid_params *pid,float i){
    // Forward
    if(pid->direction) {
        pid->y_2_fwd += i*pid->k_ff;
        printf("%f\n",pid->y_2_fwd);
    }
    // Reverse
    else{
        pid->y_2_rev += i*pid->k_ff;
        printf("%f\n",pid->y_2_rev);
    }
    update_m(pid);
}
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
float get_kp(pid_params *pid){
    float sp = (float)pid->setpoint;
    float x_1 = (float)pid->speed_table[126]*0.05f;
    float x_2 = (float)pid->speed_table[126]*0.95f;
    float y_1 = 60.0f;
    float y_2 = 4.0f;
    float y_3 = 1.5f;
    float m_1 = (y_2-y_1)/x_1;
    float m_2 = (y_3-y_2)/x_2;
    float k_p;
    if (sp < x_1){
        k_p = m_1*sp+y_1;
    }
    else{
        k_p = m_2*(sp-x_1)+y_2;
    }
    return k_p;
}
bool pid_control(struct repeating_timer *t){
    pid_params *pid = (pid_params *)(t->user_data);
    // Acceleration from standstill or change in direction -> previous derivative and integral parts are reset
    if((!pid->setpoint_prev && !pid->setpoint) || (pid->direction != pid->direction_prev)){
        pid->derivative_prev = 0.0f;
        pid->integral_prev = 0.0f;
    }
    float feed_fwd = get_ff_val(pid);
    pid->k_p = get_kp(pid);
    // Measure BEMF voltage - proportional to the rotational speed of the motor
    //absolute_time_t from = get_absolute_time();
    pid->measurement = measure(pid,pid->direction);
    // Calculate error
    float e = (float)pid->setpoint - (pid->measurement - pid->adc_offset);
    // Proportional
    float p = pid->k_p * e;
    // Integral
    float i = pid->ci_0*(e + pid->e_prev) + pid->integral_prev;
    // Check for limits on the integral part
    if (i > pid->int_lim_max)i = pid->int_lim_max;
    else if (i < pid->int_lim_min)i = pid->int_lim_min;
    // Derivative and digital low-pass-filter with time constant tau
    float d = pid->cd_0*(pid->measurement-pid->measurement_prev)+pid->cd_1*pid->derivative_prev;
    // Sum feed forward + all controller terms (p+i+d)
    float output = feed_fwd+p+i+d;
    // Limit Output from 0% to 100% duty cycle
    if (output < 0 || !pid->setpoint) output = 0.0f;
    else if (output > pid->max_output) output = pid->max_output;
    // Set PWM Level
    adjust_pwm_level((uint16_t)roundf(output),pid);
    //absolute_time_t to = get_absolute_time();
    //int64_t bla = absolute_time_diff_us(from,to);
   // printf("%f\n",i);
    // Discard results in adc fifo
    adc_fifo_drain();
    // Check for High or Low Integrator values -> Update x,y values of feedforward function
    if( (output < 5000 && output > 2500) && ((i > pid->int_lim_max/2) || (i < pid->int_lim_min/2)) ) update_y(pid,i);
    // Save previous error, integrator, differentiator and direction
    pid->e_prev = e;
    pid->integral_prev = i;
    pid->derivative_prev = d;
    pid->direction_prev = pid->direction;
    pid->setpoint_prev = pid->setpoint;
    pid->measurement_prev = pid->measurement;
    // return true to keep the repeating timer active
    return true;
}

uint16_t measure_base_pwm(pid_params *pid,bool direction){
    pid->direction = direction;
    uint16_t level = 750;
    while(measure(pid,direction)<20.0f){
        adjust_pwm_level(level,pid);
        busy_wait_ms(50);
        level += 25;
    }
    return level;
}
void init_pid(pid_params *pid){
    // Measurement variables initialization
    pid->adc_offset = 13.0f;            //(float)CV_ARRAY_FLASH[171];
    pid->msr_total_iterations = 100;    //CV_ARRAY_FLASH[60];
    pid->msr_delay_in_us = 100;         //CV_ARRAY_FLASH[61];
    pid->l_side_arr_cutoff = 15;        //CV_ARRAY_FLASH[62];
    pid->r_side_arr_cutoff = 15;        //CV_ARRAY_FLASH[63];
    // Speed table initialization
    // Calculate speed setpoint table according to V_min, V_max and V_mid CVs
    double v_min = (double)CV_ARRAY_FLASH[1];
    double v_mid = (double)CV_ARRAY_FLASH[5]*16;
    double v_max = (double)CV_ARRAY_FLASH[4]*16;
    double m_1 = (v_mid-v_min)/62;
    double m_2 = (v_max-v_mid)/63;
    pid->speed_table[0] = 0;
    for (uint8_t i = 1; i < 63; ++i) {
        pid->speed_table[i] = (uint16_t)lround(m_1*(i-1)+v_min);
    }
    for (uint8_t i = 63; i < 127; ++i) {
        pid->speed_table[i] = (uint16_t)lround(m_2*(i-63)+v_mid);
    }
    // Control variables initialization
    pid->k_p = 5.0f;
    pid->k_i = 2.0f;
    pid->k_d = 0.005f;
    pid->tau = (float)CV_ARRAY_FLASH[46]/1000;
    pid->t = (float)CV_ARRAY_FLASH[47]/1000;
    pid->ci_0 = (pid->k_i*pid->t)/2;
    pid->cd_0 = -(2*pid->k_d)/(2*pid->tau+pid->t);
    pid->cd_1 = (2*pid->tau-pid->t)/(2*pid->tau+pid->t);
    pid->int_lim_max = 1000;
    pid->int_lim_min = -1000;
    pid->direction = true;
    pid->direction_prev = true;
    uint16_t max_output = _125M/(CV_ARRAY_FLASH[8]*100+10000);
    pid->max_output = (float)max_output;
    pid->y_1_fwd = (float)measure_base_pwm(pid,1);
    pid->y_1_rev = (float)measure_base_pwm(pid,0);
    pid->y_2_fwd = pid->max_output*0.75f;
    pid->y_2_rev = pid->max_output*0.75f;
    pid->x_1_rev = 0.0f;
    pid->x_1_fwd = 0.0f;
    pid->x_2_rev = (float)pid->speed_table[94];
    pid->x_2_fwd = (float)pid->speed_table[94];
    pid->k_ff = 0.005f;
    update_m(pid);
}
void init_motor_pwm(uint8_t gpio) {
    uint16_t wrap_counter = _125M/(CV_ARRAY_FLASH[8]*100+10000);    // 125MHz / f_pwm
    uint32_t slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_clkdiv_int_frac(slice_num,CV_ARRAY_FLASH[173],0);
    pwm_set_wrap(slice_num, wrap_counter);
    pwm_set_gpio_level(gpio,0);
    pwm_set_enabled(slice_num, true);
}


void core1_entry() {
    printf("core1 init...\n");
    if(gpio_get_function(MOTOR_FWD_PIN) != 4){
        gpio_set_function(MOTOR_FWD_PIN, GPIO_FUNC_PWM);
        gpio_set_function(MOTOR_REV_PIN, GPIO_FUNC_PWM);
        adc_init();
        adc_gpio_init(FWD_V_EMF_ADC_PIN);
        adc_gpio_init(REV_V_EMF_ADC_PIN);
        adc_fifo_setup(true,false,0,false,false);
        init_motor_pwm(MOTOR_FWD_PIN);
        init_motor_pwm(MOTOR_REV_PIN);
    }
    pid_params pid_parameters;
    pid_params *pid = &pid_parameters;
    init_pid(pid);
    alarm_pool_add_repeating_timer_ms(alarm_pool_create(0,1),
                                      CV_ARRAY_FLASH[47],
                                      pid_control,
                                      pid,
                                      &pid_control_timer);
    irq_set_priority(0,0x00); // 0x00 = highest priority
    alarm_pool_add_repeating_timer_ms(alarm_pool_create(1,1),
                                      CV_ARRAY_FLASH[58],
                                      speed_helper,
                                      pid,
                                      &speed_helper_timer);
    // Send direction pointer for access and to let core0 know that core1 is done with initialization,
    // Note: Pointer will be cast into uint32_t and then back into bool * on core0 (Both 32-Bit Datatype).
    // This is done only to accommodate the correct data type associated with multicore_fifo_push_blocking().
    multicore_fifo_push_blocking((uint32_t) &(pid->direction));
    printf("core1 done\n");
    while (1){
        // If core0 sends new speed data to core1 speed_step_target and direction variables get updated
        if(multicore_fifo_rvalid()) { update_speed_dir(pid, multicore_fifo_pop_blocking()); }
    }
}