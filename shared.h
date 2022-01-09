//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      shared.h        //
//////////////////////////


#ifndef RP2040_DECODER_SHARED_VARS_H
#define RP2040_DECODER_SHARED_VARS_H
#endif //RP2040_DECODER_SHARED_VARS_H
#define MOTOR_PWM_FORWARD 10 //Custom: 10 Default: 12
#define MOTOR_PWM_REVERSE 13
extern repeating_timer_t pid_control_timer;
extern repeating_timer_t speed_helper_timer;
extern bool target_direction;
extern uint target_speed_step;
extern const uint8_t *CV_ARRAY_FLASH;
void adjust_pwm_level(uint16_t level);
void core1_entry();