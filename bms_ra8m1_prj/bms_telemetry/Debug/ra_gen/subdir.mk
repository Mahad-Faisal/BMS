################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra_gen/common_data.c \
../ra_gen/hal_data.c \
../ra_gen/main.c \
../ra_gen/new_thread0.c \
../ra_gen/pin_data.c \
../ra_gen/vector_data.c 

C_DEPS += \
./ra_gen/common_data.d \
./ra_gen/hal_data.d \
./ra_gen/main.d \
./ra_gen/new_thread0.d \
./ra_gen/pin_data.d \
./ra_gen/vector_data.d 

OBJS += \
./ra_gen/common_data.o \
./ra_gen/hal_data.o \
./ra_gen/main.o \
./ra_gen/new_thread0.o \
./ra_gen/pin_data.o \
./ra_gen/vector_data.o 

SREC += \
bms_telemetry.srec 

MAP += \
bms_telemetry.map 


# Each subdirectory must supply rules for building sources it contributes
ra_gen/%.o: ../ra_gen/%.c
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CM85 -D_RA_ORDINAL=1 -I"C:/rh/bms/bms_telemetry/ra_gen" -I"C:/rh/bms/bms_telemetry/src/bms" -I"C:/rh/bms/bms_telemetry/src/ina226" -I"." -I"C:/rh/bms/bms_telemetry/ra_cfg/fsp_cfg/bsp" -I"C:/rh/bms/bms_telemetry/ra_cfg/fsp_cfg" -I"C:/rh/bms/bms_telemetry/ra_cfg/aws" -I"C:/rh/bms/bms_telemetry/src" -I"C:/rh/bms/bms_telemetry/ra/fsp/inc" -I"C:/rh/bms/bms_telemetry/ra/fsp/inc/api" -I"C:/rh/bms/bms_telemetry/ra/fsp/inc/instances" -I"C:/rh/bms/bms_telemetry/ra/fsp/src/rm_freertos_port" -I"C:/rh/bms/bms_telemetry/ra/aws/FreeRTOS/FreeRTOS/Source/include" -I"C:/rh/bms/bms_telemetry/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

