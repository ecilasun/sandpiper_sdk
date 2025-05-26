/* SPDX-License-Identifier: GPL-2.0-only */


#include <linux/fb.h>


#define LCD_CONN_TYPE(_x)	((_x) & 0x0f)
#define LCD_CONN_WIDTH(_x)	(((_x) >> 4) & 0x1f)

#define LCD_TYPE_MASK		0xf
#define LCD_TYPE_UNKNOWN	0
#define LCD_TYPE_MONO_STN	1
#define LCD_TYPE_MONO_DSTN	2
#define LCD_TYPE_COLOR_STN	3
#define LCD_TYPE_COLOR_DSTN	4
#define LCD_TYPE_COLOR_TFT	5
#define LCD_TYPE_SMART_PANEL	6
#define LCD_TYPE_MAX		7

#define LCD_MONO_STN_4BPP	((4  << 4) | LCD_TYPE_MONO_STN)
#define LCD_MONO_STN_8BPP	((8  << 4) | LCD_TYPE_MONO_STN)
#define LCD_MONO_DSTN_8BPP	((8  << 4) | LCD_TYPE_MONO_DSTN)
#define LCD_COLOR_STN_8BPP	((8  << 4) | LCD_TYPE_COLOR_STN)
#define LCD_COLOR_DSTN_16BPP	((16 << 4) | LCD_TYPE_COLOR_DSTN)
#define LCD_COLOR_TFT_8BPP	((8  << 4) | LCD_TYPE_COLOR_TFT)
#define LCD_COLOR_TFT_16BPP	((16 << 4) | LCD_TYPE_COLOR_TFT)
#define LCD_COLOR_TFT_18BPP	((18 << 4) | LCD_TYPE_COLOR_TFT)
#define LCD_SMART_PANEL_8BPP	((8  << 4) | LCD_TYPE_SMART_PANEL)
#define LCD_SMART_PANEL_16BPP	((16 << 4) | LCD_TYPE_SMART_PANEL)
#define LCD_SMART_PANEL_18BPP	((18 << 4) | LCD_TYPE_SMART_PANEL)

#define LCD_AC_BIAS_FREQ(x)	(((x) & 0xff) << 10)
#define LCD_BIAS_ACTIVE_HIGH	(0 << 18)
#define LCD_BIAS_ACTIVE_LOW	(1 << 18)
#define LCD_PCLK_EDGE_RISE	(0 << 19)
#define LCD_PCLK_EDGE_FALL	(1 << 19)
#define LCD_ALTERNATE_MAPPING	(1 << 20)


struct pxafb_mode_info {
	u_long		pixclock;

	u_short		xres;
	u_short		yres;

	u_char		bpp;
	u_int		cmap_greyscale:1,
			depth:8,
			transparency:1,
			unused:22;

	
	u_char		hsync_len;
	u_char		left_margin;
	u_char		right_margin;

	u_char		vsync_len;
	u_char		upper_margin;
	u_char		lower_margin;
	u_char		sync;

	
	unsigned	a0csrd_set_hld;	
	unsigned	a0cswr_set_hld;	
	unsigned	wr_pulse_width;	
	unsigned	rd_pulse_width;	
	unsigned	cmd_inh_time;	
	unsigned	op_hold_time;	
};

struct pxafb_mach_info {
	struct pxafb_mode_info *modes;
	unsigned int num_modes;

	unsigned int	lcd_conn;
	unsigned long	video_mem_size;

	u_int		fixed_modes:1,
			cmap_inverse:1,
			cmap_static:1,
			acceleration_enabled:1,
			unused:28;

	
	u_int		lccr0;
	
	u_int		lccr3;
	
	u_int		lccr4;
	void (*pxafb_backlight_power)(int);
	void (*pxafb_lcd_power)(int, struct fb_var_screeninfo *);
	void (*smart_update)(struct fb_info *);
};

void pxa_set_fb_info(struct device *, struct pxafb_mach_info *);
unsigned long pxafb_get_hsync_time(struct device *dev);


#define SMART_CMD_A0			 (0x1 << 8)
#define SMART_CMD_READ_STATUS_REG	 (0x0 << 9)
#define SMART_CMD_READ_FRAME_BUFFER	((0x0 << 9) | SMART_CMD_A0)
#define SMART_CMD_WRITE_COMMAND		 (0x1 << 9)
#define SMART_CMD_WRITE_DATA		((0x1 << 9) | SMART_CMD_A0)
#define SMART_CMD_WRITE_FRAME		((0x2 << 9) | SMART_CMD_A0)
#define SMART_CMD_WAIT_FOR_VSYNC	 (0x3 << 9)
#define SMART_CMD_NOOP			 (0x4 << 9)
#define SMART_CMD_INTERRUPT		 (0x5 << 9)

#define SMART_CMD(x)	(SMART_CMD_WRITE_COMMAND | ((x) & 0xff))
#define SMART_DAT(x)	(SMART_CMD_WRITE_DATA | ((x) & 0xff))


#define SMART_CMD_DELAY		(0x6 << 9)
#define SMART_DELAY(ms)		(SMART_CMD_DELAY | ((ms) & 0xff))

#ifdef CONFIG_FB_PXA_SMARTPANEL
extern int pxafb_smart_queue(struct fb_info *info, uint16_t *cmds, int);
extern int pxafb_smart_flush(struct fb_info *info);
#else
static inline int pxafb_smart_queue(struct fb_info *info,
				    uint16_t *cmds, int n)
{
	return 0;
}

static inline int pxafb_smart_flush(struct fb_info *info)
{
	return 0;
}
#endif
