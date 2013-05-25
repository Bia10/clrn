################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CacheManager.cpp \
../src/Connection.cpp \
../src/DefaultRequestExecutor.cpp \
../src/HttpData.cpp \
../src/HttpServer.cpp \
../src/PluginLoader.cpp \
../src/Server.cpp \
../src/ServerLogger.cpp \
../src/ServerSettings.cpp 

OBJS += \
./src/CacheManager.o \
./src/Connection.o \
./src/DefaultRequestExecutor.o \
./src/HttpData.o \
./src/HttpServer.o \
./src/PluginLoader.o \
./src/Server.o \
./src/ServerLogger.o \
./src/ServerSettings.o 

CPP_DEPS += \
./src/CacheManager.d \
./src/Connection.d \
./src/DefaultRequestExecutor.d \
./src/HttpData.d \
./src/HttpServer.d \
./src/PluginLoader.d \
./src/Server.d \
./src/ServerLogger.d \
./src/ServerSettings.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../../../../ThirdParty/boost_1_47_0 -I../src/ -I../../httpcmn/inc -O0 -g3 -Wall -c -fmessage-length=0 -std=gnu++0x -pg -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


