################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
ECU.lss \

FLASH_IMAGE += \
ECU.hex \

EEPROM_IMAGE += \
ECU.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: ECU.elf secondary-outputs

# Tool invocations
ECU.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,ECU.map -mmcu=atmega328p -o "ECU.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

ECU.lss: ECU.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S ECU.elf  >"ECU.lss"
	@echo 'Finished building: $@'
	@echo ' '

ECU.hex: ECU.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex ECU.elf  "ECU.hex"
	@echo 'Finished building: $@'
	@echo ' '

ECU.eep: ECU.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex ECU.elf  "ECU.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: ECU.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega328p ECU.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) ECU.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
