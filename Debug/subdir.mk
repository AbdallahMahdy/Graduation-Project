################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DIO.c \
../I2C.c \
../USART.c \
../eeprom.c \
../gps.c \
../gsm.c \
../lcd.c \
../main.c \
../mfrc522.c \
../spi.c \
../wifi.c 

OBJS += \
./DIO.o \
./I2C.o \
./USART.o \
./eeprom.o \
./gps.o \
./gsm.o \
./lcd.o \
./main.o \
./mfrc522.o \
./spi.o \
./wifi.o 

C_DEPS += \
./DIO.d \
./I2C.d \
./USART.d \
./eeprom.d \
./gps.d \
./gsm.d \
./lcd.d \
./main.d \
./mfrc522.d \
./spi.d \
./wifi.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


