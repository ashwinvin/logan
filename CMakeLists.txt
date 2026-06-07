cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on) # for clangd lsp

set(PICO_SDK_FETCH_FROM_GIT on)
set(PICO_PLATFORM "rp2040")

include(pico_sdk_import.cmake)

set(FREERTOS_KERNEL_PATH ${CMAKE_SOURCE_DIR}/freertos)
include(FreeRTOS_Kernel_import.cmake)

project(logan C CXX ASM)

pico_sdk_init()

add_executable(logan 
	src/main.c
	src/usb_descriptors.c
)

target_compile_options(logan PRIVATE -Wall)

target_include_directories(logan PRIVATE src)

# USB serial interface
pico_enable_stdio_usb(logan 1) 

target_link_libraries(logan PRIVATE 
	pico_stdlib 
	pico_unique_id 
	tinyusb_device 
	tinyusb_board
	FreeRTOS-Kernel
	FreeRTOS-Kernel-Heap1
)

pico_add_extra_outputs(logan)

