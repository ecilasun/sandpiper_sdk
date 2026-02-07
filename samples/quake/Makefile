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

TARGET = quake

default: $(TARGET)

# Directories
bin_dir = bin
sdk_dir = ../../SDK

ifeq ($(UNAME), Windows)
# Windows
ARM_GCC ?= arm-none-linux-gnueabihf-gcc
else ifeq ($(UNAME), Linux)
# Linux
ARM_GCC ?= arm-linux-gnueabihf-gcc
else
# Petalinux
ARM_GCC ?= arm-amd-linux-gnueabi-gcc
endif

ARM_GCC_OPTS = -std=gnu99 -Ofast -flto -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard -Wno-stringop-overflow -Wno-address-of-packed-member -Wno-unused-result -Wno-format-security
ARM_GCC_LIBS = -lgcc -lc -lm

incs = -I$(sdk_dir)/include -Iinclude -Iwinquake -Isandpiper -I.

# Source files
SOURCES := \
	$(sdk_dir)/src/platform.c \
	$(sdk_dir)/src/vpu.c \
	$(sdk_dir)/src/vcp.c \
	$(sdk_dir)/src/apu.c \
	sandpiper/platformav.c \
	sandpiper/fio.c \
	sandpiper/main.c \
	sandpiper/cd_null.c \
	sandpiper/in_port.c \
	sandpiper/snd_null.c \
	sandpiper/sys_port.c \
	sandpiper/vid_port.c \
	winquake/chase.c \
	winquake/cmd.c \
	winquake/common.c \
	winquake/console.c \
	winquake/crc.c \
	winquake/cvar.c \
	winquake/draw.c \
	winquake/host.c \
	winquake/host_cmd.c \
	winquake/keys.c \
	winquake/mathlib.c \
	winquake/menu.c \
	winquake/model.c \
	winquake/nonintel.c \
	winquake/screen.c \
	winquake/sbar.c \
	winquake/zone.c \
	winquake/view.c \
	winquake/wad.c \
	winquake/world.c \
	winquake/cl_demo.c \
	winquake/cl_input.c \
	winquake/cl_main.c \
	winquake/cl_parse.c \
	winquake/cl_tent.c \
	winquake/d_edge.c \
	winquake/d_fill.c \
	winquake/d_init.c \
	winquake/d_modech.c \
	winquake/d_part.c \
	winquake/d_polyse.c \
	winquake/d_scan.c \
	winquake/d_sky.c \
	winquake/d_sprite.c \
	winquake/d_surf.c \
	winquake/d_vars.c \
	winquake/d_zpoint.c \
	winquake/net_loop.c \
	winquake/net_main.c \
	winquake/pr_cmds.c \
	winquake/pr_edict.c \
	winquake/pr_exec.c \
	winquake/r_aclip.c \
	winquake/r_alias.c \
	winquake/r_bsp.c \
	winquake/r_light.c \
	winquake/r_draw.c \
	winquake/r_efrag.c \
	winquake/r_edge.c \
	winquake/r_misc.c \
	winquake/r_main.c \
	winquake/r_sky.c \
	winquake/r_sprite.c \
	winquake/r_surf.c \
	winquake/r_part.c \
	winquake/r_vars.c \
	winquake/sv_main.c \
	winquake/sv_phys.c \
	winquake/sv_move.c \
	winquake/sv_user.c \
	winquake/net_none.c \
	$(NULL)

# Object files in bin directory (flattened - all .o files go to bin/)
objs := $(addprefix $(bin_dir)/,$(notdir $(SOURCES:.c=.o)))

# Create bin directory
$(bin_dir):
	@mkdir $(bin_dir)

# Build SDK library if needed
$(sdk_dir)/libsandpiper.a:
	$(MAKE) -C $(sdk_dir)

# VPATH to find source files in subdirectories
vpath %.c $(sort $(dir $(SOURCES)))

# Compile .c files
$(bin_dir)/%.o: %.c | $(bin_dir)
	$(ARM_GCC) $(ARM_GCC_OPTS) $(incs) -c $< -o $@

# Link target
$(TARGET): $(sdk_dir)/libsandpiper.a $(objs)
	$(ARM_GCC) $(ARM_GCC_OPTS) -o $@ $(objs) -L$(sdk_dir) -lsandpiper $(ARM_GCC_LIBS)

.PHONY: clean
clean:
ifeq ($(UNAME), Windows)
	@-del /F /Q $(TARGET)
	@-rmdir /S /Q $(bin_dir)
else
	@-rm -f $(TARGET)
	@-rm -rf $(bin_dir)
endif
