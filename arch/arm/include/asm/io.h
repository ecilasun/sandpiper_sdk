/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASM_ARM_IO_H
#define __ASM_ARM_IO_H

#ifdef __KERNEL__

#include <linux/string.h>
#include <linux/types.h>
#include <asm/byteorder.h>
#include <asm/page.h>
#include <asm-generic/pci_iomap.h>


#define isa_virt_to_bus virt_to_phys
#define isa_bus_to_virt phys_to_virt


extern void atomic_io_modify(void __iomem *reg, u32 mask, u32 set);
extern void atomic_io_modify_relaxed(void __iomem *reg, u32 mask, u32 set);


void __raw_writesb(volatile void __iomem *addr, const void *data, int bytelen);
void __raw_writesw(volatile void __iomem *addr, const void *data, int wordlen);
void __raw_writesl(volatile void __iomem *addr, const void *data, int longlen);

void __raw_readsb(const volatile void __iomem *addr, void *data, int bytelen);
void __raw_readsw(const volatile void __iomem *addr, void *data, int wordlen);
void __raw_readsl(const volatile void __iomem *addr, void *data, int longlen);

#if __LINUX_ARM_ARCH__ < 6

#define __raw_readw(a)         (__chk_io_ptr(a), *(volatile unsigned short __force *)(a))
#define __raw_writew(v,a)      ((void)(__chk_io_ptr(a), *(volatile unsigned short __force *)(a) = (v)))
#else

#define __raw_writew __raw_writew
static inline void __raw_writew(u16 val, volatile void __iomem *addr)
{
	asm volatile("strh %1, %0"
		     : : "Q" (*(volatile u16 __force *)addr), "r" (val));
}

#define __raw_readw __raw_readw
static inline u16 __raw_readw(const volatile void __iomem *addr)
{
	u16 val;
	asm volatile("ldrh %0, %1"
		     : "=r" (val)
		     : "Q" (*(volatile u16 __force *)addr));
	return val;
}
#endif

#define __raw_writeb __raw_writeb
static inline void __raw_writeb(u8 val, volatile void __iomem *addr)
{
	asm volatile("strb %1, %0"
		     : : "Qo" (*(volatile u8 __force *)addr), "r" (val));
}

#define __raw_writel __raw_writel
static inline void __raw_writel(u32 val, volatile void __iomem *addr)
{
	asm volatile("str %1, %0"
		     : : "Qo" (*(volatile u32 __force *)addr), "r" (val));
}

#define __raw_readb __raw_readb
static inline u8 __raw_readb(const volatile void __iomem *addr)
{
	u8 val;
	asm volatile("ldrb %0, %1"
		     : "=r" (val)
		     : "Qo" (*(volatile u8 __force *)addr));
	return val;
}

#define __raw_readl __raw_readl
static inline u32 __raw_readl(const volatile void __iomem *addr)
{
	u32 val;
	asm volatile("ldr %0, %1"
		     : "=r" (val)
		     : "Qo" (*(volatile u32 __force *)addr));
	return val;
}


#define MT_DEVICE		0
#define MT_DEVICE_NONSHARED	1
#define MT_DEVICE_CACHED	2
#define MT_DEVICE_WC		3



extern void __iomem *__arm_ioremap_caller(phys_addr_t, size_t, unsigned int,
	void *);
extern void __iomem *__arm_ioremap_pfn(unsigned long, unsigned long, size_t, unsigned int);
extern void __iomem *__arm_ioremap_exec(phys_addr_t, size_t, bool cached);
void __arm_iomem_set_ro(void __iomem *ptr, size_t size);

extern void __iomem * (*arch_ioremap_caller)(phys_addr_t, size_t,
	unsigned int, void *);


extern void __readwrite_bug(const char *fn);


static inline void __iomem *__typesafe_io(unsigned long addr)
{
	return (void __iomem *)addr;
}

#define IOMEM(x)	((void __force __iomem *)(x))


