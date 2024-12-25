/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// -----------------------------------------------------
// NOTE: THIS HEADER IS ALSO INCLUDED BY ASSEMBLER SO
//       SHOULD ONLY CONSIST OF PREPROCESSOR DIRECTIVES
// -----------------------------------------------------

// Modified version of "boards/pico.h" for using the RP2040-Decoder board

// pico_cmake_set PICO_PLATFORM=rp2040

#ifndef _BOARDS_RP2040_DECODER_H
#define _BOARDS_RP2040_DECODER_H

// For board detection
#define RP2040_DECODER

// In case the xosc takes longer to stabilize than usual
#ifndef PICO_XOSC_STARTUP_DELAY_MULTIPLIER
#define PICO_XOSC_STARTUP_DELAY_MULTIPLIER 64
#endif

// --- UART ---
#ifndef PICO_DEFAULT_UART
#define PICO_DEFAULT_UART 0
#endif
#ifndef PICO_DEFAULT_UART_TX_PIN
#define PICO_DEFAULT_UART_TX_PIN 0
#endif
#ifndef PICO_DEFAULT_UART_RX_PIN
#define PICO_DEFAULT_UART_RX_PIN 1
#endif

// --- LED ---
#ifndef RP2040_DECODER_DEFAULT_LED_PIN
#define RP2040_DECODER_DEFAULT_LED_PIN 12
#endif

// --- FLASH ---

// Flash size is 8 MiB for W25Q64JVSSIQ if you decide to use another capacity flash IC change the size to the corresponding value
// 8MiB = (8 * 1024 * 1024) bytes = 8388608 bytes
#ifndef PICO_FLASH_SIZE_BYTES
#define PICO_FLASH_SIZE_BYTES 8388608
#endif

// Offset from base address used for saving CV_ARRAY_FLASH - in this case one sector size from the end of flash
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES-FLASH_SECTOR_SIZE)

#ifndef PICO_FLASH_SPI_CLKDIV
#define PICO_FLASH_SPI_CLKDIV 2
#endif


#ifndef PICO_RP2040_B0_SUPPORTED
#define PICO_RP2040_B0_SUPPORTED 1
#endif

// Other pins
// GPIO used for DCC-Signal, Motor PWM or ADC
#define DCC_INPUT_PIN 21u
#define MOTOR_FWD_PIN 23u
#define MOTOR_REV_PIN 22u
#define FWD_V_EMF_ADC_PIN 29u
#define REV_V_EMF_ADC_PIN 28u


#if LOGLEVEL != 0
   // GPIO used directly (GPIO 2-5 incl.) as outputs or to switch auxiliary output transistors (GPIO 24-27 incl.)
   #define GPIO_OUTPUT_PIN_MASK ((1u<<24) | (1u<<25) | (1u<<26) | (1u<<27) | (1u<<5) | (1u<<4) | (1u<<3) | (1u<<2))
#else
   // GPIO used directly (GPIO 0-5 incl.) as outputs or to switch auxiliary output transistors (GPIO 24-27 incl.)
   #define GPIO_OUTPUT_PIN_MASK ((1u<<24) | (1u<<25) | (1u<<26) | (1u<<27) | (1u<<5) | (1u<<4) | (1u<<3) | (1u<<2) | (1u<<1) | (1u<<0))
#endif

// GPIO pin mask to prevent setting illegal GPIOs (ADC, Motor, DCC Input Pin)
// When logging to stdio using UART is enabled do NOT use PICO_DEFAULT_UART_TX_PIN and PICO_DEFAULT_UART_RX_PIN as GPIO/PWM outputs
#if (LOGLEVEL != 0) && (STDIO_UART_ENABLED != 0)
    #define GPIO_ILLEGAL_MASK ((1u<<DCC_INPUT_PIN) | (1u<<MOTOR_FWD_PIN) | (1u<<MOTOR_REV_PIN) | (1u<<FWD_V_EMF_ADC_PIN) | (1u<<REV_V_EMF_ADC_PIN) | (1u<<PICO_DEFAULT_UART_TX_PIN) | (1u<<PICO_DEFAULT_UART_RX_PIN))
#else
    #define GPIO_ILLEGAL_MASK ((1u<<DCC_INPUT_PIN) | (1u<<MOTOR_FWD_PIN) | (1u<<MOTOR_REV_PIN) | (1u<<FWD_V_EMF_ADC_PIN) | (1u<<REV_V_EMF_ADC_PIN))
#endif

// GPIO pin mask with allowed outputs (AUX & GPIO (configured as outputs))
#define GPIO_ALLOWED_OUTPUTS (GPIO_OUTPUT_PIN_MASK) & ~(GPIO_ILLEGAL_MASK)


#endif
