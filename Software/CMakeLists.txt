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
# Please select RP2040-Decoder-board.h for Rev >= 1.0 or RP2040-Decoder-board-legacy.h for Rev < 1.0
set(PICO_BOARD RP2040-Decoder-board.h CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(RP2040-Decoder C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_compile_options(-Wall
                    -Ofast
                    -g3
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
# In practice the program size is usually only about 100KiB.
# Extract the directory of the C compiler and construct the path to the 'arm-none-eabi-size' tool
get_filename_component(C_COMPILER_DIR ${CMAKE_C_COMPILER} DIRECTORY)
set(SIZE_TOOL ${C_COMPILER_DIR}/arm-none-eabi-size)

# Add custom command to check the flash size after build
add_custom_command(TARGET RP2040-Decoder POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Checking flash size for $<TARGET_FILE:RP2040-Decoder>"
    COMMAND ${SIZE_TOOL} $<TARGET_FILE:RP2040-Decoder> > size_output.txt
)

# Read the size output file
file(READ "${CMAKE_BINARY_DIR}/size_output.txt" SIZE_OUTPUT)

# Print the content of SIZE_OUTPUT
message(STATUS "arm-none-eabi-size output:\n${SIZE_OUTPUT}")

# Use regex to match all numbers
string(REGEX MATCHALL "[0-9]+" MATCHED_VALUES "${SIZE_OUTPUT}")

# Get third element from list of matches, this corresponds with dec table value in "size_output.txt"
list(GET MATCHED_VALUES 3 PROGRAM_SIZE_DEC)

# Check for success of program size extraction from "size_output.txt"
if(PROGRAM_SIZE_DEC)
    message(STATUS "PROGRAM_SIZE_DEC: ${PROGRAM_SIZE_DEC} bytes")
else()
    message(FATAL_ERROR "Could not extract program size from size_output.txt")
endif()


# Path to the header file that defines PICO_FLASH_SIZE_BYTES 
set(FLASH_CONFIG_HEADER ${CMAKE_SOURCE_DIR}/${PICO_BOARD}.h)
# Read the contents of the header file into a variable
file(READ ${FLASH_CONFIG_HEADER} HEADER_CONTENT)

# Extract the value of PICO_FLASH_SIZE_BYTES using a regular expression
string(REGEX MATCHALL "#define PICO_FLASH_SIZE_BYTES [0-9]+" MACRO_MATCH "${HEADER_CONTENT}")
string(REGEX MATCHALL "[0-9]+" PICO_FLASH_SIZE_BYTES "${MACRO_MATCH}")


# Check for success of PICO_FLASH_SIZE_BYTES extraction from board header file
if(PICO_FLASH_SIZE_BYTES)
    message(STATUS "PICO_FLASH_SIZE_BYTES: ${PICO_FLASH_SIZE_BYTES} bytes")
else()
    message(FATAL_ERROR "Failed to find PICO_FLASH_SIZE_BYTES in the header file.")
endif()

# Sector size is always 4096 bytes
set(FLASH_SECTOR_SIZE 4096)
message(STATUS "FLASH_SECTOR_SIZE: ${FLASH_SECTOR_SIZE} bytes")
# Subtract flash sector size from flash size
math(EXPR FLASH_TARGET_OFFSET "${PICO_FLASH_SIZE_BYTES} - ${FLASH_SECTOR_SIZE}")
message(STATUS "FLASH_TARGET_OFFSET: ${FLASH_TARGET_OFFSET} bytes")

if(PROGRAM_SIZE_DEC GREATER FLASH_TARGET_OFFSET)
    message(FATAL_ERROR "Program size exceeds allowed flash size. Flash sector containing the CVs would be overwritten! Please reduce the size of the program.")
endif()