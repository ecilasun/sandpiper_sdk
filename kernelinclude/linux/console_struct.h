/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_CONSOLE_STRUCT_H
#define _LINUX_CONSOLE_STRUCT_H

#include <linux/wait.h>
#include <linux/vt.h>
#include <linux/workqueue.h>

struct uni_pagedict;

#define NPAR 16
#define VC_TABSTOPS_COUNT	256U

enum vc_intensity {
	VCI_HALF_BRIGHT,
	VCI_NORMAL,
	VCI_BOLD,
	VCI_MASK = 0x3,
};


struct vc_state {
	unsigned int	x, y;

	unsigned char	color;

	unsigned char	Gx_charset[2];
	unsigned int	charset		: 1;

	
	enum vc_intensity intensity;
	bool		italic;
	bool		underline;
	bool		blink;
	bool		reverse;
};


struct vc_data {
	struct tty_port port;			

	struct vc_state state, saved_state;

	unsigned short	vc_num;			
	unsigned int	vc_cols;		
	unsigned int	vc_rows;
	unsigned int	vc_size_row;		
	unsigned int	vc_scan_lines;		
	unsigned int	vc_cell_height;		
	unsigned long	vc_origin;		
	unsigned long	vc_scr_end;		
	unsigned long	vc_visible_origin;	
	unsigned int	vc_top, vc_bottom;	
	const struct consw *vc_sw;
	unsigned short	*vc_screenbuf;		
	unsigned int	vc_screenbuf_size;
	unsigned char	vc_mode;		
	
	unsigned char	vc_attr;		
	unsigned char	vc_def_color;		
	unsigned char	vc_ulcolor;		
	unsigned char   vc_itcolor;
	unsigned char	vc_halfcolor;		
	
	unsigned int	vc_cursor_type;
	unsigned short	vc_complement_mask;	
	unsigned short	vc_s_complement_mask;	
	unsigned long	vc_pos;			
		
	unsigned short	vc_hi_font_mask;	
	struct console_font vc_font;		
	unsigned short	vc_video_erase_char;	
	
	unsigned int	vc_state;		
	unsigned int	vc_npar,vc_par[NPAR];	
	
	struct vt_mode	vt_mode;
	struct pid 	*vt_pid;
	int		vt_newvt;
	wait_queue_head_t paste_wait;
	
	unsigned int	vc_disp_ctrl	: 1;	
	unsigned int	vc_toggle_meta	: 1;	
	unsigned int	vc_decscnm	: 1;	
	unsigned int	vc_decom	: 1;	
	unsigned int	vc_decawm	: 1;	
	unsigned int	vc_deccm	: 1;	
	unsigned int	vc_decim	: 1;	
	
	unsigned int	vc_priv		: 3;
	unsigned int	vc_need_wrap	: 1;
	unsigned int	vc_can_do_color	: 1;
	unsigned int	vc_report_mouse : 2;
	unsigned char	vc_utf		: 1;	
	unsigned char	vc_utf_count;
		 int	vc_utf_char;
	DECLARE_BITMAP(vc_tab_stop, VC_TABSTOPS_COUNT);	
	unsigned char   vc_palette[16*3];       
	unsigned short * vc_translate;
	unsigned int    vc_resize_user;         
	unsigned int	vc_bell_pitch;		
	unsigned int	vc_bell_duration;	
	unsigned short	vc_cur_blink_ms;	
	struct vc_data **vc_display_fg;		
	struct uni_pagedict *uni_pagedict;
	struct uni_pagedict **uni_pagedict_loc; 
	u32 **vc_uni_lines;			
	
};

struct vc {
	struct vc_data *d;
	struct work_struct SAK_work;

	
};

extern struct vc vc_cons [MAX_NR_CONSOLES];
extern void vc_SAK(struct work_struct *work);

#define CUR_MAKE(size, change, set)	((size) | ((change) << 8) |	\
		((set) << 16))
#define CUR_SIZE(c)		 ((c) & 0x00000f)
# define CUR_DEF			       0
# define CUR_NONE			       1
# define CUR_UNDERLINE			       2
# define CUR_LOWER_THIRD		       3
# define CUR_LOWER_HALF			       4
# define CUR_TWO_THIRDS			       5
# define CUR_BLOCK			       6
#define CUR_SW				0x000010
#define CUR_ALWAYS_BG			0x000020
#define CUR_INVERT_FG_BG		0x000040
#define CUR_FG				0x000700
#define CUR_BG				0x007000
#define CUR_CHANGE(c)		 ((c) & 0x00ff00)
#define CUR_SET(c)		(((c) & 0xff0000) >> 8)

bool con_is_visible(const struct vc_data *vc);

#endif 
