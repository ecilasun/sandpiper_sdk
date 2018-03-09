
TARGET = test.elf

default: $(TARGET)

# Directories

src_dir = .
corelib_dir = ../SDK

# Rules

ARM_OBJDUMP ?= $(ARM_PREFIX)objdump

ARM_GCC ?= gcc

ARM_GCC_OPTS += -std=gnu99 -O0 -g -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
ARM_GCC_OPTS += -lgcc -lc -lm -lmetal

incs  += -I$(src_dir) -I$(corelib_dir) $(addprefix -I$(src_dir)/, $(folders))
libs += $(wildcard $(corelib_dir)/*.S) $(wildcard $(corelib_dir)/*.c)
objs  := 

$(TARGET):
	$(ARM_GCC) $(ARM_GCC_OPTS) $(incs) -o $(TARGET) $(wildcard $(src_dir)/*.c) $(libs) 

dump: $(TARGET)
	$(ARM_OBJDUMP) $(TARGET) -x -D -S >> $(TARGET).txt

.PHONY: clean
clean:
	rm $(TARGET) $(TARGET).txt
