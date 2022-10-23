################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../modbus_rtu/my_modbus_slave.c \
../modbus_rtu/program_timer.c 

C_DEPS += \
./modbus_rtu/my_modbus_slave.d \
./modbus_rtu/program_timer.d 

OBJS += \
./modbus_rtu/my_modbus_slave.o \
./modbus_rtu/program_timer.o 


# Each subdirectory must supply rules for building sources it contributes
modbus_rtu/%.o modbus_rtu/%.su: ../modbus_rtu/%.c modbus_rtu/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F030x6 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -I../modbus_rtu -I../global_var -I../math_functions -I../periph -I../ds18b20 -I../TM74HC595 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-modbus_rtu

clean-modbus_rtu:
	-$(RM) ./modbus_rtu/my_modbus_slave.d ./modbus_rtu/my_modbus_slave.o ./modbus_rtu/my_modbus_slave.su ./modbus_rtu/program_timer.d ./modbus_rtu/program_timer.o ./modbus_rtu/program_timer.su

.PHONY: clean-modbus_rtu

