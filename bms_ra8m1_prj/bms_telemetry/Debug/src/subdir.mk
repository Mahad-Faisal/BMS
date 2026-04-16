################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/can_task_entry.c \
../src/hal_warmstart.c \
../src/new_thread0_entry.c \
../src/sensor_task_entry.c \
../src/uart_task_entry.c 

C_DEPS += \
./src/can_task_entry.d \
./src/hal_warmstart.d \
./src/new_thread0_entry.d \
./src/sensor_task_entry.d \
./src/uart_task_entry.d 

OBJS += \
./src/can_task_entry.o \
./src/hal_warmstart.o \
./src/new_thread0_entry.o \
./src/sensor_task_entry.o \
./src/uart_task_entry.o 

SREC += \
bms_telemetry.srec 

MAP += \
bms_telemetry.map 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	$(file > $@.in,-mthumb -mfloat-abi=hard -mcpu=cortex-m85+nopacbti -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CM85 -D_RA_ORDINAL=1 -I"C:/rh/bms/bms_telemetry/ra_gen" -I"C:/rh/bms/bms_telemetry/src/bms" -I"C:/rh/bms/bms_telemetry/src/ina226" -I"." -I"C:/rh/bms/bms_telemetry/ra_cfg/fsp_cfg/bsp" -I"C:/rh/bms/bms_telemetry/ra_cfg/fsp_cfg" -I"C:/rh/bms/bms_telemetry/ra_cfg/aws" -I"C:/rh/bms/bms_telemetry/src" -I"C:/rh/bms/bms_telemetry/ra/fsp/inc" -I"C:/rh/bms/bms_telemetry/ra/fsp/inc/api" -I"C:/rh/bms/bms_telemetry/ra/fsp/inc/instances" -I"C:/rh/bms/bms_telemetry/ra/fsp/src/rm_freertos_port" -I"C:/rh/bms/bms_telemetry/ra/aws/FreeRTOS/FreeRTOS/Source/include" -I"C:/rh/bms/bms_telemetry/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -Wno-stringop-overflow -Wno-format-truncation -flax-vector-conversions --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

