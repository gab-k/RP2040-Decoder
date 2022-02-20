//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core1.c         //
//////////////////////////

#include "core1.h"

uint16_t current_target = 0;
float current_target_voltage = 0;
struct repeating_timer pid_control_timer,speed_helper_timer;

typedef struct pid_params{
    float k_p;
    float t_s;
    float a_1;
    float a_2;
    float sum_limit_max;
    float sum_limit_min;
    float output;
    float e;
    float e_prev;
    float e_sum;
    uint startup_offset_fwd;
    uint startup_offset_rev;
    bool direction_prev;
}pid_params;
pid_params pid;

typedef struct measure_params{
    uint8_t delay_in_us;
    uint8_t total_iterations;
    uint8_t left_side_array_cutoff;
    uint8_t right_side_array_cutoff;
}measure_params;
measure_params msr;
uint8_t a_factor = 50; /*CV_ARRAY_FLASH[59];..................*/

uint16_t calc_end_target(){
    //Forward
    if(target_direction){
        if (target_speed_step == 128) return 0;
        else return (target_speed_step-129) * a_factor ; /* (float) CV_ARRAY_FLASH[59]*/
    }
    //Reverse
    else {
        if (target_speed_step == 0) return 0;
        else return (target_speed_step-1) * a_factor; /*(float)CV_ARRAY_FLASH[59]*/
    }
}
bool speed_helper() {
    uint8_t accel_rate = CV_ARRAY_FLASH[2];
    uint8_t decel_rate = CV_ARRAY_FLASH[3];
    uint16_t end_target = calc_end_target();
    //current_target only gets adjusted when the counter equals the acc/dec rate
    //-> Time for 1 Speed Step := (speed_helper timer delay)*(accel_rate)
    static uint8_t speed_helper_counter;
    //Emergency Stop
    if (target_speed_step == 129 || target_speed_step==1) {
        current_target = 0;
        speed_helper_counter = 0;
    }
    //Acceleration
    else if (end_target > current_target && speed_helper_counter == accel_rate) {
        if(!accel_rate) current_target = end_target;    // directly accelerate to end target speed
        else current_target += a_factor;
        speed_helper_counter = 0;
    }
    //Deceleration
    else if (end_target < current_target && speed_helper_counter == decel_rate) {
        if(!decel_rate) current_target = end_target;    // directly decelerate to end target speed
        else current_target -= a_factor;
        speed_helper_counter = 0;
    }
    //Acceleration/Deceleration "scheduled"
    else if (end_target != current_target) speed_helper_counter++;
    //Target reached -> Reset Counter
    else speed_helper_counter = 0;
    current_target_voltage = (float)current_target/1260;
    return true;
}
float measure(bool direction){
    uint32_t sum = 0;
    uint16_t adc_val[100];
    pwm_set_gpio_level(MOTOR_PWM_FORWARD,0);
    pwm_set_gpio_level(MOTOR_PWM_REVERSE,0);
    adc_select_input(!direction);
    busy_wait_us(msr.delay_in_us);
    adc_run(true);
    int key, j;
    for (int i = 0; i < msr.total_iterations; ++i) {
        //Take adc value out of fifo when available, then do one step of insertion sort.
        adc_val[i] = adc_fifo_get_blocking();
        key = adc_val[i];
        j = i - 1;
        while (j >= 0 && adc_val[j] > key) {
            adc_val[j + 1] = adc_val[j];
            j = j - 1;
        }
        adc_val[j + 1] = key;
    }
    adc_run(false);
    //discard x entries beginning from the lowest entry of the array (counting up); x = msr.left_side_array_cutoff
    //discard y entries beginning from the highest index of the array (counting down); y = msr.right_side_array_cutoff
    for (uint8_t i = msr.left_side_array_cutoff; i < msr.total_iterations - msr.right_side_array_cutoff ; ++i) {
        sum += adc_val[i];
    }
    //V_z is represented in 12bit
    float V_z = (float)sum / (float)(msr.total_iterations - (msr.left_side_array_cutoff+msr.right_side_array_cutoff));
    //Zener correction (approximated) & Conversion factor for V_z <= 1951
    //Function Returns Value in "Volts"
    if (V_z > 1951){
        float V_z_sq = V_z * V_z;
        return ( C0 + C1 * V_z + C2 * V_z_sq + C3 * V_z * V_z_sq + C4 * V_z_sq * V_z_sq);
    }
    else return (L1*V_z);
}
void adjust_pwm_level(uint16_t level) {
    if (target_direction) {
        pwm_set_gpio_level(MOTOR_PWM_REVERSE,0);
        pwm_set_gpio_level(MOTOR_PWM_FORWARD,level);
    }
        //Reverse
    else {
        pwm_set_gpio_level(MOTOR_PWM_FORWARD,0);
        pwm_set_gpio_level(MOTOR_PWM_REVERSE,level);
    }
}
void set_pid_params(float v){
    float v_max = (float)a_factor/10;
    pid.t_s = 5.0f / 1000;
    pid.k_p = 200.0f;
    pid.k_p = (-200000*current_target_voltage)/v_max+200000.0f;
    pid.a_1 = 200.0f * pid.t_s;                             //  k_i*t_s
    pid.a_2 = 0.0f / pid.t_s;                               //  k_d/t_s
}