#ifdef CONFIG_ARM_DMA_MEM_BUFFERABLE
#include <asm/barrier.h>
#define __iormb()		rmb()
#define __iowmb()		wmb()
#else
#define __iormb()		do { } while (0)
#define __iowmb()		do { } while (0)
#endif


#define PCI_IO_VIRT_BASE	0xfee00000
#define PCI_IOBASE		((void __iomem *)PCI_IO_VIRT_BASE)

#if defined(CONFIG_PCI) || IS_ENABLED(CONFIG_PCMCIA)
void pci_ioremap_set_mem_type(int mem_type);
#else
static inline void pci_ioremap_set_mem_type(int mem_type) {}
#endif

struct resource;

#define pci_remap_iospace pci_remap_iospace
int pci_remap_iospace(const struct resource *res, phys_addr_t phys_addr);


#define pci_remap_cfgspace pci_remap_cfgspace
void __iomem *pci_remap_cfgspace(resource_size_t res_cookie, size_t size);

#ifdef CONFIG_NEED_MACH_IO_H
#include <mach/io.h>
#else
#if IS_ENABLED(CONFIG_PCMCIA) || defined(CONFIG_PCI)
#define IO_SPACE_LIMIT	((resource_size_t)0xfffff)
#else
#define IO_SPACE_LIMIT ((resource_size_t)0)
#endif
#define __io(a)		__typesafe_io(PCI_IO_VIRT_BASE + ((a) & IO_SPACE_LIMIT))
#endif


#ifdef __io
#define outb(v,p)	({ __iowmb(); __raw_writeb(v,__io(p)); })
#define outw(v,p)	({ __iowmb(); __raw_writew((__force __u16) \
					cpu_to_le16(v),__io(p)); })
#define outl(v,p)	({ __iowmb(); __raw_writel((__force __u32) \
					cpu_to_le32(v),__io(p)); })

#define inb(p)	({ __u8 __v = __raw_readb(__io(p)); __iormb(); __v; })
#define inw(p)	({ __u16 __v = le16_to_cpu((__force __le16) \
			__raw_readw(__io(p))); __iormb(); __v; })
#define inl(p)	({ __u32 __v = le32_to_cpu((__force __le32) \
			__raw_readl(__io(p))); __iormb(); __v; })

#define outsb(p,d,l)		__raw_writesb(__io(p),d,l)
#define outsw(p,d,l)		__raw_writesw(__io(p),d,l)
#define outsl(p,d,l)		__raw_writesl(__io(p),d,l)

#define insb(p,d,l)		__raw_readsb(__io(p),d,l)
#define insw(p,d,l)		__raw_readsw(__io(p),d,l)
#define insl(p,d,l)		__raw_readsl(__io(p),d,l)
#endif


extern void _memcpy_fromio(void *, const volatile void __iomem *, size_t);
extern void _memcpy_toio(volatile void __iomem *, const void *, size_t);
extern void _memset_io(volatile void __iomem *, int, size_t);


#ifndef readl
#define readb_relaxed(c) ({ u8  __r = __raw_readb(c); __r; })
#define readw_relaxed(c) ({ u16 __r = le16_to_cpu((__force __le16) \
					__raw_readw(c)); __r; })
#define readl_relaxed(c) ({ u32 __r = le32_to_cpu((__force __le32) \
					__raw_readl(c)); __r; })

#define writeb_relaxed(v,c)	__raw_writeb(v,c)
#define writew_relaxed(v,c)	__raw_writew((__force u16) cpu_to_le16(v),c)
#define writel_relaxed(v,c)	__raw_writel((__force u32) cpu_to_le32(v),c)

#define readb(c)		({ u8  __v = readb_relaxed(c); __iormb(); __v; })
#define readw(c)		({ u16 __v = readw_relaxed(c); __iormb(); __v; })
#define readl(c)		({ u32 __v = readl_relaxed(c); __iormb(); __v; })

#define writeb(v,c)		({ __iowmb(); writeb_relaxed(v,c); })
#define writew(v,c)		({ __iowmb(); writew_relaxed(v,c); })
#define writel(v,c)		({ __iowmb(); writel_relaxed(v,c); })

