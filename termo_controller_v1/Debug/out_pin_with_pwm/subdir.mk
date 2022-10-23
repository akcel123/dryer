################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../out_pin_with_pwm/regul_pin.cpp 

OBJS += \
./out_pin_with_pwm/regul_pin.o 

CPP_DEPS += \
./out_pin_with_pwm/regul_pin.d 


# Each subdirectory must supply rules for building sources it contributes
out_pin_with_pwm/%.o out_pin_with_pwm/%.su: ../out_pin_with_pwm/%.cpp out_pin_with_pwm/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m0 -std=gnu++14 -g3 -DDEBUG -DSTM32F030x6 -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DHSI_VALUE=8000000 -DLSI_VALUE=40000 -DVDD_VALUE=3300 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=0 -DDATA_CACHE_ENABLE=0 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -I../global_var -I../regulator -I../out_pin_with_pwm -I../ssd1306 -I../uart_prot -I../ds18b20 -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-out_pin_with_pwm

clean-out_pin_with_pwm:
	-$(RM) ./out_pin_with_pwm/regul_pin.d ./out_pin_with_pwm/regul_pin.o ./out_pin_with_pwm/regul_pin.su

.PHONY: clean-out_pin_with_pwm

