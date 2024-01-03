/*!
 *
 * \file core0.h
 * Used exclusively by core0
 *
 */
#pragma once
#include "shared.h"
#include "CV.h"
#define SIZE_BYTE_ARRAY 5
#define MESSAGE_3_BYTES 0b11111111110000000000000000000000000001
#define MESSAGE_MASK_3_BYTES 0b11111111111000000001000000001000000001
#define MESSAGE_4_BYTES 0b11111111110000000000000000000000000000000000001
#define MESSAGE_MASK_4_BYTES 0b11111111111000000001000000001000000001000000001
#define MESSAGE_5_BYTES 0b11111111110000000000000000000000000000000000000000000001
#define MESSAGE_MASK_5_BYTES 0b11111111111000000001000000001000000001000000001000000001
const uint32_t clr_bit_arr[6] = {
        0b11111111111111111111111111100000, // Clear Bits 0-4 (F0-F4)
        0b11111111111111111111111000011111, // Clear Bits 5-8 (F5-F8)
        0b11111111111111111110000111111111, // Clear Bits 9-12 (F9-F12)
        0b11111111111000000001111111111111, // Clear Bits 13-20 (F13-F20)
        0b11100000000111111111111111111111, // Clear Bits 21-28 (F21-F28)
        0b00011111111111111111111111111111, // Clear Bits 29-31 (F29-F31)
};

float two_std_dev(const float arr[], uint32_t length);
void adc_offset_adjustment(uint32_t length);
void acknowledge();
void verify_cv_bit(uint16_t cv_address,bool bit_val, uint8_t bit_pos);
void verify_cv_byte(uint16_t cv_address, uint8_t cv_data);
void regular_cv_write(uint16_t cv_index, uint8_t cv_data);
//void write_cv_handler(uint16_t cv_index, uint8_t cv_data);


#define ProgramInstrVerifyBit  0b10
#define ProgramInstrVerifyByte 0b01
#define ProgramInstrWriteByte  0b11
#define ProgramCmd(instr, adrmsbits) ((instr<<2) | (adrmsbits))

/*!
 * \brief CV Programming mode function
 * Used on entering programming mode in order to write CV values to persistent flash memory or verify/read bits/bytes from flash. <br>
 * Refers to NMRA Standard 9.2.3 ("Service Mode Programming") or RCN-216<br>
 *
 * Procedure:
 *      1. Checks for valid programming command
 *      2. Disables core1 timers
 *      3. Resets core1
 *      4. Saves interrupts
 *      5. Evaluates the type of programming instruction Write byte / Verify byte / Verify bit and runs corresponding function
 *      6. Restores interrupts
 *      7. Launch core1
 *
 * \param number_of_bytes Number of bytes in byte_array[]
 * \param byte_array Pointer to byte_array[]
 */
void program_mode(uint8_t number_of_bytes, const uint8_t byte_array[]);
void set_outputs(uint32_t functions_to_set_bitmask);
void update_active_functions(uint32_t new_function_bitmask, uint8_t clr_bit_ind, bool direction_change);
bool error_detection(int8_t number_of_bytes, const uint8_t * byte_array);
bool is_long_address(uint8_t number_of_bytes, const uint8_t byte_array[]);
bool address_evaluation(uint8_t number_of_bytes,const uint8_t byte_array[]);
void instruction_evaluation(uint8_t number_of_bytes,const uint8_t byte_array[]);
bool reset_message_check(uint8_t number_of_bytes, const uint8_t *const byte_array);
int8_t verify_dcc_message();
void bits_to_byte_array(int8_t number_of_bytes,uint8_t byte_array[]);
void evaluate_message();
void track_signal_rise(unsigned int gpio, long unsigned int events);
void track_signal_fall(unsigned int gpio, long unsigned int events);

/*!
 * \brief Output initialization function
 *
 * Initializes all allowed GPIO outputs as outputs <br>
 * Sets PWM functionality for GPIOs configured as PWM pins CV_112 - CV_115 (see CV.h for reference) <br>
 * PWM frequency and clock divider are configured according to CV_116 - CV_171 (see CV.h for reference) <br>
 */
void init_outputs();

/*!
 * \brief Measures base pwm duty cycles at which the motor barely starts moving.
 *
 * Function starts with 1/20 of the maximum duty cycle then increases the duty cycle by 1/500 of the maximum until
 * the motor movement can be measured. This is done for the specified amount of iterations.
 * The array with raw measurements values is then passed to two_std_dev() in order to detect outliers and
 * calculate the average.An additional correction factor is applied and the overall measurement result is then
 * returned. In case no motor movement could be measured a value of 0 is returned and normally written to flash,
 * indicating a pending calibration next time the decoder starts, rerunning the calibration.
 * \param direction Specifies direction which is to be measured
 * \param iterations Specifies the amount of iterations the measurement is to be repeated and averaged out over
 * \return Measurement
 */
uint16_t measure_base_pwm(bool direction, uint8_t iterations);

/*!
 * \brief Checks various CV values in order to detect factory condition or pending calibrations
 *
 * Procedure:
 * 1. Check for flash memory factory condition when found write default values specified in CV.h to flash memory
 * 2. Check for existing ADC offset setup when not found write 7 to read-only CV_7 in order to trigger a ADC offset measurement/calibration
 * 3. Check for base PWM configuration/calibration, run calibration when none is found
 */
void cv_setup_check();

/*!
 * \brief Function initializes motor PWM for pin specified
 * \param gpio GPIO pin
 */
void init_motor_pwm(uint8_t gpio);

/*!
 * \brief Initializes ADC
 *
 * Function initializes ADC, corresponding GPIO pins and ADC FIFO
 */
void init_adc();