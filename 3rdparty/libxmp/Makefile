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

TARGET = libxmp.a

default: $(TARGET)

# Directories

src_dir = ./src
inc_dir = ./include

SRCS = $(wildcard ./src/*.c) $(wildcard ./src/loaders/*.c)
OBJS = $(SRCS:.c=.o)

# Rules

ifeq ($(UNAME), Windows)
ARM_GCC ?= arm-none-linux-gnueabihf-gcc
else
ARM_GCC ?= gcc
endif

ARM_GCC_OPTS += -c -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
ARM_GCC_OPTS += -DLIBXMP_NO_DEPACKERS -DLIBXMP_CORE_PLAYER -Wno-narrowing

incs += -I$(src_dir) -I$(inc_dir) $(addprefix -I$(src_dir)/, $(folders))

$(TARGET): $(OBJS)
	ar rcs $@ $^

%.o: %.c
	$(ARM_GCC) $(ARM_GCC_OPTS) $(incs) -c $< -o $@

.PHONY: clean
clean:
ifeq ($(UNAME), Windows)
	del $(TARGET)
else
	rm -f $(OBJS) $(TARGET)
endif
