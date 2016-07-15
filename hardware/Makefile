# Binaries will be generated with this name (.elf, .bin, .hex, etc)
PROJ_NAME=UART1

# object files
OBJS=  $(STARTUP) main.o  stm32f4xx_gpio.o stm32f4xx_rcc.o stm32f4xx_usart.o misc.o uart.o stm32f4xx_it.o


#############################
## fixed for every project
#############################


# name of executable
#ELF=$(notdir $(CURDIR)).elf                    
ELF=$(PROJ_NAME).elf

TEMPLATEROOT=.

# Put your stlink folder here so make burn will work.
STLINK=/home/elvis/stlink

# Library path
LIBROOT=/home/elvis/stm32_discovery_arm_gcc/STM32F4-Discovery_FW_V1.1.0


# Tool path
TOOLROOT=/usr/bin

# Tools
CC=$(TOOLROOT)/arm-none-eabi-gcc
LD=$(TOOLROOT)/arm-none-eabi-gcc
AR=$(TOOLROOT)/arm-none-eabi-ar
AS=$(TOOLROOT)/arm-none-eabi-as
OBJCOPY=$(TOOLROOT)/arm-none-eabi-objcopy

# Code Paths
DEVICE=$(LIBROOT)/Libraries/CMSIS/ST/STM32F4xx/Include/
CORE=$(LIBROOT)/Libraries/CMSIS/Include/
PERIPH=$(LIBROOT)/Libraries/STM32F4xx_StdPeriph_Driver
Utility=$(LIBROOT)/Utilities/STM32F4-Discovery 
                  
# Search path for standard files
vpath %.c $(TEMPLATEROOT)

# Search path for perpheral library
vpath %.c $(CORE)
vpath %.c $(PERIPH)/src
vpath %.c $(DEVICE)


#  Processor specific
PTYPE = STM32F40_41xxx
LDSCRIPT = $(TEMPLATEROOT)/stm32_flash.ld
STARTUP= startup_stm32f40xx.o system_stm32f4xx.o 

# compilation flags for gdb
CFLAGS  = -O1 -g
ASFLAGS = -g 

CFLAGS = -std=c11 

# Compilation Flags
FULLASSERT = -DUSE_FULL_ASSERT 
LDFLAGS += -T$(LDSCRIPT) -mthumb -mcpu=cortex-m4 #-mthumb-interwork
#LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 #-mthumb-interwork
#CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS+= -I$(TEMPLATEROOT) -I$(DEVICE) -I$(CORE) -I$(PERIPH)/inc -I$(Utility)
CFLAGS+= -D$(PTYPE) -DUSE_STDPERIPH_DRIVER $(FULLASSERT)


# Build executable 

$(ELF) : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

# compile and generate dependency info

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM $(CFLAGS) $< > $*.d

%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) startup_stm32f40xx.o $(CLEANOTHER)
	rm -f *.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin

debug: $(ELF)
	arm-none-eabi-gdb $(ELF)


# pull in dependencies

-include $(OBJS:.o=.d)



# Flash the STM32F4
burn: $(ELF)
	$(STLINK)/st-flash write $(PROJ_NAME).bin 0x8000000

