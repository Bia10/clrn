################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ContentParser.cpp \
../src/DataAnalyzer.cpp \
../src/Downloader.cpp \
../src/SiteParser.cpp \
../src/stdafx.cpp 

OBJS += \
./src/ContentParser.o \
./src/DataAnalyzer.o \
./src/Downloader.o \
./src/SiteParser.o \
./src/stdafx.o 

CPP_DEPS += \
./src/ContentParser.d \
./src/DataAnalyzer.d \
./src/Downloader.d \
./src/SiteParser.d \
./src/stdafx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DURDL_HEADER_ONLY -DURDL_DISABLE_SSL -I/media/All/Dev/ThirdParty/boost_1_46_0 -I/media/All/Dev/ThirdParty/urdl-0.1/include -I/media/All/Dev/ThirdParty/openssl-1.0.0d/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


