/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_USB_MUSB_H
#define __LINUX_USB_MUSB_H


enum musb_mode {
	MUSB_UNDEFINED = 0,
	MUSB_HOST,		
	MUSB_PERIPHERAL,	
	MUSB_OTG		
};

struct clk;

enum musb_fifo_style {
	FIFO_RXTX,
	FIFO_TX,
	FIFO_RX
} __attribute__ ((packed));

enum musb_buf_mode {
	BUF_SINGLE,
	BUF_DOUBLE
} __attribute__ ((packed));

struct musb_fifo_cfg {
	u8			hw_ep_num;
	enum musb_fifo_style	style;
	enum musb_buf_mode	mode;
	u16			maxpacket;
};

#define MUSB_EP_FIFO(ep, st, m, pkt)		\
{						\
	.hw_ep_num	= ep,			\
	.style		= st,			\
	.mode		= m,			\
	.maxpacket	= pkt,			\
}

#define MUSB_EP_FIFO_SINGLE(ep, st, pkt)	\
	MUSB_EP_FIFO(ep, st, BUF_SINGLE, pkt)

#define MUSB_EP_FIFO_DOUBLE(ep, st, pkt)	\
	MUSB_EP_FIFO(ep, st, BUF_DOUBLE, pkt)

struct musb_hdrc_eps_bits {
	const char	name[16];
	u8		bits;
};

struct musb_hdrc_config {
	struct musb_fifo_cfg	*fifo_cfg;	
	unsigned		fifo_cfg_size;	

	
	unsigned	multipoint:1;	
	unsigned	dyn_fifo:1 __deprecated; 

	
	unsigned	host_port_deassert_reset_at_resume:1;

	u8		num_eps;	
	u8		ram_bits;	

	u32		maximum_speed;
};

struct musb_hdrc_platform_data {
	
	u8		mode;

	
	const char	*clock;

	
	int		(*set_vbus)(struct device *dev, int is_on);

	
	u8		power;

	
	u8		min_power;

	
	u8		potpgt;

	
	unsigned	extvbus:1;

	
	const struct musb_hdrc_config *config;

	
	void		*board_data;

	
	const void	*platform_ops;
};

enum musb_vbus_id_status {
	MUSB_UNKNOWN = 0,
	MUSB_ID_GROUND,
	MUSB_ID_FLOAT,
	MUSB_VBUS_VALID,
	MUSB_VBUS_OFF,
};

#if IS_ENABLED(CONFIG_USB_MUSB_HDRC)
int musb_mailbox(enum musb_vbus_id_status status);
#else
static inline int musb_mailbox(enum musb_vbus_id_status status)
{
	return 0;
}
#endif



#define	TUSB6010_OSCCLK_60	16667	
#define	TUSB6010_REFCLK_24	41667	
#define	TUSB6010_REFCLK_19	52083	

#endif 
