/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_GENERIC_SECTIONS_H_
#define _ASM_GENERIC_SECTIONS_H_



#include <linux/compiler.h>
#include <linux/types.h>


extern char _text[], _stext[], _etext[];
extern char _data[], _sdata[], _edata[];
extern char __bss_start[], __bss_stop[];
extern char __init_begin[], __init_end[];
extern char _sinittext[], _einittext[];
extern char __start_ro_after_init[], __end_ro_after_init[];
extern char _end[];
extern char __per_cpu_load[], __per_cpu_start[], __per_cpu_end[];
extern char __kprobes_text_start[], __kprobes_text_end[];
extern char __entry_text_start[], __entry_text_end[];
extern char __start_rodata[], __end_rodata[];
extern char __irqentry_text_start[], __irqentry_text_end[];
extern char __softirqentry_text_start[], __softirqentry_text_end[];
extern char __start_once[], __end_once[];


extern char __ctors_start[], __ctors_end[];


extern char __start_opd[], __end_opd[];


extern char __noinstr_text_start[], __noinstr_text_end[];

extern __visible const void __nosave_begin, __nosave_end;


#ifdef CONFIG_HAVE_FUNCTION_DESCRIPTORS
void *dereference_function_descriptor(void *ptr);
void *dereference_kernel_function_descriptor(void *ptr);
#else
#define dereference_function_descriptor(p) ((void *)(p))
#define dereference_kernel_function_descriptor(p) ((void *)(p))


typedef struct {
	unsigned long addr;
} func_desc_t;
#endif

static inline bool have_function_descriptors(void)
{
	return IS_ENABLED(CONFIG_HAVE_FUNCTION_DESCRIPTORS);
}


static inline bool memory_contains(void *begin, void *end, void *virt,
				   size_t size)
{
	return virt >= begin && virt + size <= end;
}


static inline bool memory_intersects(void *begin, void *end, void *virt,
				     size_t size)
{
	void *vend = virt + size;

	if (virt < end && vend > begin)
		return true;

	return false;
}


static inline bool init_section_contains(void *virt, size_t size)
{
	return memory_contains(__init_begin, __init_end, virt, size);
}


static inline bool init_section_intersects(void *virt, size_t size)
{
	return memory_intersects(__init_begin, __init_end, virt, size);
}


static inline bool is_kernel_core_data(unsigned long addr)
{
	if (addr >= (unsigned long)_sdata && addr < (unsigned long)_edata)
		return true;

	if (addr >= (unsigned long)__bss_start &&
	    addr < (unsigned long)__bss_stop)
		return true;

	return false;
}


static inline bool is_kernel_rodata(unsigned long addr)
{
	return addr >= (unsigned long)__start_rodata &&
	       addr < (unsigned long)__end_rodata;
}


static inline bool is_kernel_inittext(unsigned long addr)
{
	return addr >= (unsigned long)_sinittext &&
	       addr < (unsigned long)_einittext;
}


static inline bool __is_kernel_text(unsigned long addr)
{
	return addr >= (unsigned long)_stext &&
	       addr < (unsigned long)_etext;
}


static inline bool __is_kernel(unsigned long addr)
{
	return ((addr >= (unsigned long)_stext &&
	         addr < (unsigned long)_end) ||
		(addr >= (unsigned long)__init_begin &&
		 addr < (unsigned long)__init_end));
}

#endif 
