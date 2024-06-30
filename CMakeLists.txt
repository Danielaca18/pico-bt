cmake_minimum_required(VERSION 3.14)

# Set project and target name
set(PROJECT_NAME "pico_bt")
set(TARGET_NAME "pico_bt")
set(LIBRARY_NAME "pico_btlib")

# Include project configs
include(${CMAKE_SOURCE_DIR}/cmake/import_config.cmake)

# Include pico sdk and extras
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(${PICO_EXTRAS_PATH}/external/pico_extras_import.cmake)

# Project name, version and languages
project(${PROJECT_NAME} VERSION 1.0 LANGUAGES C CXX ASM)

# init pico sdk
pico_sdk_init()

# Add directories
add_subdirectory(src)
add_subdirectory(inc)

# Setup executable
add_executable(${TARGET_NAME} ${SRC} ${INC})
target_link_libraries(${TARGET_NAME} 
    pico_stdlib 
    pico_unique_id 
    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_none
)

pico_btstack_make_gatt_header(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/example.gatt")

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

# Link stdio to usb serial
pico_enable_stdio_usb(${TARGET_NAME} 1)
pico_enable_stdio_uart(${TARGET_NAME} 1)

# Generate uf2 file for flashing
pico_add_extra_outputs(${TARGET_NAME})
