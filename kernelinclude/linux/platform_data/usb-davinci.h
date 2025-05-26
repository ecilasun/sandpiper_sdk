

#ifndef __ASM_ARCH_USB_H
#define __ASM_ARCH_USB_H


struct	da8xx_ohci_root_hub {
	
	u8	potpgt;
};

void davinci_setup_usb(unsigned mA, unsigned potpgt_ms);

#endif	
