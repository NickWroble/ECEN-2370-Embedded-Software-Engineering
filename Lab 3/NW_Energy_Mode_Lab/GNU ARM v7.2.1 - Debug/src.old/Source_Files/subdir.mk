################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src.old/Source_Files/app.c \
../src.old/Source_Files/cmu.c \
../src.old/Source_Files/gpio.c \
../src.old/Source_Files/letimer.c 

OBJS += \
./src.old/Source_Files/app.o \
./src.old/Source_Files/cmu.o \
./src.old/Source_Files/gpio.o \
./src.old/Source_Files/letimer.o 

C_DEPS += \
./src.old/Source_Files/app.d \
./src.old/Source_Files/cmu.d \
./src.old/Source_Files/gpio.d \
./src.old/Source_Files/letimer.d 


# Each subdirectory must supply rules for building sources it contributes
src.old/Source_Files/app.o: ../src.old/Source_Files/app.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DEFM32PG12B500F1024GL125=1' '-DDEBUG_EFM=1' -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/CMSIS/Include" -I"C:\Users\nickw\Desktop\ECEN 2370 Embed. Soft. Eng\Lab 3\NW_PWM_F20\src\Source_Files" -I"C:\Users\nickw\Desktop\ECEN 2370 Embed. Soft. Eng\Lab 3\NW_PWM_F20\src\Header_Files" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//hardware/kit/common/bsp" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//hardware/kit/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//hardware/kit/SLSTK3402A_EFM32PG12/config" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src.old/Source_Files/app.d" -MT"src.old/Source_Files/app.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src.old/Source_Files/cmu.o: ../src.old/Source_Files/cmu.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DEFM32PG12B500F1024GL125=1' '-DDEBUG_EFM=1' -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/CMSIS/Include" -I"C:\Users\nickw\Desktop\ECEN 2370 Embed. Soft. Eng\Lab 3\NW_PWM_F20\src\Source_Files" -I"C:\Users\nickw\Desktop\ECEN 2370 Embed. Soft. Eng\Lab 3\NW_PWM_F20\src\Header_Files" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//hardware/kit/common/bsp" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//hardware/kit/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//hardware/kit/SLSTK3402A_EFM32PG12/config" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src.old/Source_Files/cmu.d" -MT"src.old/Source_Files/cmu.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src.old/Source_Files/gpio.o: ../src.old/Source_Files/gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DEFM32PG12B500F1024GL125=1' '-DDEBUG_EFM=1' -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/CMSIS/Include" -I"C:\Users\nickw\Desktop\ECEN 2370 Embed. Soft. Eng\Lab 3\NW_PWM_F20\src\Source_Files" -I"C:\Users\nickw\Desktop\ECEN 2370 Embed. Soft. Eng\Lab 3\NW_PWM_F20\src\Header_Files" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//hardware/kit/common/bsp" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//hardware/kit/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//hardware/kit/SLSTK3402A_EFM32PG12/config" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src.old/Source_Files/gpio.d" -MT"src.old/Source_Files/gpio.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src.old/Source_Files/letimer.o: ../src.old/Source_Files/letimer.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DEFM32PG12B500F1024GL125=1' '-DDEBUG_EFM=1' -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/CMSIS/Include" -I"C:\Users\nickw\Desktop\ECEN 2370 Embed. Soft. Eng\Lab 3\NW_PWM_F20\src\Source_Files" -I"C:\Users\nickw\Desktop\ECEN 2370 Embed. Soft. Eng\Lab 3\NW_PWM_F20\src\Header_Files" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//hardware/kit/common/bsp" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//hardware/kit/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//hardware/kit/SLSTK3402A_EFM32PG12/config" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src.old/Source_Files/letimer.d" -MT"src.old/Source_Files/letimer.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


