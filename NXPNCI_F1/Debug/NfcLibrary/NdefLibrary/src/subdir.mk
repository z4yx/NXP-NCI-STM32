################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../NfcLibrary/NdefLibrary/src/P2P_NDEF.c \
../NfcLibrary/NdefLibrary/src/RW_NDEF.c \
../NfcLibrary/NdefLibrary/src/RW_NDEF_T1T.c \
../NfcLibrary/NdefLibrary/src/RW_NDEF_T2T.c \
../NfcLibrary/NdefLibrary/src/RW_NDEF_T3T.c \
../NfcLibrary/NdefLibrary/src/RW_NDEF_T4T.c \
../NfcLibrary/NdefLibrary/src/T4T_NDEF_emu.c 

OBJS += \
./NfcLibrary/NdefLibrary/src/P2P_NDEF.o \
./NfcLibrary/NdefLibrary/src/RW_NDEF.o \
./NfcLibrary/NdefLibrary/src/RW_NDEF_T1T.o \
./NfcLibrary/NdefLibrary/src/RW_NDEF_T2T.o \
./NfcLibrary/NdefLibrary/src/RW_NDEF_T3T.o \
./NfcLibrary/NdefLibrary/src/RW_NDEF_T4T.o \
./NfcLibrary/NdefLibrary/src/T4T_NDEF_emu.o 

C_DEPS += \
./NfcLibrary/NdefLibrary/src/P2P_NDEF.d \
./NfcLibrary/NdefLibrary/src/RW_NDEF.d \
./NfcLibrary/NdefLibrary/src/RW_NDEF_T1T.d \
./NfcLibrary/NdefLibrary/src/RW_NDEF_T2T.d \
./NfcLibrary/NdefLibrary/src/RW_NDEF_T3T.d \
./NfcLibrary/NdefLibrary/src/RW_NDEF_T4T.d \
./NfcLibrary/NdefLibrary/src/T4T_NDEF_emu.d 


# Each subdirectory must supply rules for building sources it contributes
NfcLibrary/NdefLibrary/src/%.o: ../NfcLibrary/NdefLibrary/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -D__weak=__attribute__"((weak))" -DCARDEMU_RAW_EXCHANGE -DRW_RAW_EXCHANGE -D__packed=__attribute__"((__packed__))" -DUSE_HAL_DRIVER -DSTM32F103xB -DRW_SUPPORT -DP2P_SUPPORT -DCARDEMU_SUPPORT -I"/Users/zhang/Projects/hackRFID/PN7150S-STM32/NXPNCI_F1/Inc" -I"/Users/zhang/Projects/hackRFID/PN7150S-STM32/NXPNCI_F1/NfcLibrary/inc" -I"/Users/zhang/Projects/hackRFID/PN7150S-STM32/NXPNCI_F1/NfcLibrary/NdefLibrary/inc" -I"/Users/zhang/Projects/hackRFID/PN7150S-STM32/NXPNCI_F1/NfcLibrary/NxpNci/inc" -I"/Users/zhang/Projects/hackRFID/PN7150S-STM32/NXPNCI_F1/Drivers/STM32F1xx_HAL_Driver/Inc" -I"/Users/zhang/Projects/hackRFID/PN7150S-STM32/NXPNCI_F1/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"/Users/zhang/Projects/hackRFID/PN7150S-STM32/NXPNCI_F1/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"/Users/zhang/Projects/hackRFID/PN7150S-STM32/NXPNCI_F1/Drivers/CMSIS/Include" -I"/Users/zhang/Projects/hackRFID/PN7150S-STM32/NXPNCI_F1/Inc"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


