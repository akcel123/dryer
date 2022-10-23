################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TM74HC595/delay_micros.cpp \
../TM74HC595/tm1637.cpp 

OBJS += \
./TM74HC595/delay_micros.o \
./TM74HC595/tm1637.o 

CPP_DEPS += \
./TM74HC595/delay_micros.d \
./TM74HC595/tm1637.d 


# Each subdirectory must supply rules for building sources it contributes
TM74HC595/%.o TM74HC595/%.su: ../TM74HC595/%.cpp TM74HC595/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m0 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F030x6 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -I../modbus_rtu -I../global_var -I../math_functions -I../periph -I../ds18b20 -I../TM74HC595 -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-TM74HC595

clean-TM74HC595:
	-$(RM) ./TM74HC595/delay_micros.d ./TM74HC595/delay_micros.o ./TM74HC595/delay_micros.su ./TM74HC595/tm1637.d ./TM74HC595/tm1637.o ./TM74HC595/tm1637.su

.PHONY: clean-TM74HC595

