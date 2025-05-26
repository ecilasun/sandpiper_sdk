/* SPDX-License-Identifier: GPL-2.0 */

#ifndef PXA2XX_UDC_H
#define PXA2XX_UDC_H

struct pxa2xx_udc_mach_info {
        int  (*udc_is_connected)(void);		
        void (*udc_command)(int cmd);
#define	PXA2XX_UDC_CMD_CONNECT		0	
#define	PXA2XX_UDC_CMD_DISCONNECT	1	

	
	bool	gpio_pullup_inverted;
	int	gpio_pullup;			
};

#ifdef CONFIG_PXA27x
extern void pxa27x_clear_otgph(void);
#else
#define pxa27x_clear_otgph()	do {} while (0)
#endif

#endif
