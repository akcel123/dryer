################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../global_var/config_global_var_values.c 

CPP_SRCS += \
../global_var/global_var.cpp 

C_DEPS += \
./global_var/config_global_var_values.d 

OBJS += \
./global_var/config_global_var_values.o \
./global_var/global_var.o 

CPP_DEPS += \
./global_var/global_var.d 


# Each subdirectory must supply rules for building sources it contributes
global_var/%.o global_var/%.su: ../global_var/%.c global_var/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F030x6 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -I../modbus_rtu -I../global_var -I../math_functions -I../periph -I../ds18b20 -I../TM74HC595 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
global_var/%.o global_var/%.su: ../global_var/%.cpp global_var/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m0 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F030x6 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -I../modbus_rtu -I../global_var -I../math_functions -I../periph -I../ds18b20 -I../TM74HC595 -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-global_var

clean-global_var:
	-$(RM) ./global_var/config_global_var_values.d ./global_var/config_global_var_values.o ./global_var/config_global_var_values.su ./global_var/global_var.d ./global_var/global_var.o ./global_var/global_var.su

.PHONY: clean-global_var

