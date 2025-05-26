/* SPDX-License-Identifier: GPL-2.0 */
#ifndef LINUX_KEXEC_H
#define LINUX_KEXEC_H

#define IND_DESTINATION_BIT 0
#define IND_INDIRECTION_BIT 1
#define IND_DONE_BIT        2
#define IND_SOURCE_BIT      3

#define IND_DESTINATION  (1 << IND_DESTINATION_BIT)
#define IND_INDIRECTION  (1 << IND_INDIRECTION_BIT)
#define IND_DONE         (1 << IND_DONE_BIT)
#define IND_SOURCE       (1 << IND_SOURCE_BIT)
#define IND_FLAGS (IND_DESTINATION | IND_INDIRECTION | IND_DONE | IND_SOURCE)

#if !defined(__ASSEMBLY__)

#include <linux/crash_core.h>
#include <asm/io.h>
#include <linux/range.h>

#include <uapi/linux/kexec.h>
#include <linux/verification.h>


extern struct resource crashk_res;
extern struct resource crashk_low_res;
extern note_buf_t __percpu *crash_notes;

#ifdef CONFIG_KEXEC_CORE
#include <linux/list.h>
#include <linux/compat.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/highmem.h>
#include <asm/kexec.h>



#ifndef KEXEC_SOURCE_MEMORY_LIMIT
#error KEXEC_SOURCE_MEMORY_LIMIT not defined
#endif

#ifndef KEXEC_DESTINATION_MEMORY_LIMIT
#error KEXEC_DESTINATION_MEMORY_LIMIT not defined
#endif

#ifndef KEXEC_CONTROL_MEMORY_LIMIT
#error KEXEC_CONTROL_MEMORY_LIMIT not defined
#endif

#ifndef KEXEC_CONTROL_MEMORY_GFP
#define KEXEC_CONTROL_MEMORY_GFP (GFP_KERNEL | __GFP_NORETRY)
#endif

#ifndef KEXEC_CONTROL_PAGE_SIZE
#error KEXEC_CONTROL_PAGE_SIZE not defined
#endif

#ifndef KEXEC_ARCH
#error KEXEC_ARCH not defined
#endif

#ifndef KEXEC_CRASH_CONTROL_MEMORY_LIMIT
#define KEXEC_CRASH_CONTROL_MEMORY_LIMIT KEXEC_CONTROL_MEMORY_LIMIT
#endif

#ifndef KEXEC_CRASH_MEM_ALIGN
#define KEXEC_CRASH_MEM_ALIGN PAGE_SIZE
#endif

#define KEXEC_CORE_NOTE_NAME	CRASH_CORE_NOTE_NAME



typedef unsigned long kimage_entry_t;

struct kexec_segment {
	
	union {
		void __user *buf;
		void *kbuf;
	};
	size_t bufsz;
	unsigned long mem;
	size_t memsz;
};

#ifdef CONFIG_COMPAT
struct compat_kexec_segment {
	compat_uptr_t buf;
	compat_size_t bufsz;
	compat_ulong_t mem;	
	compat_size_t memsz;
};
#endif

#ifdef CONFIG_KEXEC_FILE
struct purgatory_info {
	
	const Elf_Ehdr *ehdr;
	
	Elf_Shdr *sechdrs;
	
	void *purgatory_buf;
};

struct kimage;

typedef int (kexec_probe_t)(const char *kernel_buf, unsigned long kernel_size);
typedef void *(kexec_load_t)(struct kimage *image, char *kernel_buf,
			     unsigned long kernel_len, char *initrd,
			     unsigned long initrd_len, char *cmdline,
			     unsigned long cmdline_len);
typedef int (kexec_cleanup_t)(void *loader_data);

#ifdef CONFIG_KEXEC_SIG
typedef int (kexec_verify_sig_t)(const char *kernel_buf,
				 unsigned long kernel_len);
#endif

struct kexec_file_ops {
	kexec_probe_t *probe;
	kexec_load_t *load;
	kexec_cleanup_t *cleanup;
#ifdef CONFIG_KEXEC_SIG
	kexec_verify_sig_t *verify_sig;
#endif
};

extern const struct kexec_file_ops * const kexec_file_loaders[];

int kexec_image_probe_default(struct kimage *image, void *buf,
			      unsigned long buf_len);
int kexec_image_post_load_cleanup_default(struct kimage *image);


#ifndef KEXEC_BUF_MEM_UNKNOWN
#define KEXEC_BUF_MEM_UNKNOWN 0
#endif


struct kexec_buf {
	struct kimage *image;
	void *buffer;
	unsigned long bufsz;
	unsigned long mem;
	unsigned long memsz;
	unsigned long buf_align;
	unsigned long buf_min;
	unsigned long buf_max;
	bool top_down;
};

