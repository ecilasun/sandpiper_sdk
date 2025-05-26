// SPDX-License-Identifier: GPL-1.0+

#ifndef RENESAS_USB_H
#define RENESAS_USB_H
#include <linux/notifier.h>
#include <linux/platform_device.h>
#include <linux/usb/ch9.h>


enum {
	USBHS_HOST = 0,
	USBHS_GADGET,
	USBHS_MAX,
};


struct renesas_usbhs_platform_callback {

	
	int (*hardware_init)(struct platform_device *pdev);

	
	int (*hardware_exit)(struct platform_device *pdev);

	
	int (*power_ctrl)(struct platform_device *pdev,
			   void __iomem *base, int enable);

	
	int (*phy_reset)(struct platform_device *pdev);

	
	int (*get_id)(struct platform_device *pdev);

	
	int (*get_vbus)(struct platform_device *pdev);

	
	int (*set_vbus)(struct platform_device *pdev, int enable);

	
	int (*notifier)(struct notifier_block *nb, unsigned long event,
			void *data);
};



struct renesas_usbhs_driver_pipe_config {
	u8 type;	
	u16 bufsize;
	u8 bufnum;
	bool double_buf;
};
#define RENESAS_USBHS_PIPE(_type, _size, _num, _double_buf)	{	\
			.type = (_type),		\
			.bufsize = (_size),		\
			.bufnum = (_num),		\
			.double_buf = (_double_buf),	\
	}

struct renesas_usbhs_driver_param {
	
	struct renesas_usbhs_driver_pipe_config *pipe_configs;
	int pipe_size; 

	
	int buswait_bwait;

	
	int detection_delay; 

	
	int d0_tx_id;
	int d0_rx_id;
	int d1_tx_id;
	int d1_rx_id;
	int d2_tx_id;
	int d2_rx_id;
	int d3_tx_id;
	int d3_rx_id;

	
	int pio_dma_border; 

	
	u32 has_usb_dmac:1; 
	u32 runtime_pwctrl:1;
	u32 has_cnen:1;
	u32 cfifo_byte_addr:1; 
#define USBHS_USB_DMAC_XFER_SIZE	32	
	u32 multi_clks:1;
	u32 has_new_pipe_configs:1;
};


struct renesas_usbhs_platform_info {
	
	struct renesas_usbhs_platform_callback	platform_callback;

	
	struct renesas_usbhs_driver_param	driver_param;
};


#define renesas_usbhs_get_info(pdev)\
	((struct renesas_usbhs_platform_info *)(pdev)->dev.platform_data)
#endif 
