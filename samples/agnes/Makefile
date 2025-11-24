# Check OS type
ifeq ($(OS),Windows_NT)
	UNAME := Windows
else
	UNAME := $(shell uname -a)
	ifeq ($(findstring xilinx,$(UNAME)),)
		UNAME := Linux
	else
		UNAME := Petalinux
	endif
endif

TARGET = agnes

default: $(TARGET)

# Directories

src_dir = .
corelib_dir = ../../SDK
libxmp = ../../3rdparty/libxmp

# Rules

ifeq ($(UNAME), Windows)
# Windows
ARM_GCC ?= arm-none-linux-gnueabihf-g++
else ifeq ($(UNAME), Linux)
# Linux
ARM_GCC ?= arm-linux-gnueabihf-g++
else
# Petalinux
ARM_GCC ?= arm-amd-linux-gnueabi-g++
endif

ARM_GCC_OPTS += -std=c++20 -Ofast -flto -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
ARM_GCC_OPTS += -DLIBXMP_NO_DEPACKERS -DLIBXMP_CORE_PLAYER -Wno-narrowing
ARM_GCC_LIBS += -lgcc -lc -lm -pthread -L../../3rdparty/libxmp/ -lxmp 

incs  += -I$(src_dir) -I$(corelib_dir) -I$(libxmp)/include/ -I$(libxmp)/src/ $(addprefix -I$(src_dir)/, $(folders))
libs += $(wildcard $(corelib_dir)/*.c)

$(foreach folder,$(folders),$(eval $(call compile_template,$(folder))))

$(TARGET): $(wildcard $(src_dir)/*)
	$(ARM_GCC) $(ARM_GCC_OPTS) $(incs) -o $@ $(wildcard $(src_dir)/*.c) $(libs) $(ARM_GCC_LIBS)

.PHONY: clean
clean:
ifeq ($(UNAME), Windows)
	@-del /F /Q $(TARGET)
else
	@-rm -f $(TARGET)
endif
