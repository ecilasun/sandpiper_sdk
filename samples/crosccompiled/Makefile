TARGET = test

default: $(TARGET)

# Directories

sysroot = /opt/petalinux/2025.1/sysroots/cortexa9t2hf-neon-amd-linux-gnueabi/
src_dir = .
corelib_dir = ../../SDK

# Rules

ARM_GCC ?= arm-amd-linux-gnueabi-g++

ARM_GCC_OPTS += -std=c++20 -Ofast -flto -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
ARM_GCC_LIBS += -lgcc -lc -lm

incs  += -I$(src_dir) -I$(corelib_dir) $(addprefix -I$(src_dir)/, $(folders))
libs += $(wildcard $(corelib_dir)/*.S) $(wildcard $(corelib_dir)/*.c)

$(TARGET):
	$(ARM_GCC) --sysroot=$(sysroot) $(ARM_GCC_OPTS) $(incs) -o $(TARGET) $(wildcard $(src_dir)/*.cpp) $(libs) $(ARM_GCC_LIBS)

.PHONY: clean
clean:
	rm $(TARGET)
