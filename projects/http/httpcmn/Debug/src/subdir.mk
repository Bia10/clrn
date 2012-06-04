################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Converter.cpp \
../src/Crypto.cpp \
../src/Exception.cpp \
../src/Log.cpp \
../src/MailSender.cpp \
../src/MimeType.cpp \
../src/Status.cpp 

OBJS += \
./src/Converter.o \
./src/Crypto.o \
./src/Exception.o \
./src/Log.o \
./src/MailSender.o \
./src/MimeType.o \
./src/Status.o 

CPP_DEPS += \
./src/Converter.d \
./src/Crypto.d \
./src/Exception.d \
./src/Log.d \
./src/MailSender.d \
./src/MimeType.d \
./src/Status.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../../../../ThirdParty/boost_1_47_0 -I../inc -O0 -g3 -Wall -c -fmessage-length=0 -std=gnu++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


