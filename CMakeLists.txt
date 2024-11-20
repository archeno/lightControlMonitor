CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR cortex-m4)
#SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(CMAKE_C_COMPILER "D:/programfiles/env-windows-v1.3.5/tools/ConEmu/ConEmu/../../../tools/gnu_gcc/arm_gcc/mingw/bin/arm-none-eabi-gcc.exe")
SET(CMAKE_ASM_COMPILER "D:/programfiles/env-windows-v1.3.5/tools/ConEmu/ConEmu/../../../tools/gnu_gcc/arm_gcc/mingw/bin/arm-none-eabi-gcc.exe")
SET(CMAKE_C_FLAGS " -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections -Dgcc -O0 -gdwarf-2 -g")
SET(CMAKE_ASM_FLAGS " -c -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections -x assembler-with-cpp -Wa,-mimplicit-it=thumb  -gdwarf-2")
SET(CMAKE_C_COMPILER_WORKS TRUE)

SET(CMAKE_CXX_COMPILER "D:/programfiles/env-windows-v1.3.5/tools/ConEmu/ConEmu/../../../tools/gnu_gcc/arm_gcc/mingw/bin/arm-none-eabi-g++.exe")
SET(CMAKE_CXX_FLAGS " -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections -Dgcc -O0 -gdwarf-2 -g")
SET(CMAKE_CXX_COMPILER_WORKS TRUE)

SET(CMAKE_OBJCOPY "D:/programfiles/env-windows-v1.3.5/tools/ConEmu/ConEmu/../../../tools/gnu_gcc/arm_gcc/mingw/bin/arm-none-eabi-objcopy.exe")
SET(CMAKE_SIZE "D:/programfiles/env-windows-v1.3.5/tools/ConEmu/ConEmu/../../../tools/gnu_gcc/arm_gcc/mingw/bin/arm-none-eabi-size.exe")

SET(CMAKE_EXE_LINKER_FLAGS " -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections -Wl,--gc-sections,-Map=rtthread.map,-cref,-u,Reset_Handler -T ${CMAKE_SOURCE_DIR}/board/linker_scripts/link.ld")

SET(CMAKE_CXX_STANDARD 14)
PROJECT(rtthread C CXX ASM)
INCLUDE_DIRECTORIES(
	packages/agile_modbus-v1.1.2/inc
	packages/agile_modbus-v1.1.2/util
	applications
	.
	applications/inc
	packages/at24cxx-latest
	rt-thread/components/libc/compilers/common/include
	rt-thread/components/libc/compilers/newlib
	rt-thread/libcpu/arm/common
	rt-thread/libcpu/arm/cortex-m4
	rt-thread/components/drivers/include
	rt-thread/components/drivers/spi
	board
	libraries/gd32_drivers
	rt-thread/components/finsh
	rt-thread/include
	libraries/GD32F4xx_Firmware_Library/CMSIS/GD/GD32F4xx/Include
	libraries/GD32F4xx_Firmware_Library/CMSIS
	libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Include
	rt-thread/components/libc/posix/io/epoll
	rt-thread/components/libc/posix/io/eventfd
	rt-thread/components/libc/posix/io/poll
	rt-thread/components/libc/posix/ipc
)

ADD_DEFINITIONS(
	-DGD32F425
	-DRT_USING_LIBC
	-DRT_USING_NEWLIBC
	-DUSE_STDPERIPH_DRIVER
	-D_POSIX_C_SOURCE=1
	-D__RTTHREAD__
)

