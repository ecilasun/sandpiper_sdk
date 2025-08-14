# Check OS type
ifeq ($(OS),Windows_NT)
	ifeq ($(MSYSTEM), MINGW32)
		UNAME := MSYS
	else
		UNAME := Windows
	endif
else
	UNAME := $(shell uname)
endif

TARGET := play

default: $(TARGET)

# Directories

src_dir = .
corelib_dir = ../../SDK
libxmp = ../../3rdparty/libxmp

# Rules

ifeq ($(UNAME), Windows)
ARM_GCC ?= arm-none-linux-gnueabihf-g++
else
ARM_GCC ?= g++
endif

ARM_GCC_OPTS += -Ofast -flto -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
ARM_GCC_OPTS += -DLIBXMP_NO_DEPACKERS -DLIBXMP_CORE_PLAYER -Wno-narrowing
ARM_GCC_LIBS += -lgcc -lc -lm -pthread -L../../3rdparty/libxmp/ -lxmp

incs  += -I$(src_dir) -I$(corelib_dir)/ -I$(libxmp)/include/ -I$(libxmp)/src/ -I$(src_dir)/.
libs += $(wildcard $(corelib_dir)/*.c)

$(foreach folder,$(folders),$(eval $(call compile_template,$(folder))))

# Build

$(TARGET): $(wildcard $(src_dir)/*)
	$(ARM_GCC) $(ARM_GCC_OPTS) $(incs) -o $@ $(wildcard $(src_dir)/*.cpp) $(libs) $(ARM_GCC_LIBS)

.PHONY: clean
clean:
ifeq ($(UNAME), Windows)
	del $(TARGET)
else
	rm $(TARGET)
endif
