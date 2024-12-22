/*!
 *
 * \file core0.h
 * Used exclusively by core0
 *
 */
#pragma once

#include "shared.h"
#include "CV.h"
#include <stdlib.h>

#define SIZE_BYTE_ARRAY 5
#define MESSAGE_3_BYTES 0b11111111110000000000000000000000000001
#define MESSAGE_MASK_3_BYTES 0b11111111111000000001000000001000000001
#define MESSAGE_4_BYTES 0b11111111110000000000000000000000000000000000001
#define MESSAGE_MASK_4_BYTES 0b11111111111000000001000000001000000001000000001
#define MESSAGE_5_BYTES 0b11111111110000000000000000000000000000000000000000000001
#define MESSAGE_MASK_5_BYTES 0b11111111111000000001000000001000000001000000001000000001

#define FLASH_CMD_READ_JEDEC_ID 0x9F
#define ADC_CALIBRATION_ITERATIONS 8192

/*!
 * \brief Bitmasks used for clearing bits corresponding to function blocks
 *
 * [0] = F0-F4
 * [1] = F5-F8
 * [2] = F9-F12
 * [3] = F13-F20
 * [4] = F21-F28
 * [5] = F29-F31
 *
 */
const uint32_t clr_bit_arr[6] = {
        0b11111111111111111111111111100000, // Clear Bits 0-4 (F0-F4)
        0b11111111111111111111111000011111, // Clear Bits 5-8 (F5-F8)
        0b11111111111111111110000111111111, // Clear Bits 9-12 (F9-F12)
        0b11111111111000000001111111111111, // Clear Bits 13-20 (F13-F20)
        0b11100000000111111111111111111111, // Clear Bits 21-28 (F21-F28)
        0b00011111111111111111111111111111, // Clear Bits 29-31 (F29-F31)
};

/*!
 * \brief Function returns average of values deviating less than twice the standard deviation from original average.
 *
 * \param arr input array
 * \param length amount of elements in input array
 * \return average value of values deviating less than 2*standard_dev.
 */
uint16_t two_std_dev(const uint16_t arr[], uint32_t length);

/*!
 * \brief Measures ADC offset and saves the offset value to flash memory.
 * \param n Amount of measurements to be done
 */
void adc_offset_adjustment(uint32_t n);

/*!
 * \brief Function for acknowledging a CV write or CV verify instruction
 *
 * Acknowledgement works by creating a spike in current consumption, normally realized by running the motor in alternating directions.
 * This current increase is detected by the command station.
 * Refers to NMRA S-9.2.3 Section D and RCN-216 Section 3
 */
void acknowledge();

/*!
 * \brief Function for verifying a single bit of a CV
 * \param cv_address CV Address
 * \param bit_val Value of bit to be verified
 * \param bit_pos Position of bit to be verified
 */
void verify_cv_bit(uint16_t cv_address, bool bit_val, uint8_t bit_pos);

/*!
 * \brief Function for verifying a CV byte
 * \param cv_address CV Address
 * \param cv_data Byte to be verified
 */
void verify_cv_byte(uint16_t cv_address, uint8_t cv_data);

/*!
 * \brief Function for writing a CV byte
 * \param cv_address
 * \param cv_data
 */
void write_cv_byte(uint16_t cv_address, uint8_t cv_data);

/*!
 * \brief This function sets all the CV values stored in flash back to their default settings (CV_ARRAY_DEFAULT).
 */
void reset_cv_array_to_default();

/*!
 * \brief CV Programming mode function
 * Used on entering programming mode in order to write CV values to persistent flash memory or verify/read bits/bytes from flash. <br>
 * Refers to NMRA Standard 9.2.3 ("Service Mode Programming") or RCN-216<br>
 * TODO: describe new procedure
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

/*!
 * \brief Set outputs according to function mapping in CV257 to CV512
 *
 * This function maps the functions and direction to the mapped outputs configured in CV257 to CV512.
 * Also enables PWM when enabled according to CV112 to CV115.
 *
 * \param functions_to_set_bitmask Function bitmask F0 is bit0, F1 is bit1, ...
 */
void set_outputs(uint32_t functions_to_set_bitmask);

void update_active_functions(uint32_t new_function_bitmask, uint8_t clr_bit_ind, bool direction_change);

bool error_detection(int8_t number_of_bytes, const uint8_t *byte_array);

bool is_long_address(uint8_t number_of_bytes, const uint8_t byte_array[]);

bool address_evaluation(uint8_t number_of_bytes, const uint8_t byte_array[]);

void instruction_evaluation(uint8_t number_of_bytes, const uint8_t byte_array[]);

bool reset_message_check(uint8_t number_of_bytes, const uint8_t *const byte_array);

int8_t verify_dcc_message();

void bits_to_byte_array(int8_t number_of_bytes, uint8_t byte_array[]);

void evaluate_message();

/*!
 * \brief Track signal rising edge interrupt callback function
 *
 * Saves timer counter value using get_absolute_time(), then disables rising edge IRQ and enables falling edge IRQ.
 * Parameters are unused.
 *
 * \param gpio Unused parameter
 * \param events Unused parameter
 */
void track_signal_rise(unsigned int gpio, long unsigned int events);

/*!
 * \brief Track signal falling edge interrupt callback function
 *
 * Saves timer counter value using get_absolute_time(), then calculates time difference between rising and falling edge.
 * When the time difference is greater than 87us, this bit is interpreted as a logical 0.
 * Refers to NMRA S-9.1 and RCN-210 standards
 * Puts the latest bit value into buffer, then evaluates complete buffer using evaluate_message().
 * Parameters are unused.
 *
 * \param gpio Unused parameter
 * \param events Unused parameter
 */
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