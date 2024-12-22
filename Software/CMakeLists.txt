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

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(RP2040-Decoder 0)
pico_enable_stdio_usb(RP2040-Decoder 1)

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

