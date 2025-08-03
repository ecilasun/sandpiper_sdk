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

TARGET = tinyrt

default: $(TARGET)

# Directories

src_dir = .
corelib_dir = ../../SDK

# Rules

ifeq ($(UNAME), Windows)
ARM_GCC ?= arm-none-linux-gnueabihf-g++
else
ARM_GCC ?= g++
endif

ARM_GCC_OPTS += -std=c++20 -Ofast -flto -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
ARM_GCC_LIBS += -lgcc -lc -lm

incs  += -I$(src_dir) -I$(corelib_dir) $(addprefix -I$(src_dir)/, $(folders))
libs += $(wildcard $(corelib_dir)/*.S) $(wildcard $(corelib_dir)/*.c)

$(TARGET):
	$(ARM_GCC) $(ARM_GCC_OPTS) $(incs) -o $(TARGET) $(wildcard $(src_dir)/*.cpp) $(libs) $(ARM_GCC_LIBS)

.PHONY: clean
clean:
ifeq ($(UNAME), Windows)
	del $(TARGET)
else
	rm $(TARGET)
endif
