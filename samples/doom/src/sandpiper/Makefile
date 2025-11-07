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

ifeq ($(UNAME), Windows)
ARM_GCC ?= arm-none-linux-gnueabihf-gcc
else
ARM_GCC ?= gcc
endif

ARM_GCC_OPTS += -std=gnu99 -Ofast -flto -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard

CFLAGS += \
	-DNORMALUNIX \
	$(NULL)

CLIBS = -lgcc -lm

include ../sources.mk

# Filter out d_main, we provide our own simplified one
SOURCES_doom := $(filter-out d_main.c,$(SOURCES_doom))

SOURCES_doom_arch := \
	../../../../SDK/platform.c \
	../../../../SDK/apu.c \
	../../../../SDK/vcp.c \
	../../../../SDK/vpu.c \
	mini-printf.c \
	d_main.c \
	i_main.c \
	i_net.c \
	i_sound.c \
	i_system.c \
	i_video.c \
	$(NULL)

all: doom
doom: $(addprefix ../,$(SOURCES_doom)) $(SOURCES_doom_arch)
	$(ARM_GCC) $(CFLAGS) -o $@ -I../../../../SDK -I../../src $(addprefix ../,$(SOURCES_doom)) $(SOURCES_doom_arch) $(CLIBS)

.PHONY: clean
clean:
ifeq ($(UNAME), Windows)
	del doom
else
	rm doom
endif
