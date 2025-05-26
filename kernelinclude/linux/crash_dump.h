/* SPDX-License-Identifier: GPL-2.0 */
#ifndef LINUX_CRASH_DUMP_H
#define LINUX_CRASH_DUMP_H

#include <linux/kexec.h>
#include <linux/proc_fs.h>
#include <linux/elf.h>
#include <linux/pgtable.h>
#include <uapi/linux/vmcore.h>


#define ELFCORE_ADDR_MAX	(-1ULL)
#define ELFCORE_ADDR_ERR	(-2ULL)

extern unsigned long long elfcorehdr_addr;
extern unsigned long long elfcorehdr_size;

#ifdef CONFIG_CRASH_DUMP
extern int elfcorehdr_alloc(unsigned long long *addr, unsigned long long *size);
extern void elfcorehdr_free(unsigned long long addr);
extern ssize_t elfcorehdr_read(char *buf, size_t count, u64 *ppos);
extern ssize_t elfcorehdr_read_notes(char *buf, size_t count, u64 *ppos);
extern int remap_oldmem_pfn_range(struct vm_area_struct *vma,
				  unsigned long from, unsigned long pfn,
				  unsigned long size, pgprot_t prot);

ssize_t copy_oldmem_page(struct iov_iter *i, unsigned long pfn, size_t csize,
		unsigned long offset);
ssize_t copy_oldmem_page_encrypted(struct iov_iter *iter, unsigned long pfn,
				   size_t csize, unsigned long offset);

void vmcore_cleanup(void);


#ifndef vmcore_elf_check_arch_cross
#define vmcore_elf_check_arch_cross(x) 0
#endif


#ifndef vmcore_elf32_check_arch
#define vmcore_elf32_check_arch(x) elf_check_arch(x)
#endif

#ifndef vmcore_elf64_check_arch
#define vmcore_elf64_check_arch(x) (elf_check_arch(x) || vmcore_elf_check_arch_cross(x))
#endif



static inline bool is_kdump_kernel(void)
{
	return elfcorehdr_addr != ELFCORE_ADDR_MAX;
}



static inline int is_vmcore_usable(void)
{
	return is_kdump_kernel() && elfcorehdr_addr != ELFCORE_ADDR_ERR ? 1 : 0;
}



static inline void vmcore_unusable(void)
{
	if (is_kdump_kernel())
		elfcorehdr_addr = ELFCORE_ADDR_ERR;
}


struct vmcore_cb {
	bool (*pfn_is_ram)(struct vmcore_cb *cb, unsigned long pfn);
	struct list_head next;
};
extern void register_vmcore_cb(struct vmcore_cb *cb);
extern void unregister_vmcore_cb(struct vmcore_cb *cb);

#else 
static inline bool is_kdump_kernel(void) { return false; }
#endif 


struct vmcoredd_data {
	char dump_name[VMCOREDD_MAX_NAME_BYTES]; 
	unsigned int size;                       
	
	int (*vmcoredd_callback)(struct vmcoredd_data *data, void *buf);
};

#ifdef CONFIG_PROC_VMCORE_DEVICE_DUMP
int vmcore_add_device_dump(struct vmcoredd_data *data);
#else
static inline int vmcore_add_device_dump(struct vmcoredd_data *data)
{
	return -EOPNOTSUPP;
}
#endif 

#ifdef CONFIG_PROC_VMCORE
ssize_t read_from_oldmem(struct iov_iter *iter, size_t count,
			 u64 *ppos, bool encrypted);
#else
static inline ssize_t read_from_oldmem(struct iov_iter *iter, size_t count,
				       u64 *ppos, bool encrypted)
{
	return -EOPNOTSUPP;
}
#endif 

#endif 
