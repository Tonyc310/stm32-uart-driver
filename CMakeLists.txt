cmake_minimum_required(VERSION 3.25)
project(stm32_uart_driver LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(FetchContent)

add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE -Wall -Wextra -Werror)

# Hardware-independent code: linked into the firmware and unit-tested on the host.
add_library(portable STATIC src/ring_buffer.c src/shell.c src/crc16.c src/cobs.c src/telemetry.c)
target_include_directories(portable PUBLIC src)
target_link_libraries(portable PRIVATE project_warnings)

if(NOT CMAKE_CROSSCOMPILING)
    enable_testing()
    add_subdirectory(tests/unit)
    return()
endif()

enable_language(ASM)

FetchContent_Declare(cmsis_core
    GIT_REPOSITORY https://github.com/ARM-software/CMSIS_6.git
    GIT_TAG        v6.3.0
    GIT_SHALLOW    TRUE)
FetchContent_Declare(cmsis_device_f4
    GIT_REPOSITORY https://github.com/STMicroelectronics/cmsis-device-f4.git
    GIT_TAG        v2.6.11
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(cmsis_core cmsis_device_f4)

# ST's startup code and SystemInit, kept apart so our warning flags don't apply to vendor code.
add_library(cmsis_device OBJECT
    ${cmsis_device_f4_SOURCE_DIR}/Source/Templates/gcc/startup_stm32f407xx.s
    ${cmsis_device_f4_SOURCE_DIR}/Source/Templates/system_stm32f4xx.c)
target_include_directories(cmsis_device SYSTEM PUBLIC
    ${cmsis_core_SOURCE_DIR}/CMSIS/Core/Include
    ${cmsis_device_f4_SOURCE_DIR}/Include)
target_compile_definitions(cmsis_device PUBLIC STM32F407xx)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/platform/stm32f407vg.ld)

add_executable(stm32-uart-driver src/main.c src/board.c src/uart.c)
set_target_properties(stm32-uart-driver PROPERTIES
    SUFFIX .elf
    LINK_DEPENDS ${LINKER_SCRIPT})
target_link_libraries(stm32-uart-driver PRIVATE cmsis_device portable project_warnings)
target_link_options(stm32-uart-driver PRIVATE
    -T${LINKER_SCRIPT}
    -Wl,-Map=$<TARGET_FILE_DIR:stm32-uart-driver>/stm32-uart-driver.map
    -Wl,--print-memory-usage)

find_program(RENODE_TEST renode-test)
if(RENODE_TEST)
    enable_testing()
    set(RENODE_RESULTS_DIR ${CMAKE_BINARY_DIR}/renode)
    file(MAKE_DIRECTORY ${RENODE_RESULTS_DIR})
    # renode-test always writes its results to the working directory, so keep them in the build tree.
    add_test(NAME renode_uart_shell
        COMMAND ${RENODE_TEST} ${CMAKE_SOURCE_DIR}/tests/sim/uart_shell.robot
        WORKING_DIRECTORY ${RENODE_RESULTS_DIR})
endif()
