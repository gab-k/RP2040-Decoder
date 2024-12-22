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
#ifndef PICO_FLASH_SIZE_BYTES
#define PICO_FLASH_SIZE_BYTES (8 * 1024 * 1024)
#endif
// Offset from base address used for saving CV_ARRAY_FLASH - in this case one sector size from the end of flash
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES-FLASH_SECTOR_SIZE)

#ifndef PICO_FLASH_SPI_CLKDIV
#define PICO_FLASH_SPI_CLKDIV 2
#endif


#ifndef PICO_RP2040_B0_SUPPORTED
#define PICO_RP2040_B0_SUPPORTED 1
#endif



#endif
