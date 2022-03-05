//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core1.c         //
//////////////////////////

#include "core1.h"

uint32_t latest_target = 0;
struct repeating_timer pid_control_timer,speed_helper_timer;

typedef struct pid_params{
    int32_t k_p;
    int32_t k_i;
    int32_t k_d;
    int32_t t_s;
    int32_t sum_limit_max;
    int32_t sum_limit_min;
    int32_t output;
    int32_t e;
    int32_t e_prev;
    int32_t e_sum;
    uint32_t m_fwd_scaled;
    uint32_t m_rev_scaled;
    uint32_t b_fwd;
    uint32_t b_rev;
    uint16_t max_output;
    bool direction_prev;
}pid_params;
pid_params pid;

typedef struct measure_params{
    uint32_t val;
    uint8_t delay_in_us;
    uint8_t total_iterations;
    uint8_t left_side_array_cutoff;
    uint8_t right_side_array_cutoff;
}measure_params;
measure_params msr;


uint8_t target_factor = 10; /*CV_ARRAY_FLASH[59];*/ // One Discrete Step

uint16_t calc_end_target(){
    //Forward
    if(target_direction){
        if (target_speed_step == 128) return 0;
        else return (target_speed_step-129) * target_factor ; /* (float) CV_ARRAY_FLASH[59]*/
    }
    //Reverse
    else {
        if (target_speed_step == 0) return 0;
        else return (target_speed_step-1) * target_factor; /*(float)CV_ARRAY_FLASH[59]*/
    }
}
bool speed_helper() {
    uint8_t accel_rate = CV_ARRAY_FLASH[2];
    uint8_t decel_rate = CV_ARRAY_FLASH[3];
    uint16_t end_target = calc_end_target();
    //latest_target only gets adjusted when the counter equals the acc/dec rate
    //-> Time for 1 Speed Step := (speed_helper timer delay)*(accel_rate)
    static uint8_t speed_helper_counter;
    //Emergency Stop
    if (target_speed_step == 129 || target_speed_step==1) {
        latest_target = 0;
        speed_helper_counter = 0;
    }
    //Acceleration
    else if (end_target > latest_target && speed_helper_counter == accel_rate) {
        if(!accel_rate) latest_target = end_target;    // directly accelerate to end target speed
        else latest_target += target_factor;
        speed_helper_counter = 0;
    }
    //Deceleration
    else if (end_target < latest_target && speed_helper_counter == decel_rate) {
        if(!decel_rate) latest_target = end_target;    // directly decelerate to end target speed
        else latest_target -= target_factor;
        speed_helper_counter = 0;
    }
    //Acceleration/Deceleration "scheduled"
    else if (end_target != latest_target) speed_helper_counter++;
    //Target reached -> Reset Counter
    else speed_helper_counter = 0;
    return true;
}
uint32_t measure(bool direction){
    uint32_t sum = 0;
    uint16_t adc_val[100 /*msr.total_iterations*/];
    int16_t j = 0;
    uint16_t key;
    pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
    pwm_set_gpio_level(MOTOR_REV_PIN, 0);
    adc_select_input(!direction);
    busy_wait_us(msr.delay_in_us);
    adc_run(true);
    for (int16_t i = 0; i < msr.total_iterations; ++i) {
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
    //discard x entries beginning from the lowest entry of the array (counting up); x = msr.left_side_array_cutoff
    //discard y entries beginning from the highest index of the array (counting down); y = msr.right_side_array_cutoff
    for (uint8_t i = msr.left_side_array_cutoff; i < msr.total_iterations - msr.right_side_array_cutoff ; ++i) {
        sum += adc_val[i];
    }
    uint32_t res = sum / ( msr.total_iterations - (msr.left_side_array_cutoff+msr.right_side_array_cutoff) );
    return res;
}
void adjust_pwm_level(uint16_t level) {
    if (target_direction) {
        pwm_set_gpio_level(MOTOR_REV_PIN, 0);
        pwm_set_gpio_level(MOTOR_FWD_PIN, level);
    }
        //Reverse
    else {
        pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
        pwm_set_gpio_level(MOTOR_REV_PIN, level);
    }
}
void init_offsets(uint16_t lvl_max){
    uint16_t lvl_min_fwd = (CV_ARRAY_FLASH[53]) + (CV_ARRAY_FLASH[54]<<8);
    uint16_t lvl_min_rev = (CV_ARRAY_FLASH[55]) + (CV_ARRAY_FLASH[56]<<8);
    pid.m_fwd_scaled = ((lvl_max-lvl_min_fwd)*1000) / (125*target_factor);  //scaled with 1000 due to integer division
    pid.m_rev_scaled = ((lvl_max-lvl_min_rev)*1000) / (125*target_factor);  //scaled with 1000 due to integer division
    pid.b_fwd = lvl_min_fwd - ((pid.m_fwd_scaled*target_factor) / 1000) - 200;
    pid.b_rev = lvl_min_rev - ((pid.m_rev_scaled*target_factor) / 1000) - 200;
}
uint32_t offset(){
    // f(x) = mx+b   -   with x = latest_target
    if (target_direction)   return (pid.m_fwd_scaled*latest_target)/1000 + pid.b_fwd;
    else                    return (pid.m_rev_scaled*latest_target)/1000 + pid.b_rev;
}
bool pid_control(struct repeating_timer *t){
    if(target_direction != pid.direction_prev || !latest_target) pid.e_sum = 0;
    msr.val = measure(target_direction);
    pid.e = (int32_t)latest_target - (int32_t)msr.val + ADC_OFFSET;
    pid.e_sum += pid.e;
    if (pid.e_sum > pid.sum_limit_max) pid.e_sum = pid.sum_limit_max;
    else if (pid.e_sum < pid.sum_limit_min) pid.e_sum = pid.sum_limit_min;
    pid.output = ( pid.k_p*pid.e + pid.k_i*pid.t_s*pid.e_sum + (pid.k_d/pid.t_s)*(pid.e-pid.e_prev) ) / 1000;
    pid.output += (int32_t)offset();
    if (pid.output < 0 || !latest_target) pid.output = 0;
    else if (pid.output > pid.max_output) pid.output = pid.max_output;
    adjust_pwm_level(pid.output);
    //printf("o: %d\n",pid.output);
    //printf("e: %d\n",pid.e);
    adc_fifo_drain();
    pid.direction_prev = target_direction;
    pid.e_prev = pid.e;
    return true;
}
void init_pid(){
    msr.total_iterations = 100;/*CV_ARRAY_FLASH[60];*/
    msr.delay_in_us = 100; /*CV_ARRAY_FLASH[61];*/
    msr.left_side_array_cutoff = 15;/*CV_ARRAY_FLASH[62];*/
    msr.right_side_array_cutoff = 15;/*CV_ARRAY_FLASH[63];*/
    pid.e_sum = 0;
    pid.e_prev = 0;
    pid.k_p = 2500;
    pid.k_i = 100;
    pid.k_d = 100;
    pid.t_s = 5  /*(float) CV_ARRAY_FLASH[47]*/;
    pid.sum_limit_max = 1000;/*(float)(CV_ARRAY_FLASH[51] );*/
    pid.sum_limit_min = -1000;/*(float)(CV_ARRAY_FLASH[52] * (-1));*/
    pid.direction_prev = target_direction;
    pid.max_output = _125M/(CV_ARRAY_FLASH[8]*100+10000);
}
void init_motor_pwm(uint8_t gpio) {
    uint16_t wrap_counter = _125M/(CV_ARRAY_FLASH[8]*100+10000);        //  125MHz / f_pwm
    init_offsets(wrap_counter);
    uint32_t slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_clkdiv_int_frac(slice_num,CV_ARRAY_FLASH[46],0);
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
    init_pid();
    alarm_pool_add_repeating_timer_ms(alarm_pool_create(0,1),
                                      CV_ARRAY_FLASH[47],
                                      pid_control,
                                      NULL,
                                      &pid_control_timer);
    irq_set_priority(0,0x00); // 0x00 = highest priority
    alarm_pool_add_repeating_timer_ms(alarm_pool_create(1,1),
                                      CV_ARRAY_FLASH[58],
                                      speed_helper,
                                      NULL,
                                      &speed_helper_timer);
    printf("core1 done\n");
    while (1);
}
