################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../NfcLibrary/NxpNci/src/NxpNci.c 

OBJS += \
./NfcLibrary/NxpNci/src/NxpNci.o 

C_DEPS += \
./NfcLibrary/NxpNci/src/NxpNci.d 


# Each subdirectory must supply rules for building sources it contributes
NfcLibrary/NxpNci/src/%.o: ../NfcLibrary/NxpNci/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -D__weak=__attribute__"((weak))" -DCARDEMU_RAW_EXCHANGE -DRW_RAW_EXCHANGE -D__packed=__attribute__"((__packed__))" -DUSE_HAL_DRIVER -DSTM32F103xB -DnoRW_SUPPORT -DnoP2P_SUPPORT -DCARDEMU_SUPPORT -I"/Users/zhang/Projects/NXP-NCI-STM32/NXPNCI_F1/Inc" -I"/Users/zhang/Projects/NXP-NCI-STM32/NXPNCI_F1/NfcLibrary/inc" -I"/Users/zhang/Projects/NXP-NCI-STM32/NXPNCI_F1/NfcLibrary/NdefLibrary/inc" -I"/Users/zhang/Projects/NXP-NCI-STM32/NXPNCI_F1/NfcLibrary/NxpNci/inc" -I"/Users/zhang/Projects/NXP-NCI-STM32/NXPNCI_F1/Drivers/STM32F1xx_HAL_Driver/Inc" -I"/Users/zhang/Projects/NXP-NCI-STM32/NXPNCI_F1/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"/Users/zhang/Projects/NXP-NCI-STM32/NXPNCI_F1/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"/Users/zhang/Projects/NXP-NCI-STM32/NXPNCI_F1/Drivers/CMSIS/Include" -I"/Users/zhang/Projects/NXP-NCI-STM32/NXPNCI_F1/Inc"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


