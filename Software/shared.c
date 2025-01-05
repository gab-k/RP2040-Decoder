//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      shared.c        //
//////////////////////////

#include "shared.h"

// TODO: Implement getter and setter functions for speed steps, checking validity of speed step values.
speed_step_t speed_step_target = SPEED_STEP_REVERSE_STOP;
speed_step_t speed_step_target_prev = SPEED_STEP_REVERSE_STOP;
bool cv_setup_check_done = false;
bool flash_safe_execute_core_init_done = false;
error_t error_state = 0;

// Functions in shared.c are accessed by both cores

// retrieve 4Bytes from CV_ARRAY_FLASH at once and return 32-Bit value
uint32_t get_32bit_CV (uint16_t CV_start_index){
    uint8_t byte_3 = CV_ARRAY_FLASH[CV_start_index];
    uint8_t byte_2 = CV_ARRAY_FLASH[CV_start_index+1];
    uint8_t byte_1 = CV_ARRAY_FLASH[CV_start_index+2];
    uint8_t byte_0 = CV_ARRAY_FLASH[CV_start_index+3];
    return (byte_0) + (byte_1<<8) + (byte_2<<16) + (byte_3<<24);
}


// retrieve 2Bytes from CV_ARRAY_FLASH at once and return 16-Bit value
uint16_t get_16bit_CV (uint16_t CV_start_index){
    uint8_t byte_1 = CV_ARRAY_FLASH[CV_start_index];
    uint8_t byte_0 = CV_ARRAY_FLASH[CV_start_index+1];
    return (byte_0) + (byte_1<<8);
}

direction_t get_direction_of_speed_step(speed_step_t speed_step){
    // Bit 7 is the direction bit.
    // Shift by 7 bits to move bit7 into bit0 position and return direction.
    return speed_step >> 7;
}

void set_error(error_t err){
    error_state |= err;
    #ifdef RP2040_DECODER_DEFAULT_LED_PIN
        gpio_put(RP2040_DECODER_DEFAULT_LED_PIN, true);
    #endif
}

void clear_error(error_t err){
    error_state &= (~err);
    if (error_state == 0){
        #ifdef RP2040_DECODER_DEFAULT_LED_PIN
            gpio_put(RP2040_DECODER_DEFAULT_LED_PIN, false);
        #endif
    }
}

void clear_all_errors(){
    error_state = 0;
    #ifdef RP2040_DECODER_DEFAULT_LED_PIN
        gpio_put(RP2040_DECODER_DEFAULT_LED_PIN, false);
    #endif
}

error_t get_error_state(){
    return error_state;
}