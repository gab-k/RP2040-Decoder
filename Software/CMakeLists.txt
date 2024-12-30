# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD_HEADER_DIRS ${CMAKE_SOURCE_DIR}) 
# Board type selection:
#
# Please select RP2040-Decoder-board-Rev-1_0 for Rev >= 1.0 or RP2040-Decoder-board-Rev-0_3 for Rev <= 0.3
set(PICO_BOARD RP2040-Decoder-board-Rev-1_0)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(RP2040-Decoder C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Manual compiler options, these generally override configuration set by CMAKE_BUILD_TYPE
add_compile_options(-Wall
                    # -O0                   # Overrides optimization flags specified by CMAKE_BUILD_TYPE
                    # -g3                   # Overrides debug flags specified by CMAKE_BUILD_TYPE
                    -Wno-format             # unsigned int = uint32_t using gcc so don't warn about format
                    -Wno-unused-function    # Some defined functions used for debugging are never called or not used when logging is disabled
                    )

# Add executable
add_executable( RP2040-Decoder 
                core0.c
                core0.h
                core1.c
                core1.h
                shared.c
                shared.h )

pico_set_program_name(RP2040-Decoder "RP2040-Decoder")

# Build type
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("PICO_DEOPTIMIZED_DEBUG: ${PICO_DEOPTIMIZED_DEBUG}")
if(NOT CMAKE_BUILD_TYPE)
    message(WARNING "CMAKE_BUILD_TYPE is not set. Defaulting to 'Release'")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# Logging configuration:
# 
# LOGLEVEL>0 enables logging messages via stdio, this can be done either via USB or alternatively,
# directly with the UART TX/RX pins (see pin definition in RP2040-Decoder-board.h).
#
# In order to use stdio via USB please set STDIO_USB_ENABLED to 1 below
# In order to use stdio via UART TX/RX pins please set STDIO_UART_ENABLED to 1 below
#
# Additional Notes:
# - When logging is disabled (LOGLEVEL 0), all logging functions will be completely excluded during compilation.
# - Be cautious: Logging can alter timing behavior. For example, the PID controller is time critical,
#   limit the frequency of log outputs—such as logging only every 100th iteration.
# - Note that with UART logging enabled, PICO_DEFAULT_UART_TX_PIN and PICO_DEFAULT_UART_RX_PIN are disabled for decoder functions.
#   These pins are used for UART communication. Disabling is done automatically in RP2040-Decoder-board.h or RP2040-Decoder-board-legacy.h respectively.
set(LOGLEVEL 1)
# Additionally, the LOG_WAIT parameter can be configured to make the controller wait for user input via stdio before continuing.
# This is useful for ensuring that all stdio logging outputs are captured.
# If disabled, there may not be enough time to connect the serial terminal before most of the logging has already been completed.
set(LOG_WAIT 0)
# Modify the below lines to enable/disable output over UART and/or USB
set(STDIO_USB_ENABLED 1)
set(STDIO_UART_ENABLED 0)
# Leave those two unmodified
pico_enable_stdio_uart(RP2040-Decoder ${STDIO_UART_ENABLED})
pico_enable_stdio_usb(RP2040-Decoder ${STDIO_USB_ENABLED})
# Add a preprocessor definition which defines whether UART is used or not. 
# This is done to make the C-Code know whether pins used for UART need to be reserved and restricted from setting them as PWM/GPIO.
target_compile_definitions( RP2040-Decoder PRIVATE 
                            LOGLEVEL=${LOGLEVEL}
                            LOG_WAIT=${LOG_WAIT}
                            STDIO_UART_ENABLED=${STDIO_UART_ENABLED}
                            STDIO_USB_ENABLED=${STDIO_USB_ENABLED}
                            )
# Warn when logging is enabled (LOGLEVEL>0) and neither stdio is enabled
if (LOGLEVEL GREATER 0 AND (STDIO_USB_ENABLED EQUAL 0 AND STDIO_UART_ENABLED EQUAL 0))
    message(WARNING "\nWARNING: Logging (LOGLEVEL>0) enabled with both STDIO_USB_ENABLED and STDIO_UART_ENABLED disabled!")
endif()
if (LOGLEVEL EQUAL 0 AND LOG_WAIT EQUAL 1)
    message(WARNING "\nWARNING: LOG_WAIT enabled with Logging disabled (LOGLEVEL==0)!")
endif()

# Add the standard library to the build
target_link_libraries(  RP2040-Decoder
                        pico_stdlib
                        pico_multicore
                        pico_flash
                        hardware_pwm
                        hardware_adc
                        hardware_flash
                        hardware_watchdog
                        hardware_exception  )

# Add the standard include files to the build
target_include_directories(RP2040-Decoder PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# create map/bin/hex file etc.
pico_add_extra_outputs(RP2040-Decoder)

# Program size check
#
# Check for program size exceeding limits as CVs are stored at the end of flash, the program could theoretically overwrite this.
# In practice the program size is usually does not exceed about 100KiB, the actual size is printed by CMake from arm-none-eabi-size.
# Extract the directory of the C compiler and construct the path to the 'arm-none-eabi-size' tool
get_filename_component(C_COMPILER_DIR ${CMAKE_C_COMPILER} DIRECTORY)
set(SIZE_TOOL ${C_COMPILER_DIR}/arm-none-eabi-size)
set(BOARD_HEADER "${CMAKE_SOURCE_DIR}/${PICO_BOARD}.h")
set(ELF_FILE "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf")
add_custom_command(TARGET RP2040-Decoder POST_BUILD
    COMMAND ${CMAKE_COMMAND} 
    -D BOARD_HEADER=${BOARD_HEADER} 
    -D SIZE_TOOL=${SIZE_TOOL} 
    -D ELF_FILE=${ELF_FILE}
    -P ${CMAKE_SOURCE_DIR}/post_build.cmake
)