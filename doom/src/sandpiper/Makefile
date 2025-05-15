
CC = gcc
CFLAGS = -O1 -funsigned-char -flto -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -ffunction-sections -fdata-sections -Wl,-gc-sections -Wl,--strip-all -I.. -I../../../SDK/

CFLAGS += \
	-DNORMALUNIX \
	$(NULL)

CLIBS = -lgcc -lc -lm -lmetal

include ../sources.mk

# Filter out d_main, we provide our own simplified one
SOURCES_doom := $(filter-out d_main.c,$(SOURCES_doom))

SOURCES_doom_arch := \
	../../../SDK/platform.c \
	../../../SDK/audio.c \
	../../../SDK/video.c \
	mini-printf.c \
	d_main.c \
	i_main.c \
	i_net.c \
	i_sound.c \
	i_system.c \
	i_video.c \
	$(NULL)

all: doom.elf

doom.elf: $(addprefix ../,$(SOURCES_doom)) $(SOURCES_doom_arch)
	$(CC) $(CFLAGS) -o $@ $(addprefix ../,$(SOURCES_doom)) $(SOURCES_doom_arch) $(CLIBS)

clean:
	rm -rf *.elf *.o *.gen.h

.PHONY: all clean
