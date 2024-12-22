//////////////////////////
//   RP2040-Decoder     //
// Gabriel Koppenstein  //
//      shared.c        //
//////////////////////////

#include "shared.h"

uint8_t speed_step_target = 0;
uint8_t speed_step_target_prev = 0;

uint32_t error_state = 0;


// Functions in shared.c can be accessed by both cores
// Measures Back-EMF voltage (proportional to the rotational speed of the motor) on GPIO 28 and GPIO 29 respectively (depending on direction)
float measure(uint8_t total_iterations,
              uint8_t measurement_delay_us,
              uint8_t l_side_arr_cutoff,
              uint8_t r_side_arr_cutoff,
              bool direction){
    uint32_t sum = 0;
    uint16_t adc_val[total_iterations];
    int32_t j = 0;
    uint16_t key;
    pwm_set_gpio_level(MOTOR_FWD_PIN, 0);
    pwm_set_gpio_level(MOTOR_REV_PIN, 0);
    adc_select_input((!direction)+2);
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
    //discard x entries beginning from the lowest entry of the array (counting up); x = msr.l_side_arr_cutoff
    //discard y entries beginning from the highest index of the array (counting down); y = msr.r_side_arr_cutoff
    for (int32_t i = l_side_arr_cutoff; i < total_iterations - r_side_arr_cutoff ; ++i) {
        sum += adc_val[i];
    }
    float res = (float)sum / (float)( total_iterations - (l_side_arr_cutoff + r_side_arr_cutoff) );
    // return true to keep the repeating timer active
    return res;
}

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

bool get_direction_of_speed_step(uint8_t speed_step){
    // Shift by 7 Bytes to move bit7 into bit0 position and return
    return speed_step >> 7;
}

void set_error(error_t err){
    error_state |= err;
    gpio_put(RP2040_DECODER_DEFAULT_LED_PIN, true);
}

void clear_error(error_t err){
    error_state & (~err);
}

uint32_t get_error_state(){
    return error_state;
}