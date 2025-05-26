/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __GENERIC_IO_H
#define __GENERIC_IO_H

#include <linux/linkage.h>
#include <asm/byteorder.h>




extern unsigned int ioread8(const void __iomem *);
extern unsigned int ioread16(const void __iomem *);
extern unsigned int ioread16be(const void __iomem *);
extern unsigned int ioread32(const void __iomem *);
extern unsigned int ioread32be(const void __iomem *);
#ifdef CONFIG_64BIT
extern u64 ioread64(const void __iomem *);
extern u64 ioread64be(const void __iomem *);
#endif

#ifdef readq
#define ioread64_lo_hi ioread64_lo_hi
#define ioread64_hi_lo ioread64_hi_lo
#define ioread64be_lo_hi ioread64be_lo_hi
#define ioread64be_hi_lo ioread64be_hi_lo
extern u64 ioread64_lo_hi(const void __iomem *addr);
extern u64 ioread64_hi_lo(const void __iomem *addr);
extern u64 ioread64be_lo_hi(const void __iomem *addr);
extern u64 ioread64be_hi_lo(const void __iomem *addr);
#endif

extern void iowrite8(u8, void __iomem *);
extern void iowrite16(u16, void __iomem *);
extern void iowrite16be(u16, void __iomem *);
extern void iowrite32(u32, void __iomem *);
extern void iowrite32be(u32, void __iomem *);
#ifdef CONFIG_64BIT
extern void iowrite64(u64, void __iomem *);
extern void iowrite64be(u64, void __iomem *);
#endif

#ifdef writeq
#define iowrite64_lo_hi iowrite64_lo_hi
#define iowrite64_hi_lo iowrite64_hi_lo
#define iowrite64be_lo_hi iowrite64be_lo_hi
#define iowrite64be_hi_lo iowrite64be_hi_lo
extern void iowrite64_lo_hi(u64 val, void __iomem *addr);
extern void iowrite64_hi_lo(u64 val, void __iomem *addr);
extern void iowrite64be_lo_hi(u64 val, void __iomem *addr);
extern void iowrite64be_hi_lo(u64 val, void __iomem *addr);
#endif


extern void ioread8_rep(const void __iomem *port, void *buf, unsigned long count);
extern void ioread16_rep(const void __iomem *port, void *buf, unsigned long count);
extern void ioread32_rep(const void __iomem *port, void *buf, unsigned long count);

extern void iowrite8_rep(void __iomem *port, const void *buf, unsigned long count);
extern void iowrite16_rep(void __iomem *port, const void *buf, unsigned long count);
extern void iowrite32_rep(void __iomem *port, const void *buf, unsigned long count);

#ifdef CONFIG_HAS_IOPORT_MAP

extern void __iomem *ioport_map(unsigned long port, unsigned int nr);
extern void ioport_unmap(void __iomem *);
#endif

#ifndef ioremap_wc
#define ioremap_wc ioremap
#endif

#ifndef ioremap_wt
#define ioremap_wt ioremap
#endif

#ifndef ioremap_np

#define ioremap_np ioremap_np
static inline void __iomem *ioremap_np(phys_addr_t offset, size_t size)
{
	return NULL;
}
#endif

#include <asm-generic/pci_iomap.h>

#endif