int kexec_load_purgatory(struct kimage *image, struct kexec_buf *kbuf);
int kexec_purgatory_get_set_symbol(struct kimage *image, const char *name,
				   void *buf, unsigned int size,
				   bool get_value);
void *kexec_purgatory_get_symbol_addr(struct kimage *image, const char *name);

#ifndef arch_kexec_kernel_image_probe
static inline int
arch_kexec_kernel_image_probe(struct kimage *image, void *buf, unsigned long buf_len)
{
	return kexec_image_probe_default(image, buf, buf_len);
}
#endif

#ifndef arch_kimage_file_post_load_cleanup
static inline int arch_kimage_file_post_load_cleanup(struct kimage *image)
{
	return kexec_image_post_load_cleanup_default(image);
}
#endif

#ifdef CONFIG_KEXEC_SIG
#ifdef CONFIG_SIGNED_PE_FILE_VERIFICATION
int kexec_kernel_verify_pe_sig(const char *kernel, unsigned long kernel_len);
#endif
#endif

extern int kexec_add_buffer(struct kexec_buf *kbuf);
int kexec_locate_mem_hole(struct kexec_buf *kbuf);

#ifndef arch_kexec_locate_mem_hole

static inline int arch_kexec_locate_mem_hole(struct kexec_buf *kbuf)
{
	return kexec_locate_mem_hole(kbuf);
}
#endif

#ifndef arch_kexec_apply_relocations_add

static inline int
arch_kexec_apply_relocations_add(struct purgatory_info *pi, Elf_Shdr *section,
				 const Elf_Shdr *relsec, const Elf_Shdr *symtab)
{
	pr_err("RELA relocation unsupported.\n");
	return -ENOEXEC;
}
#endif

#ifndef arch_kexec_apply_relocations

static inline int
arch_kexec_apply_relocations(struct purgatory_info *pi, Elf_Shdr *section,
			     const Elf_Shdr *relsec, const Elf_Shdr *symtab)
{
	pr_err("REL relocation unsupported.\n");
	return -ENOEXEC;
}
#endif
#endif 

#ifdef CONFIG_KEXEC_ELF
struct kexec_elf_info {
	
	const char *buffer;

	const struct elfhdr *ehdr;
	const struct elf_phdr *proghdrs;
};

int kexec_build_elf_info(const char *buf, size_t len, struct elfhdr *ehdr,
			       struct kexec_elf_info *elf_info);

int kexec_elf_load(struct kimage *image, struct elfhdr *ehdr,
			 struct kexec_elf_info *elf_info,
			 struct kexec_buf *kbuf,
			 unsigned long *lowest_load_addr);

void kexec_free_elf_info(struct kexec_elf_info *elf_info);
int kexec_elf_probe(const char *buf, unsigned long len);
#endif
struct kimage {
	kimage_entry_t head;
	kimage_entry_t *entry;
	kimage_entry_t *last_entry;

	unsigned long start;
	struct page *control_code_page;
	struct page *swap_page;
	void *vmcoreinfo_data_copy; 

	unsigned long nr_segments;
	struct kexec_segment segment[KEXEC_SEGMENT_MAX];

	struct list_head control_pages;
	struct list_head dest_pages;
	struct list_head unusable_pages;

	
	unsigned long control_page;

	
	unsigned int type : 1;
#define KEXEC_TYPE_DEFAULT 0
#define KEXEC_TYPE_CRASH   1
	unsigned int preserve_context : 1;
	
	unsigned int file_mode:1;
#ifdef CONFIG_CRASH_HOTPLUG
	
	unsigned int update_elfcorehdr:1;
#endif

#ifdef ARCH_HAS_KIMAGE_ARCH
	struct kimage_arch arch;
#endif

#ifdef CONFIG_KEXEC_FILE
	
	void *kernel_buf;
	unsigned long kernel_buf_len;

	void *initrd_buf;
	unsigned long initrd_buf_len;

	char *cmdline_buf;
	unsigned long cmdline_buf_len;

	
	const struct kexec_file_ops *fops;

	
	void *image_loader_data;

	
	struct purgatory_info purgatory_info;
#endif

#ifdef CONFIG_CRASH_HOTPLUG
	int hp_action;
	int elfcorehdr_index;
	bool elfcorehdr_updated;
#endif

#ifdef CONFIG_IMA_KEXEC
	
	void *ima_buffer;

	phys_addr_t ima_buffer_addr;
	size_t ima_buffer_size;
#endif

	
	void *elf_headers;
	unsigned long elf_headers_sz;
	unsigned long elf_load_addr;
};


