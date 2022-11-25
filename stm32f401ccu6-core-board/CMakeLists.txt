CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR cortex-m4)
#SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(CMAKE_C_COMPILER "/usr/bin/arm-none-eabi-gcc")
SET(CMAKE_ASM_COMPILER "/usr/bin/arm-none-eabi-gcc")
SET(CMAKE_C_FLAGS " -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections -Dgcc -O0 -gdwarf-2 -g")
SET(CMAKE_ASM_FLAGS " -c -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections -x assembler-with-cpp -Wa,-mimplicit-it=thumb  -gdwarf-2")
SET(CMAKE_C_COMPILER_WORKS TRUE)

SET(CMAKE_CXX_COMPILER "/usr/bin/arm-none-eabi-g++")
SET(CMAKE_CXX_FLAGS " -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections -Dgcc -O0 -gdwarf-2 -g")
SET(CMAKE_CXX_COMPILER_WORKS TRUE)

SET(CMAKE_OBJCOPY "/usr/bin/arm-none-eabi-objcopy")
SET(CMAKE_SIZE "/usr/bin/arm-none-eabi-size")

SET(CMAKE_EXE_LINKER_FLAGS " -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections -Wl,--gc-sections,-Map=rt-thread.map,-cref,-u,Reset_Handler -T ${CMAKE_SOURCE_DIR}/board/linker_scripts/link.lds")

SET(CMAKE_CXX_STANDARD 14)
PROJECT(rtthread C CXX ASM)
INCLUDE_DIRECTORIES(
	applications
	../../../components/libc/compilers/newlib
	../../../components/libc/compilers/common/include
	../../../libcpu/arm/common
	../../../libcpu/arm/cortex-m4
	../../../components/drivers/include
	board
	board/CubeMX_Config/Inc
	../libraries/HAL_Drivers
	../libraries/HAL_Drivers/config
	../libraries/HAL_Drivers/CMSIS/Include
	../../../components/finsh
	.
	../../../include
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Inc
	../libraries/STM32F4xx_HAL/CMSIS/Device/ST/STM32F4xx/Include
	../../../components/libc/posix/ipc
	../../../components/libc/posix/io/stdio
	../../../components/libc/posix/io/poll
	../../../components/drivers/usb/usbdevice
)

ADD_DEFINITIONS(
	-DHAVE_CCONFIG_H
	-DRT_USING_LIBC
	-DRT_USING_NEWLIBC
	-DSTM32F401xC
	-DUSE_HAL_DRIVER
	-D_POSIX_C_SOURCE=1
	-D__RTTHREAD__
)

SET(PROJECT_SOURCES
	applications/main.c
	applications/SerialPortService.c
	applications/SerialPort.c
	../../../components/libc/compilers/common/cctype.c
	../../../components/libc/compilers/common/cstdio.c
	../../../components/libc/compilers/common/cstdlib.c
	../../../components/libc/compilers/common/cstring.c
	../../../components/libc/compilers/common/ctime.c
	../../../components/libc/compilers/common/cwchar.c
	../../../components/libc/compilers/newlib/syscalls.c
	../../../libcpu/arm/common/backtrace.c
	../../../libcpu/arm/common/div0.c
	../../../libcpu/arm/common/showmem.c
	../../../libcpu/arm/cortex-m4/context_gcc.S
	../../../libcpu/arm/cortex-m4/cpuport.c
	../../../components/drivers/ipc/completion.c
	../../../components/drivers/ipc/dataqueue.c
	../../../components/drivers/ipc/pipe.c
	../../../components/drivers/ipc/ringblk_buf.c
	../../../components/drivers/ipc/ringbuffer.c
	../../../components/drivers/ipc/waitqueue.c
	../../../components/drivers/ipc/workqueue.c
	../../../components/drivers/misc/pin.c
	../../../components/drivers/serial/serial.c
	../libraries/STM32F4xx_HAL/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f401xc.s
	board/CubeMX_Config/Src/stm32f4xx_hal_msp.c
	board/board.c
	../libraries/HAL_Drivers/drv_common.c
	../libraries/HAL_Drivers/drv_gpio.c
	../libraries/HAL_Drivers/drv_usart.c
	../libraries/HAL_Drivers/drv_usbd.c
	../../../components/finsh/shell.c
	../../../components/finsh/msh_parse.c
	../../../components/finsh/msh.c
	../../../components/finsh/cmd.c
	../../../src/clock.c
	../../../src/components.c
	../../../src/device.c
	../../../src/idle.c
	../../../src/ipc.c
	../../../src/irq.c
	../../../src/kservice.c
	../../../src/mem.c
	../../../src/mempool.c
	../../../src/object.c
	../../../src/scheduler.c
	../../../src/thread.c
	../../../src/timer.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rng.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_usart.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_crc.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
	../libraries/STM32F4xx_HAL/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pccard.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hcd.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cec.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cryp.c
	../libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cryp_ex.c
	../../../components/drivers/usb/usbdevice/core/usbdevice.c
	../../../components/drivers/usb/usbdevice/class/cdc_vcom.c
	../../../components/drivers/usb/usbdevice/core/usbdevice_core.c
)

LINK_DIRECTORIES(
)

LINK_LIBRARIES(
	c
	m
)

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME}.elf ${PROJECT_SOURCES})
ADD_CUSTOM_COMMAND(
        TARGET ${CMAKE_PROJECT_NAME}.elf
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin
        COMMAND ${CMAKE_SIZE} ${CMAKE_PROJECT_NAME}.elf

        COMMAND ${CMAKE_COMMAND} -E
            make_directory "/mnt/c/Users/23517/Documents/Todelete/${CMAKE_PROJECT_NAME}"

        COMMAND ${CMAKE_COMMAND} -E
            copy_if_different
                ${CMAKE_PROJECT_NAME}.bin
                "/mnt/c/Users/23517/Documents/Todelete/${CMAKE_PROJECT_NAME}/${CMAKE_PROJECT_NAME}.bin"
)
