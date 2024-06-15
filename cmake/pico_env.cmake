
# Board type
set(FAMILY rp2040)
set(PICO_PLATFORM rp2040)
set(PICO_BOARD pico_w CACHE STRING "Board type")
set(BOARD pico_sdk)

# Pico SDK and extras
if(NOT DEFINED ENV{PICO_SDK_PATH})
    message(FATAL_ERROR "PICO_SDK_PATH environment variable is not defined")
endif()
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
set(PICO_EXTRAS_PATH $ENV{PICO_EXTRAS_PATH})

# Pico toolchain and modules
set(CMAKE_TOOLCHAIN_FILE ${PICO_SDK_PATH}/cmake/preload/toolchains/pico_arm_gcc.cmake)
if(WIN32)
    set(CMAKE_MODULE_PATH ${PICO_INSTALL_PATH}/)
endif()