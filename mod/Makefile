# Setup

TARGET := play

default: $(TARGET)

# Directories

src_dir = .
corelib_dir = ../SDK
libxmploaders = ../3rdparty/libxmp/loaders
libxmploadersextra = ../3rdparty/libxmp/loaders/prowizard
libxmpdepackers = ../3rdparty/libxmp/depackers
libxmp = ../3rdparty/libxmp

# Rules

ARM_GCC ?= g++
ARM_GCC_OPTS += -std=c++20 -O0 -g -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
ARM_GCC_OPTS += -Wno-narrowing
ARM_GCC_OPTS += -lgcc -lc -lm -lmetal

incs  += -I$(src_dir) -I$(corelib_dir)/ -I$(libxmp)/ -I$(libxmploaders)/ -I$(libxmploadersextra)/ -I$(libxmpdepackers)/ -I$(src_dir)/.
libs += $(wildcard $(corelib_dir)/*.c) $(wildcard $(libxmp)/*.c) $(wildcard $(libxmploaders)/*.c) $(wildcard $(libxmploadersextra)/*.c) $(wildcard $(libxmpdepackers)/*.c)

$(foreach folder,$(folders),$(eval $(call compile_template,$(folder))))

# Build

$(TARGET): $(wildcard $(src_dir)/*)
	$(ARM_GCC) $(incs) -o $@ $(wildcard $(src_dir)/*.cpp) -D LIBXMP_NO_DEPACKERS $(libs) $(ARM_GCC_OPTS)

.PHONY: clean
clean:
	rm $(TARGET)
