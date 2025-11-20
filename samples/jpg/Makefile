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

TARGET = jpg

default: $(TARGET)

# Directories

src_dir = .
corelib_dir = ../../SDK
jpglib_dir = ../../3rdparty/nanojpeg

# Rules

ifeq ($(UNAME), Windows)
ARM_GCC ?= arm-none-linux-gnueabihf-g++
else
ARM_GCC ?= g++
endif

ARM_GCC_OPTS += -std=c++20 -Ofast -flto -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
ARM_GCC_LIBS += -lgcc -lc -lm -L../../3rdparty/nanojpeg -lnanojpeg

incs += -I$(src_dir) -I$(corelib_dir) -I$(jpglib_dir)/ $(addprefix -I$(src_dir)/, $(folders))
libs += $(wildcard $(corelib_dir)/*.c)

$(TARGET): $(wildcard $(src_dir)/*)
	$(ARM_GCC) $(ARM_GCC_OPTS) $(incs) -o $@ $(wildcard $(src_dir)/*.cpp) $(libs) $(ARM_GCC_LIBS)

.PHONY: clean
clean:
ifeq ($(UNAME), Windows)
	del $(TARGET)
else
	rm $(TARGET)
endif
