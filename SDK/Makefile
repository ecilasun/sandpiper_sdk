# SDK Static Library Makefile
# Builds libsandpiper.a static library for Sandpiper platform

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

# Library name
TARGET = libsandpiper.a

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# Compiler selection based on platform
ifeq ($(UNAME), Windows)
# Windows
ARM_GCC ?= arm-none-linux-gnueabihf-g++
ARM_CC  ?= arm-none-linux-gnueabihf-gcc
ARM_AR  ?= arm-none-linux-gnueabihf-ar
else ifeq ($(UNAME), Linux)
# Linux cross-compile
ARM_GCC ?= arm-linux-gnueabihf-g++
ARM_CC  ?= arm-linux-gnueabihf-gcc
ARM_AR  ?= arm-linux-gnueabihf-ar
else
# Petalinux (native compile)
ARM_GCC ?= arm-amd-linux-gnueabi-g++
ARM_CC  ?= arm-amd-linux-gnueabi-gcc
ARM_AR  ?= arm-amd-linux-gnueabi-ar
endif

# Compiler flags
ARM_CC_OPTS  += -Ofast -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard
ARM_CC_OPTS  += -fdata-sections -ffunction-sections -fomit-frame-pointer
ARM_CC_OPTS  += -fno-math-errno -fno-trapping-math -fstrict-aliasing -pipe -DNDEBUG
ARM_CC_OPTS  += -falign-functions=16 -falign-loops=16

# Include paths
INCS = -I$(INC_DIR)

# Source files
C_SRCS := $(wildcard $(SRC_DIR)/*.c)
ASM_SRCS := $(wildcard $(SRC_DIR)/*.S)

# Object files
C_OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SRCS))
ASM_OBJS := $(patsubst $(SRC_DIR)/%.S,$(OBJ_DIR)/%.o,$(ASM_SRCS))
ALL_OBJS := $(C_OBJS) $(ASM_OBJS)

# Default target
.PHONY: all
all: $(TARGET)

# Create object directory
$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

# Compile C source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(ARM_CC) $(ARM_CC_OPTS) $(INCS) -c $< -o $@

# Assemble ARM source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.S | $(OBJ_DIR)
	$(ARM_CC) $(ARM_CC_OPTS) $(INCS) -c $< -o $@

# Build static library
$(TARGET): $(ALL_OBJS)
	$(ARM_AR) rcs $@ $^
	@echo "Built $(TARGET)"

# Clean build artifacts
.PHONY: clean
clean:
ifeq ($(UNAME), Windows)
	@-del /F /Q $(TARGET)
	@-rmdir /S /Q $(OBJ_DIR)
else
	@-rm -f $(TARGET)
	@-rm -rf $(OBJ_DIR)
endif

# Install headers and library (optional)
.PHONY: install
install: $(TARGET)
	@echo "To install, copy $(TARGET) to your lib directory"
	@echo "and $(INC_DIR)/*.h to your include directory"

# Show help
.PHONY: help
help:
	@echo "SDK Library Makefile"
	@echo "--------------------"
	@echo "Targets:"
	@echo "  all     - Build the static library (default)"
	@echo "  clean   - Remove build artifacts"
	@echo "  install - Show installation instructions"
	@echo "  help    - Show this help message"
	@echo ""
	@echo "Output: $(TARGET)"
