cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(RP2040-Decoder C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_DECODER_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32 _t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(
        RP2040-Decoder
        core0.c
        core0.h
        core1.c
        core1.h
        shared.h)


# Add pico_multicore which is required for multicore functionality
target_link_libraries(
        RP2040-Decoder
        pico_stdlib
        hardware_pwm
        hardware_adc
        hardware_flash
        pico_multicore
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(RP2040-Decoder 1)
pico_enable_stdio_uart(RP2040-Decoder 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(RP2040-Decoder)
