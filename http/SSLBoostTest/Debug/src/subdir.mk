################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ComplexTest.cpp \
../src/SSLBoostTest.cpp \
../src/TestCase.cpp 

OBJS += \
./src/ComplexTest.o \
./src/SSLBoostTest.o \
./src/TestCase.o 

CPP_DEPS += \
./src/ComplexTest.d \
./src/SSLBoostTest.d \
./src/TestCase.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../../../../ThirdParty/boost_1_47_0 -I/media/All/svn/dev/http/HttpServer/common -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


