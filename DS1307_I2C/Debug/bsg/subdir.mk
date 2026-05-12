################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bsg/ds1307.c 

OBJS += \
./bsg/ds1307.o 

C_DEPS += \
./bsg/ds1307.d 


# Each subdirectory must supply rules for building sources it contributes
bsg/%.o bsg/%.su bsg/%.cyclo: ../bsg/%.c bsg/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I"C:/Users/kaan_/OneDrive/Masaüstü/STM32/Workspaces/STM32 Sensor Example/DS1307_I2C/bsg" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-bsg

clean-bsg:
	-$(RM) ./bsg/ds1307.cyclo ./bsg/ds1307.d ./bsg/ds1307.o ./bsg/ds1307.su

.PHONY: clean-bsg

