################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../regulator/pid.cpp \
../regulator/regulator.cpp 

OBJS += \
./regulator/pid.o \
./regulator/regulator.o 

CPP_DEPS += \
./regulator/pid.d \
./regulator/regulator.d 


# Each subdirectory must supply rules for building sources it contributes
regulator/%.o regulator/%.su: ../regulator/%.cpp regulator/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m0 -std=gnu++14 -g3 -DDEBUG -DSTM32F030x6 -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DHSI_VALUE=8000000 -DLSI_VALUE=40000 -DVDD_VALUE=3300 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=0 -DDATA_CACHE_ENABLE=0 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -I../global_var -I../regulator -I../out_pin_with_pwm -I../ssd1306 -I../uart_prot -I../ds18b20 -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-regulator

clean-regulator:
	-$(RM) ./regulator/pid.d ./regulator/pid.o ./regulator/pid.su ./regulator/regulator.d ./regulator/regulator.o ./regulator/regulator.su

.PHONY: clean-regulator

