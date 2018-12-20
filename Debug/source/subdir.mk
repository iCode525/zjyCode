################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/semihost_hardfault.c \
../source/sensor.c \
../source/slip.c \
../source/sreg.c 

OBJS += \
./source/semihost_hardfault.o \
./source/sensor.o \
./source/slip.o \
./source/sreg.o 

C_DEPS += \
./source/semihost_hardfault.d \
./source/sensor.d \
./source/slip.d \
./source/sreg.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DCPU_MK64FN1M0VLL12_cm4 -DCPU_MK64FN1M0VLL12 -DPRINTF_FLOAT_ENABLE=1 -DCR_PRINTF_CHAR -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DSDK_DEBUGCONSOLE=0 -I../board -I../source -I../ -I../drivers -I../CMSIS -I../startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