SET(PROJECT_SOURCES
	packages/agile_modbus-v1.1.2/util/agile_modbus_slave_util.c
	packages/agile_modbus-v1.1.2/src/agile_modbus_rtu.c
	packages/agile_modbus-v1.1.2/src/agile_modbus_tcp.c
	packages/agile_modbus-v1.1.2/src/agile_modbus.c
	applications/tca9535.c
	applications/database.c
	applications/key_function.c
	applications/hmi.c
	applications/i2c_app.c
	applications/main.c
	applications/multi_button.c
	applications/crc16.c
	applications/spi_driver.c
	applications/can_response.c
	applications/modbus_master.c
	applications/uart5.c
	applications/mycan.c
	applications/pulse_generate.c
	applications/event_monitor.c
	applications/lms.c
	applications/adc_sample.c
	applications/global.c
	applications/dma_uart.c
	applications/rs_485.c
	applications/io.c
	applications/lmsuart.c
	applications/oled_driver.c
	packages/at24cxx-latest/at24cxx.c
	rt-thread/components/libc/compilers/common/cctype.c
	rt-thread/components/libc/compilers/common/cstdlib.c
	rt-thread/components/libc/compilers/common/cstring.c
	rt-thread/components/libc/compilers/common/ctime.c
	rt-thread/components/libc/compilers/common/cunistd.c
	rt-thread/components/libc/compilers/common/cwchar.c
	rt-thread/components/libc/compilers/newlib/syscalls.c
	rt-thread/libcpu/arm/common/div0.c
	rt-thread/libcpu/arm/common/showmem.c
	rt-thread/libcpu/arm/cortex-m4/context_gcc.S
	rt-thread/libcpu/arm/cortex-m4/cpuport.c
	rt-thread/components/drivers/can/can.c
	rt-thread/components/drivers/core/device.c
	rt-thread/components/drivers/hwtimer/hwtimer.c
	rt-thread/components/drivers/i2c/i2c-bit-ops.c
	rt-thread/components/drivers/i2c/i2c_core.c
	rt-thread/components/drivers/i2c/i2c_dev.c
	rt-thread/components/drivers/ipc/completion.c
	rt-thread/components/drivers/ipc/dataqueue.c
	rt-thread/components/drivers/ipc/pipe.c
	rt-thread/components/drivers/ipc/ringblk_buf.c
	rt-thread/components/drivers/ipc/ringbuffer.c
	rt-thread/components/drivers/ipc/waitqueue.c
	rt-thread/components/drivers/ipc/workqueue.c
	rt-thread/components/drivers/misc/adc.c
	rt-thread/components/drivers/misc/pin.c
	rt-thread/components/drivers/serial/serial.c
	rt-thread/components/drivers/spi/spi_core.c
	rt-thread/components/drivers/spi/spi_dev.c
	rt-thread/components/drivers/watchdog/watchdog.c
	board/board.c
	libraries/GD32F4xx_Firmware_Library/CMSIS/GD/GD32F4xx/Source/GCC/startup_gd32f4xx.s
	libraries/gd32_drivers/drv_adc.c
	libraries/gd32_drivers/drv_can.c
	libraries/gd32_drivers/drv_gpio.c
	libraries/gd32_drivers/drv_hwtimer.c
	libraries/gd32_drivers/drv_soft_i2c.c
	libraries/gd32_drivers/drv_spi.c
	libraries/gd32_drivers/drv_usart.c
	libraries/gd32_drivers/drv_wdt.c
	rt-thread/components/finsh/shell.c
	rt-thread/components/finsh/msh.c
	rt-thread/components/finsh/msh_parse.c
	rt-thread/components/finsh/cmd.c
	rt-thread/src/clock.c
	rt-thread/src/components.c
	rt-thread/src/idle.c
	rt-thread/src/ipc.c
	rt-thread/src/irq.c
	rt-thread/src/kservice.c
	rt-thread/src/mem.c
	rt-thread/src/mempool.c
	rt-thread/src/object.c
	rt-thread/src/scheduler_up.c
	rt-thread/src/thread.c
	rt-thread/src/timer.c
	libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_can.c
	libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_timer.c
	libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_i2c.c
	libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_syscfg.c
	libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_dma.c
	libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_exti.c
	libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_gpio.c
	libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_rcu.c
	libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_fwdgt.c
	libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_misc.c
	libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_usart.c
	libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_spi.c
	libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_adc.c
	libraries/GD32F4xx_Firmware_Library/CMSIS/GD/GD32F4xx/Source/system_gd32f4xx.c
	libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Source/gd32f4xx_wwdgt.c
)

LINK_DIRECTORIES(
)

LINK_LIBRARIES(
	c
	m
)

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME}.elf ${PROJECT_SOURCES})
ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD 
COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin COMMAND ${CMAKE_SIZE} ${CMAKE_PROJECT_NAME}.elf)