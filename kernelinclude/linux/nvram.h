/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_NVRAM_H
#define _LINUX_NVRAM_H

#include <linux/errno.h>
#include <uapi/linux/nvram.h>

#ifdef CONFIG_PPC
#include <asm/machdep.h>
#endif



struct nvram_ops {
	ssize_t         (*get_size)(void);
	unsigned char   (*read_byte)(int);
	void            (*write_byte)(unsigned char, int);
	ssize_t         (*read)(char *, size_t, loff_t *);
	ssize_t         (*write)(char *, size_t, loff_t *);
#if defined(CONFIG_X86) || defined(CONFIG_M68K)
	long            (*initialize)(void);
	long            (*set_checksum)(void);
#endif
};

extern const struct nvram_ops arch_nvram_ops;

static inline ssize_t nvram_get_size(void)
{
#ifdef CONFIG_PPC
	if (ppc_md.nvram_size)
		return ppc_md.nvram_size();
#else
	if (arch_nvram_ops.get_size)
		return arch_nvram_ops.get_size();
#endif
	return -ENODEV;
}

static inline unsigned char nvram_read_byte(int addr)
{
#ifdef CONFIG_PPC
	if (ppc_md.nvram_read_val)
		return ppc_md.nvram_read_val(addr);
#else
	if (arch_nvram_ops.read_byte)
		return arch_nvram_ops.read_byte(addr);
#endif
	return 0xFF;
}

static inline void nvram_write_byte(unsigned char val, int addr)
{
#ifdef CONFIG_PPC
	if (ppc_md.nvram_write_val)
		ppc_md.nvram_write_val(addr, val);
#else
	if (arch_nvram_ops.write_byte)
		arch_nvram_ops.write_byte(val, addr);
#endif
}

static inline ssize_t nvram_read_bytes(char *buf, size_t count, loff_t *ppos)
{
	ssize_t nvram_size = nvram_get_size();
	loff_t i;
	char *p = buf;

	if (nvram_size < 0)
		return nvram_size;
	for (i = *ppos; count > 0 && i < nvram_size; ++i, ++p, --count)
		*p = nvram_read_byte(i);
	*ppos = i;
	return p - buf;
}

static inline ssize_t nvram_write_bytes(char *buf, size_t count, loff_t *ppos)
{
	ssize_t nvram_size = nvram_get_size();
	loff_t i;
	char *p = buf;

	if (nvram_size < 0)
		return nvram_size;
	for (i = *ppos; count > 0 && i < nvram_size; ++i, ++p, --count)
		nvram_write_byte(*p, i);
	*ppos = i;
	return p - buf;
}

static inline ssize_t nvram_read(char *buf, size_t count, loff_t *ppos)
{
#ifdef CONFIG_PPC
	if (ppc_md.nvram_read)
		return ppc_md.nvram_read(buf, count, ppos);
#else
	if (arch_nvram_ops.read)
		return arch_nvram_ops.read(buf, count, ppos);
#endif
	return nvram_read_bytes(buf, count, ppos);
}

static inline ssize_t nvram_write(char *buf, size_t count, loff_t *ppos)
{
#ifdef CONFIG_PPC
	if (ppc_md.nvram_write)
		return ppc_md.nvram_write(buf, count, ppos);
#else
	if (arch_nvram_ops.write)
		return arch_nvram_ops.write(buf, count, ppos);
#endif
	return nvram_write_bytes(buf, count, ppos);
}

#endif  
