################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/authority.cpp \
../core/message.cpp \
../core/processing.cpp \
../core/representation.cpp \
../core/request.cpp \
../core/resource.cpp \
../core/response.cpp \
../core/url.cpp 

OBJS += \
./core/authority.o \
./core/message.o \
./core/processing.o \
./core/representation.o \
./core/request.o \
./core/resource.o \
./core/response.o \
./core/url.o 

CPP_DEPS += \
./core/authority.d \
./core/message.d \
./core/processing.d \
./core/representation.d \
./core/request.d \
./core/resource.d \
./core/response.d \
./core/url.d 


# Each subdirectory must supply rules for building sources it contributes
core/%.o: ../core/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DDEBUG -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


