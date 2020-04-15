################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../nuestras-commons/mensajes.c \
../nuestras-commons/nuestras_commons.c \
../nuestras-commons/server_utils.c \
../nuestras-commons/utils.c 

OBJS += \
./nuestras-commons/mensajes.o \
./nuestras-commons/nuestras_commons.o \
./nuestras-commons/server_utils.o \
./nuestras-commons/utils.o 

C_DEPS += \
./nuestras-commons/mensajes.d \
./nuestras-commons/nuestras_commons.d \
./nuestras-commons/server_utils.d \
./nuestras-commons/utils.d 


# Each subdirectory must supply rules for building sources it contributes
nuestras-commons/%.o: ../nuestras-commons/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


