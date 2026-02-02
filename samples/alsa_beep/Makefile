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

TARGET = alsa_beep

default: $(TARGET)

# Directories
src_dir = .
bin_dir = bin
sdk_dir = ../../SDK

# Rules
ifeq ($(UNAME), Windows)
# Windows
ARM_GCC ?= arm-none-linux-gnueabihf-g++
ARM_CC  ?= arm-none-linux-gnueabihf-gcc
else ifeq ($(UNAME), Linux)
# Linux
ARM_GCC ?= arm-linux-gnueabihf-g++
ARM_CC  ?= arm-linux-gnueabihf-gcc
else
# Petalinux
ARM_GCC ?= arm-amd-linux-gnueabi-g++
ARM_CC  ?= arm-amd-linux-gnueabi-gcc
endif

ARM_GCC_OPTS += -std=c++20 -Ofast -flto -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard
ARM_CC_OPTS  += -Ofast -flto -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard
ARM_CC_OPTS  += -D_GNU_SOURCE

# Link standard libraries + math + alsa + dynamic loading if needed
ARM_GCC_LIBS += -lgcc -lc -lm -pthread -ldl

incs += -I$(src_dir) -I$(sdk_dir)/include

# Source files
cpp_srcs := $(wildcard $(src_dir)/*.cpp)
c_srcs   := $(wildcard $(src_dir)/*.c)

# Object files in bin directory
cpp_objs := $(patsubst $(src_dir)/%.cpp,$(bin_dir)/%.o,$(cpp_srcs))
c_objs   := $(patsubst $(src_dir)/%.c,$(bin_dir)/%.o,$(c_srcs))

all_objs := $(cpp_objs) $(c_objs)

# Create bin directory
$(bin_dir):
	@mkdir $(bin_dir)

# Compile local .cpp files
$(bin_dir)/%.o: $(src_dir)/%.cpp | $(bin_dir)
	$(ARM_GCC) $(ARM_GCC_OPTS) $(incs) -c $< -o $@

# Compile local .c files
$(bin_dir)/%.o: $(src_dir)/%.c | $(bin_dir)
	$(ARM_CC) $(ARM_CC_OPTS) $(incs) -c $< -o $@

# Link target
$(TARGET): $(all_objs)
	$(ARM_CC) $(ARM_CC_OPTS) -o $@ $(all_objs) $(ARM_GCC_LIBS)

.PHONY: clean
clean:
ifeq ($(UNAME), Windows)
	@-del /F /Q $(TARGET)
	@-rmdir /S /Q $(bin_dir)
else
	@-rm -f $(TARGET)
	@-rm -rf $(bin_dir)
endif
