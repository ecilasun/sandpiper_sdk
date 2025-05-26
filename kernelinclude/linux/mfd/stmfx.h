/* SPDX-License-Identifier: GPL-2.0 */


#ifndef MFD_STMFX_H
#define MFD_STMFX_H

#include <linux/regmap.h>


#define STMFX_REG_CHIP_ID		0x00 
#define STMFX_REG_FW_VERSION_MSB	0x01 
#define STMFX_REG_FW_VERSION_LSB	0x02 
#define STMFX_REG_SYS_CTRL		0x40 

#define STMFX_REG_IRQ_OUT_PIN		0x41 
#define STMFX_REG_IRQ_SRC_EN		0x42 
#define STMFX_REG_IRQ_PENDING		0x08 
#define STMFX_REG_IRQ_ACK		0x44 

#define STMFX_REG_IRQ_GPI_PENDING1	0x0C 
#define STMFX_REG_IRQ_GPI_PENDING2	0x0D 
#define STMFX_REG_IRQ_GPI_PENDING3	0x0E 
#define STMFX_REG_GPIO_STATE1		0x10 
#define STMFX_REG_GPIO_STATE2		0x11 
#define STMFX_REG_GPIO_STATE3		0x12 
#define STMFX_REG_IRQ_GPI_SRC1		0x48 
#define STMFX_REG_IRQ_GPI_SRC2		0x49 
#define STMFX_REG_IRQ_GPI_SRC3		0x4A 
#define STMFX_REG_IRQ_GPI_EVT1		0x4C 
#define STMFX_REG_IRQ_GPI_EVT2		0x4D 
#define STMFX_REG_IRQ_GPI_EVT3		0x4E 
#define STMFX_REG_IRQ_GPI_TYPE1		0x50 
#define STMFX_REG_IRQ_GPI_TYPE2		0x51 
#define STMFX_REG_IRQ_GPI_TYPE3		0x52 
#define STMFX_REG_IRQ_GPI_ACK1		0x54 
#define STMFX_REG_IRQ_GPI_ACK2		0x55 
#define STMFX_REG_IRQ_GPI_ACK3		0x56 
#define STMFX_REG_GPIO_DIR1		0x60 
#define STMFX_REG_GPIO_DIR2		0x61 
#define STMFX_REG_GPIO_DIR3		0x62 
#define STMFX_REG_GPIO_TYPE1		0x64 
#define STMFX_REG_GPIO_TYPE2		0x65 
#define STMFX_REG_GPIO_TYPE3		0x66 
#define STMFX_REG_GPIO_PUPD1		0x68 
#define STMFX_REG_GPIO_PUPD2		0x69 
#define STMFX_REG_GPIO_PUPD3		0x6A 
#define STMFX_REG_GPO_SET1		0x6C 
#define STMFX_REG_GPO_SET2		0x6D 
#define STMFX_REG_GPO_SET3		0x6E 
#define STMFX_REG_GPO_CLR1		0x70 
#define STMFX_REG_GPO_CLR2		0x71 
#define STMFX_REG_GPO_CLR3		0x72 

#define STMFX_REG_MAX			0xB0


#define STMFX_BOOT_TIME_MS 10


#define STMFX_REG_CHIP_ID_MASK		GENMASK(7, 0)


#define STMFX_REG_SYS_CTRL_GPIO_EN	BIT(0)
#define STMFX_REG_SYS_CTRL_TS_EN	BIT(1)
#define STMFX_REG_SYS_CTRL_IDD_EN	BIT(2)
#define STMFX_REG_SYS_CTRL_ALTGPIO_EN	BIT(3)
#define STMFX_REG_SYS_CTRL_SWRST	BIT(7)


#define STMFX_REG_IRQ_OUT_PIN_TYPE	BIT(0) 
#define STMFX_REG_IRQ_OUT_PIN_POL	BIT(1) 


enum stmfx_irqs {
	STMFX_REG_IRQ_SRC_EN_GPIO = 0,
	STMFX_REG_IRQ_SRC_EN_IDD,
	STMFX_REG_IRQ_SRC_EN_ERROR,
	STMFX_REG_IRQ_SRC_EN_TS_DET,
	STMFX_REG_IRQ_SRC_EN_TS_NE,
	STMFX_REG_IRQ_SRC_EN_TS_TH,
	STMFX_REG_IRQ_SRC_EN_TS_FULL,
	STMFX_REG_IRQ_SRC_EN_TS_OVF,
	STMFX_REG_IRQ_SRC_MAX,
};

enum stmfx_functions {
	STMFX_FUNC_GPIO		= BIT(0), 
	STMFX_FUNC_ALTGPIO_LOW	= BIT(1), 
	STMFX_FUNC_ALTGPIO_HIGH = BIT(2), 
	STMFX_FUNC_TS		= BIT(3),
	STMFX_FUNC_IDD		= BIT(4),
};


struct stmfx {
	struct device *dev;
	struct regmap *map;
	struct regulator *vdd;
	int irq;
	struct irq_domain *irq_domain;
	struct mutex lock; 
	u8 irq_src;
	u8 bkp_sysctrl;
	u8 bkp_irqoutpin;
};

int stmfx_function_enable(struct stmfx *stmfx, u32 func);
int stmfx_function_disable(struct stmfx *stmfx, u32 func);
#endif
