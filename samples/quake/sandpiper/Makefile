ARM_GCC = gcc

ARM_GCC_OPTS = -std=c99 -Ofast -flto -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
ARM_GCC_LIBS = -lgcc -lc -lm

SOURCES_quake_arch := \
	../../../SDK/platform.c \
	../../../SDK/vpu.c \
	../../../SDK/apu.c \
	display.c \
	fio.c \
	main.c \
	cd_null.c \
	in_port.c \
	snd_null.c \
	sys_port.c \
	vid_port.c \
	../winquake/chase.c \
	../winquake/cmd.c \
	../winquake/common.c \
	../winquake/console.c \
	../winquake/crc.c \
	../winquake/cvar.c \
	../winquake/draw.c \
	../winquake/host.c \
	../winquake/host_cmd.c \
	../winquake/keys.c \
	../winquake/mathlib.c \
	../winquake/menu.c \
	../winquake/model.c \
	../winquake/nonintel.c \
	../winquake/screen.c \
	../winquake/sbar.c \
	../winquake/zone.c \
	../winquake/view.c \
	../winquake/wad.c \
	../winquake/world.c \
	../winquake/cl_demo.c \
	../winquake/cl_input.c \
	../winquake/cl_main.c \
	../winquake/cl_parse.c \
	../winquake/cl_tent.c \
	../winquake/d_edge.c \
	../winquake/d_fill.c \
	../winquake/d_init.c \
	../winquake/d_modech.c \
	../winquake/d_part.c \
	../winquake/d_polyse.c \
	../winquake/d_scan.c \
	../winquake/d_sky.c \
	../winquake/d_sprite.c \
	../winquake/d_surf.c \
	../winquake/d_vars.c \
	../winquake/d_zpoint.c \
	../winquake/net_loop.c \
	../winquake/net_main.c \
	../winquake/pr_cmds.c \
	../winquake/pr_edict.c \
	../winquake/pr_exec.c \
	../winquake/r_aclip.c \
	../winquake/r_alias.c \
	../winquake/r_bsp.c \
	../winquake/r_light.c \
	../winquake/r_draw.c \
	../winquake/r_efrag.c \
	../winquake/r_edge.c \
	../winquake/r_misc.c \
	../winquake/r_main.c \
	../winquake/r_sky.c \
	../winquake/r_sprite.c \
	../winquake/r_surf.c \
	../winquake/r_part.c \
	../winquake/r_vars.c \
	../winquake/sv_main.c \
	../winquake/sv_phys.c \
	../winquake/sv_move.c \
	../winquake/sv_user.c \
	../winquake/net_none.c \
	$(NULL)

all: quake

incs = -I../../../SDK -I../include -I../winquake -I.

quake: $(addprefix ../,$(SOURCES_quake)) $(SOURCES_quake_arch)
	$(ARM_GCC) $(ARM_GCC_OPTS) -o ../$@ $(addprefix ../,$(SOURCES_quake)) $(incs) $(SOURCES_quake_arch) $(ARM_GCC_LIBS)

clean:
	rm -rf *.elf *.o *.gen.h

.PHONY: all clean
