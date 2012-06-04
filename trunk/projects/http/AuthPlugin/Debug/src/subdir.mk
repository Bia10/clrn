################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AuthChecker.cpp \
../src/AuthPlugin.cpp \
../src/Authorizer.cpp \
../src/Registrator.cpp \
../src/Singletones.cpp \
../src/UserInfo.cpp 

OBJS += \
./src/AuthChecker.o \
./src/AuthPlugin.o \
./src/Authorizer.o \
./src/Registrator.o \
./src/Singletones.o \
./src/UserInfo.o 

CPP_DEPS += \
./src/AuthChecker.d \
./src/AuthPlugin.d \
./src/Authorizer.d \
./src/Registrator.d \
./src/Singletones.d \
./src/UserInfo.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../../httpcmn/inc -I./src -I../../../../ThirdParty/boost_1_47_0 -O0 -g3 -Wall -c -fmessage-length=0 -std=gnu++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


