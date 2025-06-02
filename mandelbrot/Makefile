
TARGET = mandelbrot

default: $(TARGET)

# Directories

src_dir = .
corelib_dir = ../SDK

# Rules

ARM_GCC ?= g++

ARM_GCC_OPTS += -std=c++20 -Ofast -flto -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
ARM_GCC_OPTS += -lgcc -lc -lm

incs  += -I$(src_dir) -I$(corelib_dir) $(addprefix -I$(src_dir)/, $(folders))
libs += $(wildcard $(corelib_dir)/*.S) $(wildcard $(corelib_dir)/*.c)

$(TARGET):
	$(ARM_GCC) $(ARM_GCC_OPTS) $(incs) -o $(TARGET) $(wildcard $(src_dir)/*.cpp) $(libs) 

.PHONY: clean
clean:
	rm $(TARGET)
