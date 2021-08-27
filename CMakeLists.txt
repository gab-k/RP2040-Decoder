cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_decoder C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_DECODER_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(
        hello_multicore
        multicore.c
        automat.c
        )

pico_generate_pio_header(hello_multicore ${CMAKE_CURRENT_LIST_DIR}/pwm.pio)

# Add pico_multicore which is required for multicore functionality
target_link_libraries(
        hello_multicore
        pico_stdlib
        hardware_pio
        hardware_pwm
        pico_multicore
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(hello_multicore 1)
pico_enable_stdio_uart(hello_multicore 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(hello_multicore)