bool pid_control(struct repeating_timer *t){
    pid.output += (float)(pid.startup_offset_fwd*target_direction + pid.startup_offset_rev*(!target_direction));
    float measured_voltage = measure(target_direction);
    if(target_direction != pid.direction_prev) pid.e_sum = 0.0f;
    pid.e = current_target_voltage - (measured_voltage - ADC_OFFSET);
    pid.e_sum += pid.e;
    if (pid.e_sum > pid.sum_limit_max) pid.e_sum = pid.sum_limit_max;
    else if (pid.e_sum < pid.sum_limit_min) pid.e_sum = pid.sum_limit_min;
    pid.output  =   (pid.k_p * (float)pid.e) +
                    (pid.a_1 * (float)pid.e_sum) +                         //  k_i*t_s*e_sum
                    (pid.a_2 * (float)(pid.e - pid.e_prev));               //  (k_d/t_s)*(e-e_prev)
    if (pid.output < 0 || current_target == 0) pid.output = 0;
    else if (pid.output > 5000) pid.output = 5000;
    adjust_pwm_level((uint16_t )pid.output);
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
    //    pid.k_p = ((float)CV_ARRAY_FLASH[48] / 256);
    //    pid.t_s = ((float)CV_ARRAY_FLASH[47] / 1000);
    //    pid.a_1 = ((float)CV_ARRAY_FLASH[49] / 64)   * pid.t_s ;         //  k_i*t_s
    //    pid.a_2 = ((float)CV_ARRAY_FLASH[50] / 4096) / pid.t_s;          //  k_d/t_s
    //pid.k_p = (-200000*current_target_voltage)/v_max+200000.0f;
    pid.k_p = 200.0f;
    pid.t_s = (float) CV_ARRAY_FLASH[47] / 1000;
    pid.a_1 = 2000.0f * pid.t_s;                             //  k_i*t_s
    pid.a_2 = 0.0f / pid.t_s;                               //  k_d/t_s
    pid.sum_limit_max = 100.0f;/*(float)(CV_ARRAY_FLASH[51] );*/
    pid.sum_limit_min = -100.0f;/*(float)(CV_ARRAY_FLASH[52] * (-1));*/
    pid.direction_prev = target_direction;
    pid.startup_offset_fwd = (CV_ARRAY_FLASH[53]) + (CV_ARRAY_FLASH[54]<<8);
    pid.startup_offset_rev = (CV_ARRAY_FLASH[55]) + (CV_ARRAY_FLASH[56]<<8);
}
void init_pwm_core1(uint gpio) {
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint32_t wrap_counter = 5000;                  // 5000 Cycles @ 125MHz for 1 Period -> 25kHz
    pwm_set_wrap(slice_num, wrap_counter);
    pwm_set_gpio_level(gpio,0);
    pwm_set_enabled(slice_num, true);
}
void core1_entry() {
    printf("core1 init...\n");
    if(gpio_get_function(MOTOR_PWM_FORWARD)!=4){
        gpio_set_function(MOTOR_PWM_FORWARD, GPIO_FUNC_PWM);
        gpio_set_function(MOTOR_PWM_REVERSE, GPIO_FUNC_PWM);
        adc_init();
        adc_gpio_init(V_EMF_ADC_PIN_FORWARD);
        adc_gpio_init(V_EMF_ADC_PIN_REVERSE);
        adc_fifo_setup(true,false,0,false,false);
        init_pwm_core1(MOTOR_PWM_FORWARD);
        init_pwm_core1(MOTOR_PWM_REVERSE);
        init_pid();
    }
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
