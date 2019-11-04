cmake_minimum_required(VERSION 3.0)

########################################
# NAME
#######################################
set(TARGET_BASENAME stm32f769-discovery-audio-processor)
project(audio-processor)

#############################################################
# OUTPUT PATHS
############################################################
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")

########################################
# SOURCE
#######################################
set(SOURCE_ROOT_DIR "../")

set(C_SOURCES_FOR_TESTING
  "${SOURCE_ROOT_DIR}/Application/ProcessBlockFunctions.cpp"
  "${SOURCE_ROOT_DIR}/Application/MIDIMessageHandler.cpp"
  "${SOURCE_ROOT_DIR}/Help/MemoryLogger.c"
  "${SOURCE_ROOT_DIR}/Help/tinyprintf.c"
  )

set(C_SOURCES_NOT_FOR_TESTING
  "${SOURCE_ROOT_DIR}/Application/AudioProcessor.cpp"
  "${SOURCE_ROOT_DIR}/Application/MIDI_Input_Task.c"
  "${SOURCE_ROOT_DIR}/Application/ProcessBlockFunctions_FIRFilters.cpp"

  "${SOURCE_ROOT_DIR}/CPU/startup_stm32f769xx.s"
  "${SOURCE_ROOT_DIR}/CPU/stm32f7xx_it.c"
  "${SOURCE_ROOT_DIR}/Application/main.c"
  "${SOURCE_ROOT_DIR}/Drivers/CMSIS/Device/system_stm32f7xx.c"
  "${SOURCE_ROOT_DIR}/Drivers/STM32F7xx_HAL_Drivers/Src/stm32f7xx_hal_rcc.c"
  "${SOURCE_ROOT_DIR}/Drivers/STM32F7xx_HAL_Drivers/Src/stm32f7xx_hal_rcc_ex.c"
  "${SOURCE_ROOT_DIR}/Drivers/STM32F7xx_HAL_Drivers/Src/stm32f7xx_hal.c"
  "${SOURCE_ROOT_DIR}/Drivers/STM32F7xx_HAL_Drivers/Src/stm32f7xx_hal_gpio.c"
  "${SOURCE_ROOT_DIR}/Drivers/STM32F7xx_HAL_Drivers/Src/stm32f7xx_hal_cortex.c"
  "${SOURCE_ROOT_DIR}/Drivers/STM32F7xx_HAL_Drivers/Src/stm32f7xx_hal_pwr_ex.c"
  "${SOURCE_ROOT_DIR}/Drivers/STM32F7xx_HAL_Drivers/Src/stm32f7xx_hal_sai.c"
  "${SOURCE_ROOT_DIR}/Drivers/STM32F7xx_HAL_Drivers/Src/stm32f7xx_hal_dma.c"
  "${SOURCE_ROOT_DIR}/Drivers/STM32F7xx_HAL_Drivers/Src/stm32f7xx_hal_i2c.c"
  "${SOURCE_ROOT_DIR}/Drivers/STM32F7xx_HAL_Drivers/Src/stm32f7xx_hal_uart.c"

  "${SOURCE_ROOT_DIR}/Drivers/BSP/BSP_Audio_Init.c"
  "${SOURCE_ROOT_DIR}/Drivers/BSP/BSP_Audio_Task.c"
  "${SOURCE_ROOT_DIR}/Drivers/BSP/BSP_Bringup.c"
  "${SOURCE_ROOT_DIR}/Drivers/BSP/BSP_UART.c"
  "${SOURCE_ROOT_DIR}/Drivers/BSP/BSP_Fast_UART.c"
  "${SOURCE_ROOT_DIR}/Drivers/BSP/BSP_LED.c"
  "${SOURCE_ROOT_DIR}/Drivers/BSP/BSP_Audio_Codec_IO.c"
  "${SOURCE_ROOT_DIR}/Drivers/BSP/wm8994.c"

  "${SOURCE_ROOT_DIR}/Help/SerialLogger.c"

  "${SOURCE_ROOT_DIR}/FreeRTOS/portable/GCC/ARM_CM7/r0p1/port.c"
  "${SOURCE_ROOT_DIR}/FreeRTOS/tasks.c"
  "${SOURCE_ROOT_DIR}/FreeRTOS/list.c"
  "${SOURCE_ROOT_DIR}/FreeRTOS/timers.c"
  "${SOURCE_ROOT_DIR}/FreeRTOS/queue.c"
  "${SOURCE_ROOT_DIR}/FreeRTOS/stream_buffer.c"
  "${SOURCE_ROOT_DIR}/FreeRTOS/event_groups.c"
  "${SOURCE_ROOT_DIR}/FreeRTOS/portable/MemMang/heap_4.c"
  )

SET(C_SOURCES "")
LIST(APPEND C_SOURCES ${C_SOURCES_FOR_TESTING})
LIST(APPEND C_SOURCES ${C_SOURCES_NOT_FOR_TESTING})

########################################
# INCLUDES
#######################################
SET(INCLUDES_FOR_TESTING
  "./Application"
  "./Help"
  )


SET(INCLUDES_NOT_FOR_TESTING
  "./Drivers/BSP"
  "./Drivers/STM32F7xx_HAL_Drivers/Inc"
  "./Drivers/CMSIS/Device"
  "./Drivers/CMSIS/Include"
  "./FreeRTOS/include"
  "./FreeRTOS/"
  "./FreeRTOS/portable/GCC/ARM_CM7/r0p1/"
  "./static-libs"
  )

include_directories(${INCLUDES_NOT_FOR_TESTING})
include_directories(${INCLUDES_FOR_TESTING})

