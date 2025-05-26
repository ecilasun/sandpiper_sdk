/* SPDX-License-Identifier: GPL-2.0+ */

#ifndef __SOC_MESON_CANVAS_H
#define __SOC_MESON_CANVAS_H

#include <linux/kernel.h>

#define MESON_CANVAS_WRAP_NONE	0x00
#define MESON_CANVAS_WRAP_X	0x01
#define MESON_CANVAS_WRAP_Y	0x02

#define MESON_CANVAS_BLKMODE_LINEAR	0x00
#define MESON_CANVAS_BLKMODE_32x32	0x01
#define MESON_CANVAS_BLKMODE_64x64	0x02

#define MESON_CANVAS_ENDIAN_SWAP16	0x1
#define MESON_CANVAS_ENDIAN_SWAP32	0x3
#define MESON_CANVAS_ENDIAN_SWAP64	0x7
#define MESON_CANVAS_ENDIAN_SWAP128	0xf

struct device;
struct meson_canvas;


struct meson_canvas *meson_canvas_get(struct device *dev);


int meson_canvas_alloc(struct meson_canvas *canvas, u8 *canvas_index);


int meson_canvas_free(struct meson_canvas *canvas, u8 canvas_index);


int meson_canvas_config(struct meson_canvas *canvas, u8 canvas_index,
			u32 addr, u32 stride, u32 height,
			unsigned int wrap, unsigned int blkmode,
			unsigned int endian);

#endif
