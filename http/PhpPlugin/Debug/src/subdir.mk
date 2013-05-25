################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/PhpExecutor.cpp 

OBJS += \
./src/PhpExecutor.o 

CPP_DEPS += \
./src/PhpExecutor.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../../../../ThirdParty/boost_1_47_0 -I../../../../ThirdParty/php/include/php -I../../../../ThirdParty/php/include/php/Zend -I../../../../ThirdParty/php/include/php/TSRM -I../../../../ThirdParty/php/include/php/main -I../../httpcmn/inc -I./src -O0 -g3 -Wall -c -fmessage-length=0 -std=gnu++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


