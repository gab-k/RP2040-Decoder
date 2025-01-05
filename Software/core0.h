/*!
 *
 * \file core0.h
 * Used exclusively by core0
 *
 */
#pragma once

#include "shared.h"
#include "CV.h"

/**
 * @def MESSAGE_3_BYTES
 * @brief Comparison bitmask for detecting 3 byte long DCC messages/packets
 */
#define MESSAGE_3_BYTES 0b11111111110000000000000000000000000001
/**
 * @def MESSAGE_MASK_3_BYTES
 * @brief Bitmask for detecting 3 byte long DCC messages/packets, input_bit_buffer is bitwise ANDed with the mask
 */
#define MESSAGE_MASK_3_BYTES 0b11111111111000000001000000001000000001
/**
 * @def MESSAGE_4_BYTES
 * @brief Comparison bitmask for detecting 4 byte long DCC messages/packets
 */
#define MESSAGE_4_BYTES 0b11111111110000000000000000000000000000000000001
/**
 * @def MESSAGE_MASK_4_BYTES
 * @brief Bitmask for detecting 4 byte long DCC messages/packets, input_bit_buffer is bitwise ANDed with the mask
 */
#define MESSAGE_MASK_4_BYTES 0b11111111111000000001000000001000000001000000001
/**
 * @def MESSAGE_5_BYTES
 * @brief Comparison bitmask for detecting 5 byte long DCC messages/packets
 */
#define MESSAGE_5_BYTES 0b11111111110000000000000000000000000000000000000000000001
/**
 * @def MESSAGE_MASK_5_BYTES
 * @brief Bitmask for detecting 5 byte long DCC messages/packets, input_bit_buffer is bitwise ANDed with the mask
 */
#define MESSAGE_MASK_5_BYTES 0b11111111111000000001000000001000000001000000001000000001

/**
 * @def INVALID_PACKAGE
 * @brief Return value of detect_dcc_packet() when the packet is invalid, meaning no packet was detected
 */
#define INVALID_PACKAGE SIZE_MAX

/**
 * @def RING_BUFFER_PACKETS
 * @brief First dimension of dcc ring buffer array - meaning RING_BUFFER_PACKETS can be retained inside the buffer
 */
#define RING_BUFFER_PACKETS 8
/**
 * @def RING_BUFFER_BYTES
 * @brief Secoond dimension of dcc ring buffer array - meaning each packet can have a maximum size of RING_BUFFER_BYTES
 */
#define RING_BUFFER_BYTES 5

/**
 * @def FLASH_CMD_READ_JEDEC_ID
 * @brief Constant value of 0x9F used as a JEDEC ID read command for reading the JEDEC ID of a winbond flash memory chip
 */
#define FLASH_CMD_READ_JEDEC_ID 0x9F
/**
 * @def FLASH_TIMEOUT_IN_MS
 * @brief Timeout value for flash access
 */
#define FLASH_TIMEOUT_IN_MS 500

/**
 * @def ADC_CALIBRATION_ITERATIONS
 * @brief Iterations for measuring adc offset
 */
#define ADC_CALIBRATION_ITERATIONS 8192
/**
 * @def WATCHDOG_TIMER_IN_MS
 * @brief Watchdog timer setting, watchdog update needs to be called every WATCHDOG_TIMER_IN_MS milliseconds, otherwise decoder resets itself.
 */
#define WATCHDOG_TIMER_IN_MS 5000

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


/**
 * @brief Structure representing a DCC packet.
 * 
 * This structure is used to store the data and length of a DCC packet.
 * 
 * @typedef dcc_packet_t
 * @struct dcc_packet_t
 * 
 * @param data An array of bytes representing the DCC packet data.
 * @param length The length of the DCC packet data.
 */
typedef struct dcc_packet_t {
        /*! An array of bytes representing the DCC packet data. */
        uint8_t data[RING_BUFFER_BYTES];
        /*! The length of the DCC packet data. */
        size_t length;
}dcc_packet_t;

/**
 * @brief Structure representing a ring buffer for DCC packets.
 * 
 * This structure is used to manage a ring buffer that stores DCC packets.
 * It contains an array of packets and indices for writing and reading.
 * 
 * @typedef dcc_ring_buffer_t
 * @struct dcc_ring_buffer_t
 */
typedef struct dcc_ring_buffer_t {
        /*! Array of DCC packets stored in the ring buffer. */
        dcc_packet_t packets[RING_BUFFER_PACKETS];
        /*! Index for the next write operation in the ring buffer. */
        size_t wr_idx;
        /*! Index for the next read operation in the ring buffer. */
        size_t rd_idx;
} dcc_ring_buffer_t;



/**
 * @brief Increment the index of a ring buffer.
 *
 * This function increments the provided index of a ring buffer, wrapping around
 * to zero if the end of the buffer is reached.
 *
 * @param idx Pointer to the index to be incremented.
 * @param ring_buffer_length The length of the ring buffer.
 */
static void increment_ring_buffer_idx(size_t * idx, uint8_t ring_buffer_length);

