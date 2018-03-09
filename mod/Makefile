# Setup

TARGET := play

default: $(TARGET)

# Directories

src_dir = .
corelib_dir = ../SDK
libxmp = ../3rdparty/libxmp

# Rules

ARM_GCC ?= g++
ARM_GCC_OPTS += -std=c++20 -Ofast -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
ARM_GCC_OPTS += -DLIBXMP_NO_DEPACKERS -DLIBXMP_CORE_PLAYER -Wno-narrowing
ARM_GCC_OPTS += -lgcc -lc -lm -lpthread -lmetal

incs  += -I$(src_dir) -I$(corelib_dir)/ -I$(libxmp)/include/ -I$(libxmp)/src/ -I$(src_dir)/.
libs += $(wildcard $(corelib_dir)/*.c) $(wildcard $(libxmp)/src/*.c) $(wildcard $(libxmp)/src/loaders/*.c)

$(foreach folder,$(folders),$(eval $(call compile_template,$(folder))))

# Build

$(TARGET): $(wildcard $(src_dir)/*)
	$(ARM_GCC) $(ARM_GCC_OPTS) $(incs) -o $@ $(wildcard $(src_dir)/*.cpp) $(libs)

.PHONY: clean
clean:
	rm $(TARGET)