#######################################
# FLAGS
#######################################
set(COMMON_FLAGS " ")
string(APPEND COMMON_FLAGS " -Wall   -Wextra   -g                   ")

set(TARGET_SPECIFIC_FLAGS " ")
string(APPEND TARGET_SPECIFIC_FLAGS "  -mcpu=cortex-m7 -mthumb  -DARM_MATH_CM7 ")
string(APPEND TARGET_SPECIFIC_FLAGS " -mfloat-abi=hard -mfpu=fpv5-d16 -DSTM32F769xx -DUSE_HAL_DRIVER ")

set(CMAKE_C_FLAGS "  -std=c99                      ")
set(CMAKE_CXX_FLAGS "  -std=c++11                      ")
string(APPEND CMAKE_C_FLAGS ${COMMON_FLAGS})
string(APPEND CMAKE_CXX_FLAGS ${COMMON_FLAGS})

set(TARGET_LINK_FLAGS "")
string(APPEND TARGET_LINK_FLAGS " -Wl,-Map=output.map  ")

if(BUILD_TEST)

else()
  string(APPEND CMAKE_C_FLAGS ${TARGET_SPECIFIC_FLAGS})
  string(APPEND CMAKE_CXX_FLAGS ${TARGET_SPECIFIC_FLAGS})

  # Avoid known bug in linux giving:
  #    arm-none-eabi-gcc: error: unrecognized command line option '-rdynamic'
  set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
  set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

  SET(PATH_TO_LINKERSCRIPT "../CPU/STM32F769NIHx_FLASH.ld")

  string(APPEND TARGET_LINK_FLAGS " -specs=nosys.specs -specs=nano.specs ")
  string(APPEND TARGET_LINK_FLAGS " -T${PATH_TO_LINKERSCRIPT}  ")

endif()


########################################
# TOOLCHAIN
#######################################
if(BUILD_TEST)
  #do nothing, use default system tools
else()
  set(TOOLCHAIN_PATH "/opt/gcc-arm-none-eabi-7-2018-q2-update/bin")
  set(CMAKE_C_COMPILER   "${TOOLCHAIN_PATH}/arm-none-eabi-gcc")
  set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PATH}/arm-none-eabi-g++")
  set(OBJCOPY            "${TOOLCHAIN_PATH}/arm-none-eabi-objcopy")
  set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PATH}/arm-none-eabi-as")
  set(CMAKE_AR           "${TOOLCHAIN_PATH}/arm-none-eabi-ar")
  enable_language(ASM)
endif()

#############################################################
# EXECUTABLE (CROSS COMPILE)
############################################################
if(NOT BUILD_TEST)
  add_executable(${TARGET_BASENAME}  ${C_SOURCES} ${ASM_SOURCES})
  target_link_libraries(${TARGET_BASENAME} "-L../static-libs/" "-l:libarm_cortexM7lfdp_math.a")

  set_target_properties(${TARGET_BASENAME} PROPERTIES LINK_FLAGS ${TARGET_LINK_FLAGS} )

  EXEC_PROGRAM(
    "echo $(git rev-parse --abbrev-ref HEAD)_$(git rev-parse --short HEAD)"
    OUTPUT_VARIABLE gitrepoinfo
    )

  set(output_exe_basename ${TARGET_BASENAME}-${gitrepoinfo})
  set(output_exe_elfname  "${output_exe_basename}.elf")
  set_target_properties(${TARGET_BASENAME} PROPERTIES OUTPUT_NAME ${output_exe_elfname})

  add_custom_command(TARGET ${TARGET_BASENAME} POST_BUILD
    COMMAND size ${EXECUTABLE_OUTPUT_PATH}/${output_exe_elfname}
    )
  add_custom_command(TARGET ${TARGET_BASENAME} POST_BUILD
    COMMAND cp ${EXECUTABLE_OUTPUT_PATH}/${output_exe_elfname} ${EXECUTABLE_OUTPUT_PATH}/latest.elf
  )


#############################################################
# SOURCE LIBRARY FOR TESTING
############################################################
else()
  set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")

  add_library(${TARGET_BASENAME}-for-testing STATIC ${C_SOURCES_FOR_TESTING})

#############################################################
# TEST EXECUTABLE (HOST COMPILED)
############################################################

  set(UNIT_TEST_TARGET_BASENAME ${TARGET_BASENAME}-unit-tests)

  set(Sources
    test/ExampleTests.cpp
    test/ProcessBlockTests.cpp
    test/MIDIMessageHandlerTests.cpp
    test/MIDIMapTests.cpp
    test/FAKE_ProcessBlockFunctions_FIRFilters.cpp
    test/FAKE_SerialLogger.c
    )
  include_directories("test/googletest/googletest/include")
  include_directories("test/googletest/googlemock/include")

  add_executable(${UNIT_TEST_TARGET_BASENAME} ${Sources})
  target_link_libraries(${UNIT_TEST_TARGET_BASENAME} PUBLIC gtest_main gtest gmock pthread "-L../test/googletest/build/lib" ${TARGET_BASENAME}-for-testing)

  add_custom_command(TARGET ${UNIT_TEST_TARGET_BASENAME} POST_BUILD
    COMMAND size ${EXECUTABLE_OUTPUT_PATH}/${UNIT_TEST_TARGET_BASENAME}
    )
  add_custom_command(TARGET ${UNIT_TEST_TARGET_BASENAME} POST_BUILD
    COMMAND cp ${EXECUTABLE_OUTPUT_PATH}/${UNIT_TEST_TARGET_BASENAME} ${EXECUTABLE_OUTPUT_PATH}/unit-tests.elf
  )

endif()