/*!
 * \brief Function for erasing a sector of flash memory.
 *
 * This function erases a specified range of flash memory sectors.
 *
 * \param param Pointer to parameters.
 *              - First parameter: Offset into flash, in bytes. Must be aligned to a 4096-byte flash sector.
 *              - Second parameter: Number of bytes to be erased. Must be a multiple of 4096 bytes (one sector).
 */
static void call_flash_range_erase(void *param);

/*!
 * \brief Function for programming a range of flash memory
 *
 * This function programs a specified range of flash memory with the provided data.
 *
 * \param param Pointer to parameters
 *              - First parameter: The Flash address of the first byte to be programmed. Must be aligned to a 256-byte flash page.
 *              - Second parameter: The number of bytes to program. Must be a multiple of 256 bytes (one page).
 *              - Third parameter: Pointer to the data to program into flash.
 */
static void call_flash_range_program(void *param);

/*!
 * \brief Function for executing a flash command via QSPI
 *
 * \param param Pointer to parameters
 *              First parameter is the pointer to a byte buffer which will be transmitted to the flash
 *              Second parameter is the pointer to a byte buffer where data received from the flash will be written.
 *              Third parameter is the length in bytes of any of the two buffers (same size).
 */
static void call_flash_do_cmd(void *param);

/*!
 * \brief Function for calculating the absolute value of an integer.
 *
 * This function takes an integer as input and returns its absolute value.
 *
 * \param x The input integer whose absolute value is to be calculated.
 * \return The absolute value of the input integer.
 */
static unsigned int absolute_val(int x);

/*!
 * \brief Computes the average of values in an array that deviate less than twice the standard deviation from the original average.
 *
 * This function calculates the average of the elements in the input array that are within two standard deviations of the mean of the array.
 *
 * \param arr The input array of values.
 * \param length The number of elements in the input array.
 * \return The average value of the elements that deviate less than twice the standard deviation from the original average.
 */
static uint16_t two_std_dev(const uint16_t arr[], uint32_t length);

/*!
 * \brief Measures ADC offset and saves the offset value to flash memory.
 * 
 * This function performs a series of ADC measurements to determine the offset
 * and then stores this offset value in the flash memory for future use.
 *
 * \param n The number of measurements to be performed to calculate the offset.
 */
static void adc_offset_adjustment(uint32_t n);

/*!
 * \brief Function for acknowledging a CV write or CV verify instruction
 *
 * Acknowledgement works by creating a spike in current consumption, normally realized by running the motor in alternating directions.
 * This current increase is detected by the command station.
 * 
 * \note Refers to NMRA S-9.2.3 Section D and RCN-216 Section 3
 */
static void acknowledge();

/*!
 * \brief Function for verifying a single bit of a CV
 * \param cv_address CV Address
 * \param bit_val Value of bit to be verified
 * \param bit_pos Position of bit to be verified
 */
static void verify_cv_bit(uint16_t cv_address, bool bit_val, uint8_t bit_pos);

/*!
 * \brief Function for verifying a CV byte
 * 
 * This function verifies the byte of a Configuration Variable (CV) at the specified address.
 * 
 * \param cv_address The address of the CV to be verified.
 * \param cv_data The byte value to be verified against the CV.
 */
static void verify_cv_byte(uint16_t cv_address, uint8_t cv_data);

/*!
 * \brief Writes a byte to a Configuration Variable (CV).
 *
 * This function writes a byte of data to a specified CV address.
 *
 * \param cv_address The address of the CV to write to.
 * \param cv_data The data byte to write to the CV.
 */
static void write_cv_byte(uint16_t cv_address, uint8_t cv_data);

/*!
 * \brief This function sets all the CV values stored in flash back to their default settings (CV_ARRAY_DEFAULT).
 */
static void reset_cv_array_to_default();

/*!
 * \brief CV Programming mode function
 * 
 * This function is used to enter programming mode in order to write CV values to persistent flash memory or verify/read bits/bytes from flash memory.
 * 
 * Procedure:
 * 1. Checks for a valid programming command.
 * 2. Evaluates the type of programming instruction (Write byte / Verify byte / Verify bit) and runs the corresponding function. See verify_cv_byte(), verify_cv_bit() and write_cv_byte().
 *
 * @param number_of_bytes The number of bytes in the array.
 * @param byte_array A pointer to the byte array.
 * 
 * @note Refer to NMRA Standard 9.2.3 ("Service Mode Programming") or RCN-216 for more details.
 */
static void program_mode(size_t number_of_bytes, const uint8_t *const byte_array);

/*!
 * \brief Set outputs according to function mapping in CV257 to CV512
 *
 * This function maps the functions and direction to the mapped outputs configured in CV257 to CV512.
 * Also enables PWM when enabled according to CV112 to CV115.
 *
 * \param functions_to_set_bitmask Function bitmask F0 is bit0, F1 is bit1, ...
 */
static void set_outputs(uint32_t functions_to_set_bitmask);

