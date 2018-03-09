TARGET = doom

src_dir = ./src
sdk_dir = ../SDK

CC = gcc
GCC_OPTS += -std=gnu99 -Ofast -flto -g -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
GCC_OPTS += -lgcc -lc -lm -lmetal
GCC_OPTS += -DNORMALUNIX

incs  += -I$(src_dir) -I$(sdk_dir) $(addprefix -I$(src_dir)/, $(folders))
libs += $(wildcard $(sdk_dir)/*.S) $(wildcard $(sdk_dir)/*.c)

$(TARGET):
	$(CC) $(GCC_OPTS) $(incs) -o $(TARGET) $(wildcard $(src_dir)/*.c) $(wildcard $(src_dir)/device/*.c) $(libs)

.PHONY: clean
clean:
	rm $(TARGET)
