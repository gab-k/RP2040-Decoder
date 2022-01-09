//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      core1.c         //
//////////////////////////

#include "inttypes.h"
#include "core1.h"
#include "shared.h"
uint current_target = 0;
struct repeating_timer pid_control_timer,speed_helper_timer;

typedef struct pid_params{
    float k_p;
    float t_s;
    float a_1;
    float a_2;
    int sum_limit_max;
    int sum_limit_min;
    float output;
    int e;
    int e_prev;
    int e_sum;
}pid_params;
pid_params pid;
typedef struct measure_params{
    uint8_t delay_in_us;
    uint8_t total_iterations;
    uint8_t left_side_array_cutoff;
    uint8_t right_side_array_cutoff;
}measure_params;
measure_params msr;

void quicksort(uint16_t number[], int first, int last){
    int i, j, pivot, temp;
    if(first<last){
        pivot=first;
        i=first;
        j=last;
        while(i<j){
            while(number[i]<=number[pivot]&&i<last)
                i++;
            while(number[j]>number[pivot])
                j--;
            if(i<j){
                temp=number[i];
                number[i]=number[j];
                number[j]=temp;
            }
        }
        temp=number[pivot];
        number[pivot]=number[j];
        number[j]=temp;
        quicksort(number,first,j-1);
        quicksort(number,j+1,last);
    }
}

uint calc_end_target(){
    //Forward
    if(target_direction){
        if (target_speed_step == 128) return 0;
        else return (target_speed_step - 129) * CV_ARRAY_FLASH[59];
    }
    //Reverse
    else {
        if (target_speed_step == 0) return 0;
        else return (target_speed_step-1) * CV_ARRAY_FLASH[59];
    }
}


bool speed_helper() {
    uint8_t accel_rate = CV_ARRAY_FLASH[2];
    uint8_t decel_rate = CV_ARRAY_FLASH[3];
    uint end_target = calc_end_target();
    static uint8_t speed_helper_counter;
    //Emergency Stop
    if (target_speed_step == 129 || target_speed_step==1) {
        current_target = 0;
        speed_helper_counter = 0;
    }
    //Acceleration
    else if (end_target > current_target && speed_helper_counter == accel_rate) {
        current_target += CV_ARRAY_FLASH[59];
        speed_helper_counter = 0;
    }
    //Deceleration
    else if (end_target < current_target && speed_helper_counter == decel_rate) {
        current_target -= CV_ARRAY_FLASH[59];
        speed_helper_counter = 0;
    }
    //Accel/Decel "scheduled"
    else if (end_target != current_target){
        speed_helper_counter++;
    }
    //Target reached -> Reset Counter
    else{
        speed_helper_counter = 0;
    }
    return true;
}

int measure(){
    pwm_set_gpio_level(MOTOR_PWM_FORWARD,0);
    pwm_set_gpio_level(MOTOR_PWM_REVERSE,0);
    adc_select_input(!target_direction);
    busy_wait_us(msr.delay_in_us);
    gpio_put(9,true);
    int sum = 0;
    uint16_t adc_values[msr.total_iterations];
    for (int i = 0; i < msr.total_iterations; ++i) {
        adc_values[i] = adc_read();
    }
    gpio_put(9,false);
    quicksort(adc_values, 0, msr.total_iterations - 1);
    for (uint8_t i = msr.left_side_array_cutoff; i < msr.total_iterations - msr.right_side_array_cutoff ; ++i) {
        sum = sum+adc_values[i];
    }
    return sum/(msr.total_iterations - 20);
}

bool pid_control(struct repeating_timer *t){
    pid.e = (int)current_target - measure();
    pid.e_sum = pid.e_sum + pid.e;
    if (pid.e_sum > pid.sum_limit_max) pid.e_sum = pid.sum_limit_max;
    else if (pid.e_sum < pid.sum_limit_min) pid.e_sum = pid.sum_limit_min;
    pid.output +=   (pid.k_p * (float)pid.e) +
            (pid.a_1 * (float)pid.e_sum) +                         //  k_i*t_s*e_sum
            (pid.a_2 * (float)(pid.e - pid.e_prev));                //  (k_d/t_s)*(e-e_prev)
            pid.e_prev = pid.e;
            if (current_target == 0) pid.output = 0;
            else if (pid.output > 5000) pid.output = 5000;
            else if (pid.output < 1000) pid.output = 1000;
            adjust_pwm_level((uint16_t )pid.output);
            return true;
}

void init_pid(){
    //    pid.k_p = ((float)CV_ARRAY_FLASH[48] / 256);
    //    pid.t_s = ((float)CV_ARRAY_FLASH[47] / 1000);
    //    pid.a_1 = ((float)CV_ARRAY_FLASH[49] / 64)   * pid.t_s ;         //  k_i*t_s
    //    pid.a_2 = ((float)CV_ARRAY_FLASH[50] / 4096) / pid.t_s;          //  k_d/t_s
    pid.k_p = 0.45f;
    pid.t_s = 10.0f / 1000;
    pid.a_1 = 1.6f  * pid.t_s ;         //  k_i*t_s
    pid.a_2 = 0.03f / pid.t_s;          //  k_d/t_s
    pid.sum_limit_max = CV_ARRAY_FLASH[51] * 10;
    pid.sum_limit_min = CV_ARRAY_FLASH[52] * (-10);
    msr.total_iterations = 70;/*CV_ARRAY_FLASH[60];*/
    msr.delay_in_us = CV_ARRAY_FLASH[61];
    msr.left_side_array_cutoff = 15;/*CV_ARRAY_FLASH[62];*/
    msr.right_side_array_cutoff = 15;/*CV_ARRAY_FLASH[63];*/
}

void init_pwm(uint gpio) {
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint32_t wrap_counter = 5000;                  // 5000 Cycles @ 125MHz for 1 Period -> 25kHz
    pwm_set_wrap(slice_num, wrap_counter);
    pwm_set_gpio_level(gpio,0);
    pwm_set_clkdiv(slice_num,1);
    pwm_set_enabled(slice_num, true);
}

void print_cv_array(const uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        printf("CV_%u: %02x",i+1, buf[i]);
        if (i % 4 == 3)
            printf("\n");
        else
            printf(" ");
    }
}
void core1_entry() {
    if(gpio_get_function(MOTOR_PWM_FORWARD)!=4){
        gpio_set_function(MOTOR_PWM_FORWARD, GPIO_FUNC_PWM);
        gpio_set_function(MOTOR_PWM_REVERSE, GPIO_FUNC_PWM);
        adc_init();
        adc_gpio_init(V_EMF_ADC_PIN_FORWARD);
        adc_gpio_init(V_EMF_ADC_PIN_REVERSE);
        init_pwm(MOTOR_PWM_FORWARD);
        init_pwm(MOTOR_PWM_REVERSE);
        init_pid();
    }
    alarm_pool_add_repeating_timer_ms(alarm_pool_create(0,1),
                                      10/*CV_ARRAY_FLASH[47]*/,
                                      pid_control,
                                      NULL,
                                      &pid_control_timer);
    irq_set_priority(0,0x00); // 0x00 is equivalent to the highest priority!
    alarm_pool_add_repeating_timer_ms(alarm_pool_create(1,1),
                                      7,
                                      speed_helper,
                                      NULL,
                                      &speed_helper_timer);
    while (1);
}