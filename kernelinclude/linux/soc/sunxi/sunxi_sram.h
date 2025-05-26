

#ifndef _SUNXI_SRAM_H_
#define _SUNXI_SRAM_H_

int sunxi_sram_claim(struct device *dev);
void sunxi_sram_release(struct device *dev);

#endif 
