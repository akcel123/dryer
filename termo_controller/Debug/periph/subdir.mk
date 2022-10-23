################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../periph/button.cpp \
../periph/out_pins.cpp \
../periph/periph_functions.cpp 

OBJS += \
./periph/button.o \
./periph/out_pins.o \
./periph/periph_functions.o 

CPP_DEPS += \
./periph/button.d \
./periph/out_pins.d \
./periph/periph_functions.d 


# Each subdirectory must supply rules for building sources it contributes
periph/%.o periph/%.su: ../periph/%.cpp periph/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m0 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F030x6 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -I../modbus_rtu -I../global_var -I../math_functions -I../periph -I../ds18b20 -I../TM74HC595 -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-periph

clean-periph:
	-$(RM) ./periph/button.d ./periph/button.o ./periph/button.su ./periph/out_pins.d ./periph/out_pins.o ./periph/out_pins.su ./periph/periph_functions.d ./periph/periph_functions.o ./periph/periph_functions.su

.PHONY: clean-periph

