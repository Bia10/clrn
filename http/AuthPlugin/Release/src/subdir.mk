################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AuthChecker.cpp \
../src/AuthPlugin.cpp \
../src/Authorizer.cpp \
../src/Singletones.cpp 

OBJS += \
./src/AuthChecker.o \
./src/AuthPlugin.o \
./src/Authorizer.o \
./src/Singletones.o 

CPP_DEPS += \
./src/AuthChecker.d \
./src/AuthPlugin.d \
./src/Authorizer.d \
./src/Singletones.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../../HttpServer/common -I./src -I../../../../ThirdParty/boost_1_47_0 -O3 -Wall -c -fmessage-length=0 -std=gnu++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


