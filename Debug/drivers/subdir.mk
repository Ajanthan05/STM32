################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/dac_calib.c \
../drivers/dac_ctrl.c 

OBJS += \
./drivers/dac_calib.o \
./drivers/dac_ctrl.o 

C_DEPS += \
./drivers/dac_calib.d \
./drivers/dac_ctrl.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o drivers/%.su drivers/%.cyclo: ../drivers/%.c drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L072xx -Dboard -c -I../Core/Inc -I/home/ajanthan/STM32Cube/Repository/STM32Cube_FW_L0_V1.12.3/Drivers/STM32L0xx_HAL_Driver/Inc -I/home/ajanthan/STM32Cube/Repository/STM32Cube_FW_L0_V1.12.3/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I/home/ajanthan/STM32Cube/Repository/STM32Cube_FW_L0_V1.12.3/Drivers/CMSIS/Device/ST/STM32L0xx/Include -I/home/ajanthan/STM32Cube/Repository/STM32Cube_FW_L0_V1.12.3/Drivers/CMSIS/Include -I"/home/ajanthan/STM32CubeIDE/workspace_1.19.0/firmware1/board" -I../drivers -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-drivers

clean-drivers:
	-$(RM) ./drivers/dac_calib.cyclo ./drivers/dac_calib.d ./drivers/dac_calib.o ./drivers/dac_calib.su ./drivers/dac_ctrl.cyclo ./drivers/dac_ctrl.d ./drivers/dac_ctrl.o ./drivers/dac_ctrl.su

.PHONY: clean-drivers

