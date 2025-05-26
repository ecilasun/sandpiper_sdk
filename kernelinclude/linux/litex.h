/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_LITEX_H
#define _LINUX_LITEX_H

#include <linux/io.h>

static inline void _write_litex_subregister(u32 val, void __iomem *addr)
{
	writel((u32 __force)cpu_to_le32(val), addr);
}

static inline u32 _read_litex_subregister(void __iomem *addr)
{
	return le32_to_cpu((__le32 __force)readl(addr));
}



static inline void litex_write8(void __iomem *reg, u8 val)
{
	_write_litex_subregister(val, reg);
}

static inline void litex_write16(void __iomem *reg, u16 val)
{
	_write_litex_subregister(val, reg);
}

static inline void litex_write32(void __iomem *reg, u32 val)
{
	_write_litex_subregister(val, reg);
}

static inline void litex_write64(void __iomem *reg, u64 val)
{
	_write_litex_subregister(val >> 32, reg);
	_write_litex_subregister(val, reg + 4);
}

static inline u8 litex_read8(void __iomem *reg)
{
	return _read_litex_subregister(reg);
}

static inline u16 litex_read16(void __iomem *reg)
{
	return _read_litex_subregister(reg);
}

static inline u32 litex_read32(void __iomem *reg)
{
	return _read_litex_subregister(reg);
}

static inline u64 litex_read64(void __iomem *reg)
{
	return ((u64)_read_litex_subregister(reg) << 32) |
		_read_litex_subregister(reg + 4);
}

#endif 
