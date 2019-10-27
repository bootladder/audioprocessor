cmake_minimum_required(VERSION 3.0)
PROJECT(bsp-bringup)

########################################
# NAME
#######################################
set(TARGET_BASENAME stm32f769-discovery-bsp-bringup)

#############################################################
# PATH TO THE OUTPUT EXECUTABLE
############################################################
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")


########################################
# TOOLCHAIN
#######################################
set(TOOLCHAIN_PATH "/opt/gcc-arm-none-eabi-7-2018-q2-update/bin")

set(CMAKE_C_COMPILER "${TOOLCHAIN_PATH}/arm-none-eabi-gcc")
set(OBJCOPY "${TOOLCHAIN_PATH}/arm-none-eabi-objcopy")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PATH}/arm-none-eabi-as")
enable_language(ASM)

########################################
# SOURCE
#######################################
set(SOURCE_ROOT_DIR "../")
SET(C_SOURCES "")

LIST(APPEND C_SOURCES "${SOURCE_ROOT_DIR}/Application/main.c"                                      )

########################################
# INCLUDES
#######################################
include_directories("./Application")
include_directories("./Drivers")
include_directories("./Drivers/STM32F7xx_HAL_Drivers/Inc")
include_directories("./Drivers/CMSIS/Device")
include_directories("./Drivers/CMSIS/Include")
include_directories("./FreeRTOS/include")
include_directories("./FreeRTOS/")
include_directories("./FreeRTOS/portable/GCC/ARM_CM7_MPU/r0p1/")

include_directories("./Help")

#######################################
# FLAGS
#######################################
set(CMAKE_C_FLAGS "  -std=c99                      ")
string(APPEND CMAKE_C_FLAGS " -Wall                         ")
string(APPEND CMAKE_C_FLAGS " -g -Wall                         ")
string(APPEND CMAKE_C_FLAGS "  -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-d16 -DSTM32F769xx -DUSE_HAL_DRIVER -DUSE_STM32F769I_DISCO -DUSE_USB_HS ")

# Avoid known bug in linux giving:
#    arm-none-eabi-gcc: error: unrecognized command line option '-rdynamic'
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")


SET(PATH_TO_LINKERSCRIPT "../STM32F769NIHx_FLASH.ld")
set(TARGET_LINK_FLAGS "")

string(APPEND TARGET_LINK_FLAGS " -specs=nosys.specs -specs=nano.specs ")
string(APPEND TARGET_LINK_FLAGS " -T${PATH_TO_LINKERSCRIPT}  ")
string(APPEND TARGET_LINK_FLAGS " -Wl,-Map=output.map  ")


#############################################################
# CONFIGURE EXECUTABLE
############################################################
add_executable(${TARGET_BASENAME}  ${C_SOURCES} ${ASM_SOURCES})

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
