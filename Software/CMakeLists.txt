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
set(PICO_BOARD RP2040-Decoder-board CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(RP2040-Decoder C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_compile_options(#-Wall
                    -O0
                    -Wno-format          # int != int32 _t as far as the compiler is concerned because gcc has int32_t as long int
                    -Wno-unused-function # we have some for the docs that aren't called
                    -Wno-maybe-uninitialized)

# Add executable
add_executable( RP2040-Decoder 
                core0.c
                core0.h
                core1.c
                core1.h
                shared.c
                shared.h )

pico_set_program_name(RP2040-Decoder "RP2040-Decoder")
pico_set_program_version(RP2040-Decoder "1.0")


# LOGLEVEL>0 enables debugging via stdio, this can be done either via USB or alternatively,
# directly with the UART TX/RX pins (see pin definition in RP2040-Decoder-board.h).
#
# In order to use stdio via USB please set STDIO_USB_ENABLED to 1 below
# In order to use stdio via UART TX/RX pins please set STDIO_UART_ENABLED to 1 below
#
# Additional Notes:
# - When logging is disabled (LOGLEVEL 0), all logging functions will be completely excluded during compilation.
# - Be cautious: Logging can alter timing behavior. For example, the PID controller is time critical,
#   limit the frequency of log outputs—such as printing only every 100th iteration.
# - Note that with UART logging enabled, PICO_DEFAULT_UART_TX_PIN and PICO_DEFAULT_UART_RX_PIN are disabled for decoder functions
#   because they are used for UART communication. This is automatically configured for you below.
set(LOGLEVEL 1)
# Additionally, the LOG_WAIT parameter can be configured to make the controller wait for user input via stdio before continuing.
# This is useful for ensuring that all stdio logging outputs are captured.
# If disabled, there may not be enough time to connect the serial terminal before most of the logging has already been completed.
set(LOG_WAIT 1)
# Modify the below lines to enable/disable output over UART and/or USB
set(STDIO_USB_ENABLED 1)
set(STDIO_UART_ENABLED 1)
# Leave those two unmodified
pico_enable_stdio_uart(RP2040-Decoder ${STDIO_UART_ENABLED})
pico_enable_stdio_usb(RP2040-Decoder ${STDIO_USB_ENABLED})
# Add a preprocessor definition which defines whether UART is used or not. 
# This is done to make the C-Code know whether pins used for UART need to be reserved and restricted from setting them as PWM/GPIO.
target_compile_definitions( RP2040-Decoder PRIVATE 
                            LOGLEVEL=${LOGLEVEL}
                            LOG_WAIT=${LOG_WAIT}
                            STDIO_UART_ENABLED=${STDIO_UART_ENABLED}
                            STDIO_USB_ENABLED=${STDIO_USB_ENABLED})
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
                        hardware_pwm
                        hardware_adc
                        hardware_flash
                        pico_multicore
                        pico_flash)

# Add the standard include files to the build
target_include_directories(RP2040-Decoder PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

# create map/bin/hex file etc.
pico_add_extra_outputs(RP2040-Decoder)

