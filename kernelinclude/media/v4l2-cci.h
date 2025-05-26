/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _V4L2_CCI_H
#define _V4L2_CCI_H

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/types.h>

struct i2c_client;
struct regmap;


struct cci_reg_sequence {
	u32 reg;
	u64 val;
};


#define CCI_REG_ADDR_MASK		GENMASK(15, 0)
#define CCI_REG_WIDTH_SHIFT		16
#define CCI_REG_WIDTH_MASK		GENMASK(19, 16)

#define CCI_REG_WIDTH_BYTES(x)		FIELD_GET(CCI_REG_WIDTH_MASK, x)
#define CCI_REG_WIDTH(x)		(CCI_REG_WIDTH_BYTES(x) << 3)
#define CCI_REG_ADDR(x)			FIELD_GET(CCI_REG_ADDR_MASK, x)
#define CCI_REG_LE			BIT(20)

#define CCI_REG8(x)			((1 << CCI_REG_WIDTH_SHIFT) | (x))
#define CCI_REG16(x)			((2 << CCI_REG_WIDTH_SHIFT) | (x))
#define CCI_REG24(x)			((3 << CCI_REG_WIDTH_SHIFT) | (x))
#define CCI_REG32(x)			((4 << CCI_REG_WIDTH_SHIFT) | (x))
#define CCI_REG64(x)			((8 << CCI_REG_WIDTH_SHIFT) | (x))
#define CCI_REG16_LE(x)			(CCI_REG_LE | (2U << CCI_REG_WIDTH_SHIFT) | (x))
#define CCI_REG24_LE(x)			(CCI_REG_LE | (3U << CCI_REG_WIDTH_SHIFT) | (x))
#define CCI_REG32_LE(x)			(CCI_REG_LE | (4U << CCI_REG_WIDTH_SHIFT) | (x))
#define CCI_REG64_LE(x)			(CCI_REG_LE | (8U << CCI_REG_WIDTH_SHIFT) | (x))


int cci_read(struct regmap *map, u32 reg, u64 *val, int *err);


int cci_write(struct regmap *map, u32 reg, u64 val, int *err);


int cci_update_bits(struct regmap *map, u32 reg, u64 mask, u64 val, int *err);


int cci_multi_reg_write(struct regmap *map, const struct cci_reg_sequence *regs,
			unsigned int num_regs, int *err);

#if IS_ENABLED(CONFIG_V4L2_CCI_I2C)

struct regmap *devm_cci_regmap_init_i2c(struct i2c_client *client,
					int reg_addr_bits);
#endif

#endif