#define readsb(p,d,l)		__raw_readsb(p,d,l)
#define readsw(p,d,l)		__raw_readsw(p,d,l)
#define readsl(p,d,l)		__raw_readsl(p,d,l)

#define writesb(p,d,l)		__raw_writesb(p,d,l)
#define writesw(p,d,l)		__raw_writesw(p,d,l)
#define writesl(p,d,l)		__raw_writesl(p,d,l)

#ifndef __ARMBE__
static inline void memset_io(volatile void __iomem *dst, unsigned c,
	size_t count)
{
	extern void mmioset(void *, unsigned int, size_t);
	mmioset((void __force *)dst, c, count);
}
#define memset_io(dst,c,count) memset_io(dst,c,count)

static inline void memcpy_fromio(void *to, const volatile void __iomem *from,
	size_t count)
{
	extern void mmiocpy(void *, const void *, size_t);
	mmiocpy(to, (const void __force *)from, count);
}
#define memcpy_fromio(to,from,count) memcpy_fromio(to,from,count)

static inline void memcpy_toio(volatile void __iomem *to, const void *from,
	size_t count)
{
	extern void mmiocpy(void *, const void *, size_t);
	mmiocpy((void __force *)to, from, count);
}
#define memcpy_toio(to,from,count) memcpy_toio(to,from,count)

#else
#define memset_io(c,v,l)	_memset_io(c,(v),(l))
#define memcpy_fromio(a,c,l)	_memcpy_fromio((a),c,(l))
#define memcpy_toio(c,a,l)	_memcpy_toio(c,(a),(l))
#endif

#endif	


void __iomem *ioremap(resource_size_t res_cookie, size_t size);
#define ioremap ioremap


void __iomem *ioremap_cache(resource_size_t res_cookie, size_t size);
#define ioremap_cache ioremap_cache

void __iomem *ioremap_wc(resource_size_t res_cookie, size_t size);
#define ioremap_wc ioremap_wc
#define ioremap_wt ioremap_wc

void iounmap(volatile void __iomem *io_addr);
#define iounmap iounmap

void *arch_memremap_wb(phys_addr_t phys_addr, size_t size);
#define arch_memremap_wb arch_memremap_wb


#define ioread16be(p)		({ __u16 __v = be16_to_cpu((__force __be16)__raw_readw(p)); __iormb(); __v; })
#define ioread32be(p)		({ __u32 __v = be32_to_cpu((__force __be32)__raw_readl(p)); __iormb(); __v; })

#define iowrite16be(v,p)	({ __iowmb(); __raw_writew((__force __u16)cpu_to_be16(v), p); })
#define iowrite32be(v,p)	({ __iowmb(); __raw_writel((__force __u32)cpu_to_be32(v), p); })

#ifndef ioport_map
#define ioport_map ioport_map
extern void __iomem *ioport_map(unsigned long port, unsigned int nr);
#endif
#ifndef ioport_unmap
#define ioport_unmap ioport_unmap
extern void ioport_unmap(void __iomem *addr);
#endif

struct pci_dev;

#define pci_iounmap pci_iounmap
extern void pci_iounmap(struct pci_dev *dev, void __iomem *addr);


#define xlate_dev_mem_ptr(p)	__va(p)

#include <asm-generic/io.h>

#ifdef CONFIG_MMU
#define ARCH_HAS_VALID_PHYS_ADDR_RANGE
extern int valid_phys_addr_range(phys_addr_t addr, size_t size);
extern int valid_mmap_phys_addr_range(unsigned long pfn, size_t size);
extern bool arch_memremap_can_ram_remap(resource_size_t offset, size_t size,
					unsigned long flags);
#define arch_memremap_can_ram_remap arch_memremap_can_ram_remap
#endif


extern void register_isa_ports(unsigned int mmio, unsigned int io,
			       unsigned int io_shift);

#endif	
#endif	
