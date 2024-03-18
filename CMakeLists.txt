cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(shifter C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in project for the libraries
pico_sdk_init()

add_executable(shifter)

target_sources(shifter PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
)

target_include_directories(shifter PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

# Add pico_stdlib library, add more if used
target_link_libraries(shifter pico_stdlib hardware_adc pico_unique_id tinyusb_device tinyusb_board)

# enable usb output, disable uart output
pico_enable_stdio_usb(shifter 1)
pico_enable_stdio_uart(shifter 0)

# Need to generate UF2 file for upload to RP2040
pico_add_extra_outputs(shifter)