/**
 * @brief Update functions F0 - F31 when function command is received or in case of direction change.
 *
 * This function updates the active functions based on the provided bitmask. It can also clear a specific bit
 * and handle changes in direction.
 *
 * @param new_function_bitmask New function bitmask representing the state of functions F0 - F31.
 * @param clr_bit_ind Index of the bit to be cleared in the bitmask.
 * @param direction_change Flag indicating if there is a change in direction.
 */
static void update_active_functions(uint32_t new_function_bitmask, uint8_t clr_bit_ind, bool direction_change);

/**
 * @brief Detect errors in the byte array using exor of all bytes.
 * 
 * Exor of all bytes is expected to be "0b00000000".
 *
 * @param number_of_bytes Number of bytes in the array.
 * @param byte_array Pointer to the byte array.
 * @return true if no error is detected, false otherwise.
 */
static bool error_detection(size_t number_of_bytes, const uint8_t *byte_array);

/**
 * @brief Check if the address is a long address.
 *
 * @param number_of_bytes Number of bytes in the array.
 * @param byte_array Pointer to the byte array.
 * @return true if it is a long address, false otherwise.
 */
static bool is_long_address(size_t number_of_bytes, const uint8_t byte_array[]);

/**
 * @brief Evaluate the address of the message.
 *
 * @param number_of_bytes Number of bytes in the array.
 * @param byte_array Pointer to the byte array.
 * @return true if the address is correct, false otherwise.
 */
static bool address_evaluation(size_t number_of_bytes, const uint8_t byte_array[]);

/**
 * @brief Evaluate the instruction of the message.
 *
 * @param number_of_bytes Number of bytes in the array.
 * @param byte_array Pointer to the byte array.
 */
static void instruction_evaluation(size_t  number_of_bytes, const uint8_t byte_array[]);

/**
 * @brief Check for reset message - When reset message is found, stop the motor and disable all functions.
 *
 * @return Number of bytes if valid reset message bit-pattern is found. Otherwise -1 is returned.
 */
static bool reset_message_check(size_t number_of_bytes, const uint8_t *const byte_array);

/**
 * @brief Verify DCC message
 *
 * Procedure:
 * 1. Check for valid preamble using bitmasks
 * 2. Return number of bytes if valid bit-pattern is found. Otherwise -1 is returned.
 *
 * \return Number of bytes if valid bit-pattern is found. Otherwise return -1.
 */
static size_t detect_dcc_packet();

/**
 * @brief Convert sequence of bits to byte array
 *
 * For each byte in the byte array, shift the input bit buffer by 9 bits and save the result in the byte array.
 * Bytes are ordered as follows:
 * Start of transmission -> byte_n(address byte) -> ... -> byte_0(error detection byte) -> end of transmission
 * 
 * \param packet Pointer to dcc packet struct which contains data and length of packet.
 */
static void bits_to_dcc_packet_data(dcc_packet_t * packet);

/**
 * @brief Main message evaluation function
 *
 * Procedure:
 * 1. Check if input buffer contains a valid DCC message
 * 2. Split data into array of bytes
 * 3. Check for errors
 * 4. Check for matching address
 * 5. Check for reset message and set flag when reset message is received
 *
 */
static void evaluate_packet();

/*!
 * \brief Track signal rising edge interrupt callback function
 *
 * Saves timer counter value using get_absolute_time(), then disables rising edge IRQ and enables falling edge IRQ.
 *
 */
static void track_signal_rise();

/*!
 * \brief Track signal falling edge interrupt callback function
 *
 * Saves timer counter value using get_absolute_time(), then calculates time difference between rising and falling edge.
 * When the time difference is greater than 87us, this bit is interpreted as a logical 0. Otherwise, it is interpreted as a logical 1.
 * Refers to NMRA S-9.1 and RCN-210 standards
 * Puts the latest bit value into 64 bit unsigned integer "buffer", then checks if the buffer contains a valid DCC packet.
 * When a valid packet is found, it is written into the ring buffer, and the write index gets incremented.
 *
 */
static void track_signal_fall();

/*!
 * \brief Output initialization function
 *
 * Initializes all allowed GPIO outputs as outputs <br>
 * Sets PWM functionality for GPIOs configured as PWM pins CV_112 - CV_115 (see CV.h for reference) <br>
 * PWM frequency and clock divider are configured according to CV_116 - CV_171 (see CV.h for reference) <br>
 */
static void init_outputs();

/*!
 * \brief Checks various CV values in order to detect factory condition or pending calibrations
 *
 * Procedure:
 * 1. Check for flash memory factory condition when found write default values specified in CV.h to flash memory
 * 2. Check for existing ADC offset setup when not found write 7 to read-only CV_7 in order to trigger a ADC offset measurement/calibration
 * 3. Check for base PWM configuration/calibration, run calibration when none is found
 */
static void cv_setup_check();

/*!
 * \brief Function initializes motor PWM for pin specified in CMakeLists.txt
 * \param gpio GPIO pin
 */
static void init_motor_pwm(uint8_t gpio);

/*!
 * \brief Initializes ADC
 *
 * Function initializes ADC, corresponding GPIO pins specified in CMakeLists.txt, also configures ADC FIFO.
 */
static void init_adc();