extern void machine_kexec(struct kimage *image);
extern int machine_kexec_prepare(struct kimage *image);
extern void machine_kexec_cleanup(struct kimage *image);
extern int kernel_kexec(void);
extern struct page *kimage_alloc_control_pages(struct kimage *image,
						unsigned int order);

#ifndef machine_kexec_post_load
static inline int machine_kexec_post_load(struct kimage *image) { return 0; }
#endif

extern void __crash_kexec(struct pt_regs *);
extern void crash_kexec(struct pt_regs *);
int kexec_should_crash(struct task_struct *);
int kexec_crash_loaded(void);
void crash_save_cpu(struct pt_regs *regs, int cpu);
extern int kimage_crash_copy_vmcoreinfo(struct kimage *image);

extern struct kimage *kexec_image;
extern struct kimage *kexec_crash_image;

bool kexec_load_permitted(int kexec_image_type);

#ifndef kexec_flush_icache_page
#define kexec_flush_icache_page(page)
#endif


#ifndef CONFIG_KEXEC_JUMP
#define KEXEC_FLAGS    (KEXEC_ON_CRASH | KEXEC_UPDATE_ELFCOREHDR)
#else
#define KEXEC_FLAGS    (KEXEC_ON_CRASH | KEXEC_PRESERVE_CONTEXT | KEXEC_UPDATE_ELFCOREHDR)
#endif


#define KEXEC_FILE_FLAGS	(KEXEC_FILE_UNLOAD | KEXEC_FILE_ON_CRASH | \
				 KEXEC_FILE_NO_INITRAMFS)


extern bool kexec_in_progress;

int crash_shrink_memory(unsigned long new_size);
ssize_t crash_get_memory_size(void);

#ifndef arch_kexec_protect_crashkres

static inline void arch_kexec_protect_crashkres(void) { }
#endif

#ifndef arch_kexec_unprotect_crashkres
static inline void arch_kexec_unprotect_crashkres(void) { }
#endif

#ifndef page_to_boot_pfn
static inline unsigned long page_to_boot_pfn(struct page *page)
{
	return page_to_pfn(page);
}
#endif

#ifndef boot_pfn_to_page
static inline struct page *boot_pfn_to_page(unsigned long boot_pfn)
{
	return pfn_to_page(boot_pfn);
}
#endif

#ifndef phys_to_boot_phys
static inline unsigned long phys_to_boot_phys(phys_addr_t phys)
{
	return phys;
}
#endif

#ifndef boot_phys_to_phys
static inline phys_addr_t boot_phys_to_phys(unsigned long boot_phys)
{
	return boot_phys;
}
#endif

#ifndef crash_free_reserved_phys_range
static inline void crash_free_reserved_phys_range(unsigned long begin, unsigned long end)
{
	unsigned long addr;

	for (addr = begin; addr < end; addr += PAGE_SIZE)
		free_reserved_page(boot_pfn_to_page(addr >> PAGE_SHIFT));
}
#endif

static inline unsigned long virt_to_boot_phys(void *addr)
{
	return phys_to_boot_phys(__pa((unsigned long)addr));
}

static inline void *boot_phys_to_virt(unsigned long entry)
{
	return phys_to_virt(boot_phys_to_phys(entry));
}

#ifndef arch_kexec_post_alloc_pages
static inline int arch_kexec_post_alloc_pages(void *vaddr, unsigned int pages, gfp_t gfp) { return 0; }
#endif

#ifndef arch_kexec_pre_free_pages
static inline void arch_kexec_pre_free_pages(void *vaddr, unsigned int pages) { }
#endif

#ifndef arch_crash_handle_hotplug_event
static inline void arch_crash_handle_hotplug_event(struct kimage *image) { }
#endif

int crash_check_update_elfcorehdr(void);

#ifndef crash_hotplug_cpu_support
static inline int crash_hotplug_cpu_support(void) { return 0; }
#endif

#ifndef crash_hotplug_memory_support
static inline int crash_hotplug_memory_support(void) { return 0; }
#endif

#ifndef crash_get_elfcorehdr_size
static inline unsigned int crash_get_elfcorehdr_size(void) { return 0; }
#endif

#else 
struct pt_regs;
struct task_struct;
static inline void __crash_kexec(struct pt_regs *regs) { }
static inline void crash_kexec(struct pt_regs *regs) { }
static inline int kexec_should_crash(struct task_struct *p) { return 0; }
static inline int kexec_crash_loaded(void) { return 0; }
#define kexec_in_progress false
#endif 

#ifdef CONFIG_KEXEC_SIG
void set_kexec_sig_enforced(void);
#else
static inline void set_kexec_sig_enforced(void) {}
#endif

#endif 

#endif 
