/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_BINFMTS_H
#define _LINUX_BINFMTS_H

#include <linux/sched.h>
#include <linux/unistd.h>
#include <asm/exec.h>
#include <uapi/linux/binfmts.h>

struct filename;
struct coredump_params;

#define CORENAME_MAX_SIZE 128


struct linux_binprm {
#ifdef CONFIG_MMU
	struct vm_area_struct *vma;
	unsigned long vma_pages;
#else
# define MAX_ARG_PAGES	32
	struct page *page[MAX_ARG_PAGES];
#endif
	struct mm_struct *mm;
	unsigned long p; 
	unsigned long argmin; 
	unsigned int
		
		have_execfd:1,

		
		execfd_creds:1,
		
		secureexec:1,
		
		point_of_no_return:1;
	struct file *executable; 
	struct file *interpreter;
	struct file *file;
	struct cred *cred;	
	int unsafe;		
	unsigned int per_clear;	
	int argc, envc;
	const char *filename;	
	const char *interp;	
	const char *fdpath;	
	unsigned interp_flags;
	int execfd;		
	unsigned long loader, exec;

	struct rlimit rlim_stack; 

	char buf[BINPRM_BUF_SIZE];
} __randomize_layout;

#define BINPRM_FLAGS_ENFORCE_NONDUMP_BIT 0
#define BINPRM_FLAGS_ENFORCE_NONDUMP (1 << BINPRM_FLAGS_ENFORCE_NONDUMP_BIT)


#define BINPRM_FLAGS_PATH_INACCESSIBLE_BIT 2
#define BINPRM_FLAGS_PATH_INACCESSIBLE (1 << BINPRM_FLAGS_PATH_INACCESSIBLE_BIT)


#define BINPRM_FLAGS_PRESERVE_ARGV0_BIT 3
#define BINPRM_FLAGS_PRESERVE_ARGV0 (1 << BINPRM_FLAGS_PRESERVE_ARGV0_BIT)


struct linux_binfmt {
	struct list_head lh;
	struct module *module;
	int (*load_binary)(struct linux_binprm *);
	int (*load_shlib)(struct file *);
#ifdef CONFIG_COREDUMP
	int (*core_dump)(struct coredump_params *cprm);
	unsigned long min_coredump;	
#endif
} __randomize_layout;

extern void __register_binfmt(struct linux_binfmt *fmt, int insert);


static inline void register_binfmt(struct linux_binfmt *fmt)
{
	__register_binfmt(fmt, 0);
}

static inline void insert_binfmt(struct linux_binfmt *fmt)
{
	__register_binfmt(fmt, 1);
}

extern void unregister_binfmt(struct linux_binfmt *);

extern int __must_check remove_arg_zero(struct linux_binprm *);
extern int begin_new_exec(struct linux_binprm * bprm);
extern void setup_new_exec(struct linux_binprm * bprm);
extern void finalize_exec(struct linux_binprm *bprm);
extern void would_dump(struct linux_binprm *, struct file *);

extern int suid_dumpable;


#define EXSTACK_DEFAULT   0	
#define EXSTACK_DISABLE_X 1	
#define EXSTACK_ENABLE_X  2	

extern int setup_arg_pages(struct linux_binprm * bprm,
			   unsigned long stack_top,
			   int executable_stack);
extern int transfer_args_to_stack(struct linux_binprm *bprm,
				  unsigned long *sp_location);
extern int bprm_change_interp(const char *interp, struct linux_binprm *bprm);
int copy_string_kernel(const char *arg, struct linux_binprm *bprm);
extern void set_binfmt(struct linux_binfmt *new);
extern ssize_t read_code(struct file *, unsigned long, loff_t, size_t);

int kernel_execve(const char *filename,
		  const char *const *argv, const char *const *envp);

#endif 
