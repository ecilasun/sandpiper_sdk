

#ifndef LOAD_OFFSET
#define LOAD_OFFSET 0
#endif


#ifdef EMITS_PT_NOTE
#define NOTES_HEADERS		:text :note
#define NOTES_HEADERS_RESTORE	__restore_ph : { *(.__restore_ph) } :text
#else
#define NOTES_HEADERS
#define NOTES_HEADERS_RESTORE
#endif


#ifdef RO_EXCEPTION_TABLE_ALIGN
#define RO_EXCEPTION_TABLE	EXCEPTION_TABLE(RO_EXCEPTION_TABLE_ALIGN)
#else
#define RO_EXCEPTION_TABLE
#endif


#define ALIGN_FUNCTION()  . = ALIGN(CONFIG_FUNCTION_ALIGNMENT)


#if defined(CONFIG_LD_DEAD_CODE_DATA_ELIMINATION) || defined(CONFIG_LTO_CLANG)
#define TEXT_MAIN .text .text.[0-9a-zA-Z_]*
#define DATA_MAIN .data .data.[0-9a-zA-Z_]* .data..L* .data..compoundliteral* .data.$__unnamed_* .data.$L*
#define SDATA_MAIN .sdata .sdata.[0-9a-zA-Z_]*
#define RODATA_MAIN .rodata .rodata.[0-9a-zA-Z_]* .rodata..L*
#define BSS_MAIN .bss .bss.[0-9a-zA-Z_]* .bss..compoundliteral*
#define SBSS_MAIN .sbss .sbss.[0-9a-zA-Z_]*
#else
#define TEXT_MAIN .text
#define DATA_MAIN .data
#define SDATA_MAIN .sdata
#define RODATA_MAIN .rodata
#define BSS_MAIN .bss
#define SBSS_MAIN .sbss
#endif


#define STRUCT_ALIGNMENT 32
#define STRUCT_ALIGN() . = ALIGN(STRUCT_ALIGNMENT)


#define SCHED_DATA				\
	STRUCT_ALIGN();				\
	__sched_class_highest = .;		\
	*(__stop_sched_class)			\
	*(__dl_sched_class)			\
	*(__rt_sched_class)			\
	*(__fair_sched_class)			\
	*(__idle_sched_class)			\
	__sched_class_lowest = .;



#if defined(CONFIG_MEMORY_HOTPLUG)
#define MEM_KEEP(sec)    *(.mem##sec)
#define MEM_DISCARD(sec)
#else
#define MEM_KEEP(sec)
#define MEM_DISCARD(sec) *(.mem##sec)
#endif

#ifndef CONFIG_HAVE_DYNAMIC_FTRACE_NO_PATCHABLE
#define KEEP_PATCHABLE		KEEP(*(__patchable_function_entries))
#define PATCHABLE_DISCARDS
#else
#define KEEP_PATCHABLE
#define PATCHABLE_DISCARDS	*(__patchable_function_entries)
#endif

#ifndef CONFIG_ARCH_SUPPORTS_CFI_CLANG

#define	FTRACE_STUB_HACK	ftrace_stub_graph = ftrace_stub;
#else
#define FTRACE_STUB_HACK
#endif

#ifdef CONFIG_FTRACE_MCOUNT_RECORD

#define MCOUNT_REC()	. = ALIGN(8);				\
			__start_mcount_loc = .;			\
			KEEP(*(__mcount_loc))			\
			KEEP_PATCHABLE				\
			__stop_mcount_loc = .;			\
			FTRACE_STUB_HACK			\
			ftrace_ops_list_func = arch_ftrace_ops_list_func;
#else
# ifdef CONFIG_FUNCTION_TRACER
#  define MCOUNT_REC()	FTRACE_STUB_HACK			\
			ftrace_ops_list_func = arch_ftrace_ops_list_func;
# else
#  define MCOUNT_REC()
# endif
#endif

#define BOUNDED_SECTION_PRE_LABEL(_sec_, _label_, _BEGIN_, _END_)	\
	_BEGIN_##_label_ = .;						\
	KEEP(*(_sec_))							\
	_END_##_label_ = .;

#define BOUNDED_SECTION_POST_LABEL(_sec_, _label_, _BEGIN_, _END_)	\
	_label_##_BEGIN_ = .;						\
	KEEP(*(_sec_))							\
	_label_##_END_ = .;

#define BOUNDED_SECTION_BY(_sec_, _label_)				\
	BOUNDED_SECTION_PRE_LABEL(_sec_, _label_, __start, __stop)

#define BOUNDED_SECTION(_sec)	 BOUNDED_SECTION_BY(_sec, _sec)

#define HEADERED_SECTION_PRE_LABEL(_sec_, _label_, _BEGIN_, _END_, _HDR_) \
	_HDR_##_label_	= .;						\
	KEEP(*(.gnu.linkonce.##_sec_))					\
	BOUNDED_SECTION_PRE_LABEL(_sec_, _label_, _BEGIN_, _END_)

#define HEADERED_SECTION_POST_LABEL(_sec_, _label_, _BEGIN_, _END_, _HDR_) \
	_label_##_HDR_ = .;						\
	KEEP(*(.gnu.linkonce.##_sec_))					\
	BOUNDED_SECTION_POST_LABEL(_sec_, _label_, _BEGIN_, _END_)

#define HEADERED_SECTION_BY(_sec_, _label_)				\
	HEADERED_SECTION_PRE_LABEL(_sec_, _label_, __start, __stop)

#define HEADERED_SECTION(_sec)	 HEADERED_SECTION_BY(_sec, _sec)

#ifdef CONFIG_TRACE_BRANCH_PROFILING
#define LIKELY_PROFILE()						\
	BOUNDED_SECTION_BY(_ftrace_annotated_branch, _annotated_branch_profile)
#else
#define LIKELY_PROFILE()
#endif

#ifdef CONFIG_PROFILE_ALL_BRANCHES
#define BRANCH_PROFILE()					\
	BOUNDED_SECTION_BY(_ftrace_branch, _branch_profile)
#else
#define BRANCH_PROFILE()
#endif

#ifdef CONFIG_KPROBES
#define KPROBE_BLACKLIST()				\
	. = ALIGN(8);					\
	BOUNDED_SECTION(_kprobe_blacklist)
#else
#define KPROBE_BLACKLIST()
#endif

#ifdef CONFIG_FUNCTION_ERROR_INJECTION
#define ERROR_INJECT_WHITELIST()			\
	STRUCT_ALIGN();					\
	BOUNDED_SECTION(_error_injection_whitelist)
#else
#define ERROR_INJECT_WHITELIST()
#endif

#ifdef CONFIG_EVENT_TRACING
#define FTRACE_EVENTS()							\
	. = ALIGN(8);							\
	BOUNDED_SECTION(_ftrace_events)					\
	BOUNDED_SECTION_BY(_ftrace_eval_map, _ftrace_eval_maps)
#else
#define FTRACE_EVENTS()
#endif

#ifdef CONFIG_TRACING
#define TRACE_PRINTKS()		BOUNDED_SECTION_BY(__trace_printk_fmt, ___trace_bprintk_fmt)
#define TRACEPOINT_STR()	BOUNDED_SECTION_BY(__tracepoint_str, ___tracepoint_str)
#else
#define TRACE_PRINTKS()
#define TRACEPOINT_STR()
#endif

#ifdef CONFIG_FTRACE_SYSCALLS
#define TRACE_SYSCALLS()			\
	. = ALIGN(8);				\
	BOUNDED_SECTION_BY(__syscalls_metadata, _syscalls_metadata)
#else
#define TRACE_SYSCALLS()
#endif

#ifdef CONFIG_BPF_EVENTS
#define BPF_RAW_TP() STRUCT_ALIGN();				\
	BOUNDED_SECTION_BY(__bpf_raw_tp_map, __bpf_raw_tp)
#else
#define BPF_RAW_TP()
#endif

#ifdef CONFIG_SERIAL_EARLYCON
#define EARLYCON_TABLE()						\
	. = ALIGN(8);							\
	BOUNDED_SECTION_POST_LABEL(__earlycon_table, __earlycon_table, , _end)
#else
#define EARLYCON_TABLE()
#endif

#ifdef CONFIG_SECURITY
#define LSM_TABLE()					\
	. = ALIGN(8);					\
	BOUNDED_SECTION_PRE_LABEL(.lsm_info.init, _lsm_info, __start, __end)

#define EARLY_LSM_TABLE()						\
	. = ALIGN(8);							\
	BOUNDED_SECTION_PRE_LABEL(.early_lsm_info.init, _early_lsm_info, __start, __end)
#else
#define LSM_TABLE()
#define EARLY_LSM_TABLE()
#endif

#define ___OF_TABLE(cfg, name)	_OF_TABLE_##cfg(name)
#define __OF_TABLE(cfg, name)	___OF_TABLE(cfg, name)
#define OF_TABLE(cfg, name)	__OF_TABLE(IS_ENABLED(cfg), name)
#define _OF_TABLE_0(name)
#define _OF_TABLE_1(name)						\
	. = ALIGN(8);							\
	__##name##_of_table = .;					\
	KEEP(*(__##name##_of_table))					\
	KEEP(*(__##name##_of_table_end))

#define TIMER_OF_TABLES()	OF_TABLE(CONFIG_TIMER_OF, timer)
#define IRQCHIP_OF_MATCH_TABLE() OF_TABLE(CONFIG_IRQCHIP, irqchip)
#define CLK_OF_TABLES()		OF_TABLE(CONFIG_COMMON_CLK, clk)
#define RESERVEDMEM_OF_TABLES()	OF_TABLE(CONFIG_OF_RESERVED_MEM, reservedmem)
#define CPU_METHOD_OF_TABLES()	OF_TABLE(CONFIG_SMP, cpu_method)
#define CPUIDLE_METHOD_OF_TABLES() OF_TABLE(CONFIG_CPU_IDLE, cpuidle_method)

#ifdef CONFIG_ACPI
#define ACPI_PROBE_TABLE(name)						\
	. = ALIGN(8);							\
	BOUNDED_SECTION_POST_LABEL(__##name##_acpi_probe_table,		\
				   __##name##_acpi_probe_table,, _end)
#else
#define ACPI_PROBE_TABLE(name)
#endif

#ifdef CONFIG_THERMAL
#define THERMAL_TABLE(name)						\
	. = ALIGN(8);							\
	BOUNDED_SECTION_POST_LABEL(__##name##_thermal_table,		\
				   __##name##_thermal_table,, _end)
#else
#define THERMAL_TABLE(name)
#endif

#define KERNEL_DTB()							\
	STRUCT_ALIGN();							\
	__dtb_start = .;						\
	KEEP(*(.dtb.init.rodata))					\
	__dtb_end = .;


#define DATA_DATA							\
	*(.xiptext)							\
	*(DATA_MAIN)							\
	*(.data..decrypted)						\
	*(.ref.data)							\
	*(.data..shared_aligned) 			\
	MEM_KEEP(init.data*)						\
	*(.data.unlikely)						\
	__start_once = .;						\
	*(.data.once)							\
	__end_once = .;							\
	STRUCT_ALIGN();							\
	*(__tracepoints)						\
					\
	. = ALIGN(8);							\
	BOUNDED_SECTION_BY(__dyndbg_classes, ___dyndbg_classes)		\
	BOUNDED_SECTION_BY(__dyndbg, ___dyndbg)				\
	LIKELY_PROFILE()		       				\
	BRANCH_PROFILE()						\
	TRACE_PRINTKS()							\
	BPF_RAW_TP()							\
	TRACEPOINT_STR()


#define NOSAVE_DATA							\
	. = ALIGN(PAGE_SIZE);						\
	__nosave_begin = .;						\
	*(.data..nosave)						\
	. = ALIGN(PAGE_SIZE);						\
	__nosave_end = .;

#define PAGE_ALIGNED_DATA(page_align)					\
	. = ALIGN(page_align);						\
	*(.data..page_aligned)						\
	. = ALIGN(page_align);

#define READ_MOSTLY_DATA(align)						\
	. = ALIGN(align);						\
	*(.data..read_mostly)						\
	. = ALIGN(align);

#define CACHELINE_ALIGNED_DATA(align)					\
	. = ALIGN(align);						\
	*(.data..cacheline_aligned)

#define INIT_TASK_DATA(align)						\
	. = ALIGN(align);						\
	__start_init_task = .;						\
	init_thread_union = .;						\
	init_stack = .;							\
	KEEP(*(.data..init_task))					\
	KEEP(*(.data..init_thread_info))				\
	. = __start_init_task + THREAD_SIZE;				\
	__end_init_task = .;

#define JUMP_TABLE_DATA							\
	. = ALIGN(8);							\
	BOUNDED_SECTION_BY(__jump_table, ___jump_table)

#ifdef CONFIG_HAVE_STATIC_CALL_INLINE
#define STATIC_CALL_DATA						\
	. = ALIGN(8);							\
	BOUNDED_SECTION_BY(.static_call_sites, _static_call_sites)	\
	BOUNDED_SECTION_BY(.static_call_tramp_key, _static_call_tramp_key)
#else
#define STATIC_CALL_DATA
#endif


#ifndef RO_AFTER_INIT_DATA
#define RO_AFTER_INIT_DATA						\
	. = ALIGN(8);							\
	__start_ro_after_init = .;					\
	*(.data..ro_after_init)						\
	JUMP_TABLE_DATA							\
	STATIC_CALL_DATA						\
	__end_ro_after_init = .;
#endif


#ifndef KCFI_TRAPS
#ifdef CONFIG_ARCH_USES_CFI_TRAPS
#define KCFI_TRAPS							\
	__kcfi_traps : AT(ADDR(__kcfi_traps) - LOAD_OFFSET) {		\
		BOUNDED_SECTION_BY(.kcfi_traps, ___kcfi_traps)		\
	}
#else
#define KCFI_TRAPS
#endif
#endif


#define RO_DATA(align)							\
	. = ALIGN((align));						\
	.rodata           : AT(ADDR(.rodata) - LOAD_OFFSET) {		\
		__start_rodata = .;					\
		*(.rodata) *(.rodata.*)					\
		SCHED_DATA						\
		RO_AFTER_INIT_DATA		\
		. = ALIGN(8);						\
		BOUNDED_SECTION_BY(__tracepoints_ptrs, ___tracepoints_ptrs) \
		*(__tracepoints_strings)	\
	}								\
									\
	.rodata1          : AT(ADDR(.rodata1) - LOAD_OFFSET) {		\
		*(.rodata1)						\
	}								\
									\
							\
	.pci_fixup        : AT(ADDR(.pci_fixup) - LOAD_OFFSET) {	\
		BOUNDED_SECTION_PRE_LABEL(.pci_fixup_early,  _pci_fixups_early,  __start, __end) \
		BOUNDED_SECTION_PRE_LABEL(.pci_fixup_header, _pci_fixups_header, __start, __end) \
		BOUNDED_SECTION_PRE_LABEL(.pci_fixup_final,  _pci_fixups_final,  __start, __end) \
		BOUNDED_SECTION_PRE_LABEL(.pci_fixup_enable, _pci_fixups_enable, __start, __end) \
		BOUNDED_SECTION_PRE_LABEL(.pci_fixup_resume, _pci_fixups_resume, __start, __end) \
		BOUNDED_SECTION_PRE_LABEL(.pci_fixup_suspend, _pci_fixups_suspend, __start, __end) \
		BOUNDED_SECTION_PRE_LABEL(.pci_fixup_resume_early, _pci_fixups_resume_early, __start, __end) \
		BOUNDED_SECTION_PRE_LABEL(.pci_fixup_suspend_late, _pci_fixups_suspend_late, __start, __end) \
	}								\
									\
	FW_LOADER_BUILT_IN_DATA						\
	TRACEDATA							\
									\
	PRINTK_INDEX							\
									\
				\
	__ksymtab         : AT(ADDR(__ksymtab) - LOAD_OFFSET) {		\
		__start___ksymtab = .;					\
		KEEP(*(SORT(___ksymtab+*)))				\
		__stop___ksymtab = .;					\
	}								\
									\
				\
	__ksymtab_gpl     : AT(ADDR(__ksymtab_gpl) - LOAD_OFFSET) {	\
		__start___ksymtab_gpl = .;				\
		KEEP(*(SORT(___ksymtab_gpl+*)))				\
		__stop___ksymtab_gpl = .;				\
	}								\
									\
				\
	__kcrctab         : AT(ADDR(__kcrctab) - LOAD_OFFSET) {		\
		__start___kcrctab = .;					\
		KEEP(*(SORT(___kcrctab+*)))				\
		__stop___kcrctab = .;					\
	}								\
									\
				\
	__kcrctab_gpl     : AT(ADDR(__kcrctab_gpl) - LOAD_OFFSET) {	\
		__start___kcrctab_gpl = .;				\
		KEEP(*(SORT(___kcrctab_gpl+*)))				\
		__stop___kcrctab_gpl = .;				\
	}								\
									\
					\
        __ksymtab_strings : AT(ADDR(__ksymtab_strings) - LOAD_OFFSET) {	\
		*(__ksymtab_strings)					\
	}								\
									\
							\
	__init_rodata : AT(ADDR(__init_rodata) - LOAD_OFFSET) {		\
		*(.ref.rodata)						\
		MEM_KEEP(init.rodata)					\
	}								\
									\
					\
	__param : AT(ADDR(__param) - LOAD_OFFSET) {			\
		BOUNDED_SECTION_BY(__param, ___param)			\
	}								\
									\
						\
	__modver : AT(ADDR(__modver) - LOAD_OFFSET) {			\
		BOUNDED_SECTION_BY(__modver, ___modver)			\
	}								\
									\
	KCFI_TRAPS							\
									\
	RO_EXCEPTION_TABLE						\
	NOTES								\
	BTF								\
									\
	. = ALIGN((align));						\
	__end_rodata = .;



#define NOINSTR_TEXT							\
		ALIGN_FUNCTION();					\
		__noinstr_text_start = .;				\
		*(.noinstr.text)					\
		__cpuidle_text_start = .;				\
		*(.cpuidle.text)					\
		__cpuidle_text_end = .;					\
		__noinstr_text_end = .;


#define TEXT_TEXT							\
		ALIGN_FUNCTION();					\
		*(.text.hot .text.hot.*)				\
		*(TEXT_MAIN .text.fixup)				\
		*(.text.unlikely .text.unlikely.*)			\
		*(.text.unknown .text.unknown.*)			\
		NOINSTR_TEXT						\
		*(.ref.text)						\
		*(.text.asan.* .text.tsan.*)				\
	MEM_KEEP(init.text*)						\



#define SCHED_TEXT							\
		ALIGN_FUNCTION();					\
		__sched_text_start = .;					\
		*(.sched.text)						\
		__sched_text_end = .;


#define LOCK_TEXT							\
		ALIGN_FUNCTION();					\
		__lock_text_start = .;					\
		*(.spinlock.text)					\
		__lock_text_end = .;

#define KPROBES_TEXT							\
		ALIGN_FUNCTION();					\
		__kprobes_text_start = .;				\
		*(.kprobes.text)					\
		__kprobes_text_end = .;

#define ENTRY_TEXT							\
		ALIGN_FUNCTION();					\
		__entry_text_start = .;					\
		*(.entry.text)						\
		__entry_text_end = .;

#define IRQENTRY_TEXT							\
		ALIGN_FUNCTION();					\
		__irqentry_text_start = .;				\
		*(.irqentry.text)					\
		__irqentry_text_end = .;

#define SOFTIRQENTRY_TEXT						\
		ALIGN_FUNCTION();					\
		__softirqentry_text_start = .;				\
		*(.softirqentry.text)					\
		__softirqentry_text_end = .;

#define STATIC_CALL_TEXT						\
		ALIGN_FUNCTION();					\
		__static_call_text_start = .;				\
		*(.static_call.text)					\
		__static_call_text_end = .;


#define HEAD_TEXT  KEEP(*(.head.text))

#define HEAD_TEXT_SECTION							\
	.head.text : AT(ADDR(.head.text) - LOAD_OFFSET) {		\
		HEAD_TEXT						\
	}


#define EXCEPTION_TABLE(align)						\
	. = ALIGN(align);						\
	__ex_table : AT(ADDR(__ex_table) - LOAD_OFFSET) {		\
		BOUNDED_SECTION_BY(__ex_table, ___ex_table)		\
	}


#ifdef CONFIG_DEBUG_INFO_BTF
#define BTF								\
	.BTF : AT(ADDR(.BTF) - LOAD_OFFSET) {				\
		BOUNDED_SECTION_BY(.BTF, _BTF)				\
	}								\
	. = ALIGN(4);							\
	.BTF_ids : AT(ADDR(.BTF_ids) - LOAD_OFFSET) {			\
		*(.BTF_ids)						\
	}
#else
#define BTF
#endif


#define INIT_TASK_DATA_SECTION(align)					\
	. = ALIGN(align);						\
	.data..init_task :  AT(ADDR(.data..init_task) - LOAD_OFFSET) {	\
		INIT_TASK_DATA(align)					\
	}

#ifdef CONFIG_CONSTRUCTORS
#define KERNEL_CTORS()	. = ALIGN(8);			   \
			__ctors_start = .;		   \
			KEEP(*(SORT(.ctors.*)))		   \
			KEEP(*(.ctors))			   \
			KEEP(*(SORT(.init_array.*)))	   \
			KEEP(*(.init_array))		   \
			__ctors_end = .;
#else
#define KERNEL_CTORS()
#endif


#define INIT_DATA							\
	KEEP(*(SORT(___kentry+*)))					\
	*(.init.data .init.data.*)					\
	MEM_DISCARD(init.data*)						\
	KERNEL_CTORS()							\
	MCOUNT_REC()							\
	*(.init.rodata .init.rodata.*)					\
	FTRACE_EVENTS()							\
	TRACE_SYSCALLS()						\
	KPROBE_BLACKLIST()						\
	ERROR_INJECT_WHITELIST()					\
	MEM_DISCARD(init.rodata)					\
	CLK_OF_TABLES()							\
	RESERVEDMEM_OF_TABLES()						\
	TIMER_OF_TABLES()						\
	CPU_METHOD_OF_TABLES()						\
	CPUIDLE_METHOD_OF_TABLES()					\
	KERNEL_DTB()							\
	IRQCHIP_OF_MATCH_TABLE()					\
	ACPI_PROBE_TABLE(irqchip)					\
	ACPI_PROBE_TABLE(timer)						\
	THERMAL_TABLE(governor)						\
	EARLYCON_TABLE()						\
	LSM_TABLE()							\
	EARLY_LSM_TABLE()						\
	KUNIT_TABLE()

#define INIT_TEXT							\
	*(.init.text .init.text.*)					\
	*(.text.startup)						\
	MEM_DISCARD(init.text*)

#define EXIT_DATA							\
	*(.exit.data .exit.data.*)					\
	*(.fini_array .fini_array.*)					\
	*(.dtors .dtors.*)						\

#define EXIT_TEXT							\
	*(.exit.text)							\
	*(.text.exit)							\

#define EXIT_CALL							\
	*(.exitcall.exit)


#define SBSS(sbss_align)						\
	. = ALIGN(sbss_align);						\
	.sbss : AT(ADDR(.sbss) - LOAD_OFFSET) {				\
		*(.dynsbss)						\
		*(SBSS_MAIN)						\
		*(.scommon)						\
	}


#ifndef BSS_FIRST_SECTIONS
#define BSS_FIRST_SECTIONS
#endif

#define BSS(bss_align)							\
	. = ALIGN(bss_align);						\
	.bss : AT(ADDR(.bss) - LOAD_OFFSET) {				\
		BSS_FIRST_SECTIONS					\
		. = ALIGN(PAGE_SIZE);					\
		*(.bss..page_aligned)					\
		. = ALIGN(PAGE_SIZE);					\
		*(.dynbss)						\
		*(BSS_MAIN)						\
		*(COMMON)						\
	}


#define DWARF_DEBUG							\
								\
		.debug          0 : { *(.debug) }			\
		.line           0 : { *(.line) }			\
						\
		.debug_srcinfo  0 : { *(.debug_srcinfo) }		\
		.debug_sfnames  0 : { *(.debug_sfnames) }		\
						\
		.debug_aranges  0 : { *(.debug_aranges) }		\
		.debug_pubnames 0 : { *(.debug_pubnames) }		\
								\
		.debug_info     0 : { *(.debug_info			\
				.gnu.linkonce.wi.*) }			\
		.debug_abbrev   0 : { *(.debug_abbrev) }		\
		.debug_line     0 : { *(.debug_line) }			\
		.debug_frame    0 : { *(.debug_frame) }			\
		.debug_str      0 : { *(.debug_str) }			\
		.debug_loc      0 : { *(.debug_loc) }			\
		.debug_macinfo  0 : { *(.debug_macinfo) }		\
		.debug_pubtypes 0 : { *(.debug_pubtypes) }		\
								\
		.debug_ranges	0 : { *(.debug_ranges) }		\
					\
		.debug_weaknames 0 : { *(.debug_weaknames) }		\
		.debug_funcnames 0 : { *(.debug_funcnames) }		\
		.debug_typenames 0 : { *(.debug_typenames) }		\
		.debug_varnames  0 : { *(.debug_varnames) }		\
						\
		.debug_gnu_pubnames 0 : { *(.debug_gnu_pubnames) }	\
		.debug_gnu_pubtypes 0 : { *(.debug_gnu_pubtypes) }	\
								\
		.debug_types	0 : { *(.debug_types) }			\
								\
		.debug_addr	0 : { *(.debug_addr) }			\
		.debug_line_str	0 : { *(.debug_line_str) }		\
		.debug_loclists	0 : { *(.debug_loclists) }		\
		.debug_macro	0 : { *(.debug_macro) }			\
		.debug_names	0 : { *(.debug_names) }			\
		.debug_rnglists	0 : { *(.debug_rnglists) }		\
		.debug_str_offsets	0 : { *(.debug_str_offsets) }


#define STABS_DEBUG							\
		.stab 0 : { *(.stab) }					\
		.stabstr 0 : { *(.stabstr) }				\
		.stab.excl 0 : { *(.stab.excl) }			\
		.stab.exclstr 0 : { *(.stab.exclstr) }			\
		.stab.index 0 : { *(.stab.index) }			\
		.stab.indexstr 0 : { *(.stab.indexstr) }


#define ELF_DETAILS							\
		.comment 0 : { *(.comment) }				\
		.symtab 0 : { *(.symtab) }				\
		.strtab 0 : { *(.strtab) }				\
		.shstrtab 0 : { *(.shstrtab) }

#ifdef CONFIG_GENERIC_BUG
#define BUG_TABLE							\
	. = ALIGN(8);							\
	__bug_table : AT(ADDR(__bug_table) - LOAD_OFFSET) {		\
		BOUNDED_SECTION_BY(__bug_table, ___bug_table)		\
	}
#else
#define BUG_TABLE
#endif

#ifdef CONFIG_UNWINDER_ORC
#define ORC_UNWIND_TABLE						\
	.orc_header : AT(ADDR(.orc_header) - LOAD_OFFSET) {		\
		BOUNDED_SECTION_BY(.orc_header, _orc_header)		\
	}								\
	. = ALIGN(4);							\
	.orc_unwind_ip : AT(ADDR(.orc_unwind_ip) - LOAD_OFFSET) {	\
		BOUNDED_SECTION_BY(.orc_unwind_ip, _orc_unwind_ip)	\
	}								\
	. = ALIGN(2);							\
	.orc_unwind : AT(ADDR(.orc_unwind) - LOAD_OFFSET) {		\
		BOUNDED_SECTION_BY(.orc_unwind, _orc_unwind)		\
	}								\
	text_size = _etext - _stext;					\
	. = ALIGN(4);							\
	.orc_lookup : AT(ADDR(.orc_lookup) - LOAD_OFFSET) {		\
		orc_lookup = .;						\
		. += (((text_size + LOOKUP_BLOCK_SIZE - 1) /		\
			LOOKUP_BLOCK_SIZE) + 1) * 4;			\
		orc_lookup_end = .;					\
	}
#else
#define ORC_UNWIND_TABLE
#endif


#ifdef CONFIG_FW_LOADER
#define FW_LOADER_BUILT_IN_DATA						\
	.builtin_fw : AT(ADDR(.builtin_fw) - LOAD_OFFSET) ALIGN(8) {	\
		BOUNDED_SECTION_PRE_LABEL(.builtin_fw, _builtin_fw, __start, __end) \
	}
#else
#define FW_LOADER_BUILT_IN_DATA
#endif

#ifdef CONFIG_PM_TRACE
#define TRACEDATA							\
	. = ALIGN(4);							\
	.tracedata : AT(ADDR(.tracedata) - LOAD_OFFSET) {		\
		BOUNDED_SECTION_POST_LABEL(.tracedata, __tracedata, _start, _end) \
	}
#else
#define TRACEDATA
#endif

#ifdef CONFIG_PRINTK_INDEX
#define PRINTK_INDEX							\
	.printk_index : AT(ADDR(.printk_index) - LOAD_OFFSET) {		\
		BOUNDED_SECTION_BY(.printk_index, _printk_index)	\
	}
#else
#define PRINTK_INDEX
#endif


#define NOTES								\
	/DISCARD/ : {							\
		*(.note.GNU-stack)					\
		*(.note.gnu.property)					\
	}								\
	.notes : AT(ADDR(.notes) - LOAD_OFFSET) {			\
		BOUNDED_SECTION_BY(.note.*, _notes)			\
	} NOTES_HEADERS							\
	NOTES_HEADERS_RESTORE

#define INIT_SETUP(initsetup_align)					\
		. = ALIGN(initsetup_align);				\
		BOUNDED_SECTION_POST_LABEL(.init.setup, __setup, _start, _end)

#define INIT_CALLS_LEVEL(level)						\
		__initcall##level##_start = .;				\
		KEEP(*(.initcall##level##.init))			\
		KEEP(*(.initcall##level##s.init))			\

#define INIT_CALLS							\
		__initcall_start = .;					\
		KEEP(*(.initcallearly.init))				\
		INIT_CALLS_LEVEL(0)					\
		INIT_CALLS_LEVEL(1)					\
		INIT_CALLS_LEVEL(2)					\
		INIT_CALLS_LEVEL(3)					\
		INIT_CALLS_LEVEL(4)					\
		INIT_CALLS_LEVEL(5)					\
		INIT_CALLS_LEVEL(rootfs)				\
		INIT_CALLS_LEVEL(6)					\
		INIT_CALLS_LEVEL(7)					\
		__initcall_end = .;

#define CON_INITCALL							\
	BOUNDED_SECTION_POST_LABEL(.con_initcall.init, __con_initcall, _start, _end)


#define KUNIT_TABLE()							\
		. = ALIGN(8);						\
		BOUNDED_SECTION_POST_LABEL(.kunit_test_suites, __kunit_suites, _start, _end)

#ifdef CONFIG_BLK_DEV_INITRD
#define INIT_RAM_FS							\
	. = ALIGN(4);							\
	__initramfs_start = .;						\
	KEEP(*(.init.ramfs))						\
	. = ALIGN(8);							\
	KEEP(*(.init.ramfs.info))
#else
#define INIT_RAM_FS
#endif


#ifdef CONFIG_AMD_MEM_ENCRYPT
#define PERCPU_DECRYPTED_SECTION					\
	. = ALIGN(PAGE_SIZE);						\
	*(.data..percpu..decrypted)					\
	. = ALIGN(PAGE_SIZE);
#else
#define PERCPU_DECRYPTED_SECTION
#endif



#ifdef RUNTIME_DISCARD_EXIT
#define EXIT_DISCARDS
#else
#define EXIT_DISCARDS							\
	EXIT_TEXT							\
	EXIT_DATA
#endif


#ifdef CONFIG_UNWIND_TABLES
#define DISCARD_EH_FRAME
#else
#define DISCARD_EH_FRAME	*(.eh_frame)
#endif
#if defined(CONFIG_GCOV_KERNEL) || defined(CONFIG_KASAN_GENERIC) || defined(CONFIG_KCSAN)
# ifdef CONFIG_CONSTRUCTORS
#  define SANITIZER_DISCARDS						\
	DISCARD_EH_FRAME
# else
#  define SANITIZER_DISCARDS						\
	*(.init_array) *(.init_array.*)					\
	DISCARD_EH_FRAME
# endif
#else
# define SANITIZER_DISCARDS
#endif

#define COMMON_DISCARDS							\
	SANITIZER_DISCARDS						\
	PATCHABLE_DISCARDS						\
	*(.discard)							\
	*(.discard.*)							\
	*(.export_symbol)						\
	*(.modinfo)							\
		\
	*(.gnu.version*)						\

#define DISCARDS							\
	/DISCARD/ : {							\
	EXIT_DISCARDS							\
	EXIT_CALL							\
	COMMON_DISCARDS							\
	}


#define PERCPU_INPUT(cacheline)						\
	__per_cpu_start = .;						\
	*(.data..percpu..first)						\
	. = ALIGN(PAGE_SIZE);						\
	*(.data..percpu..page_aligned)					\
	. = ALIGN(cacheline);						\
	*(.data..percpu..read_mostly)					\
	. = ALIGN(cacheline);						\
	*(.data..percpu)						\
	*(.data..percpu..shared_aligned)				\
	PERCPU_DECRYPTED_SECTION					\
	__per_cpu_end = .;


#define PERCPU_VADDR(cacheline, vaddr, phdr)				\
	__per_cpu_load = .;						\
	.data..percpu vaddr : AT(__per_cpu_load - LOAD_OFFSET) {	\
		PERCPU_INPUT(cacheline)					\
	} phdr								\
	. = __per_cpu_load + SIZEOF(.data..percpu);


#define PERCPU_SECTION(cacheline)					\
	. = ALIGN(PAGE_SIZE);						\
	.data..percpu	: AT(ADDR(.data..percpu) - LOAD_OFFSET) {	\
		__per_cpu_load = .;					\
		PERCPU_INPUT(cacheline)					\
	}






#define RW_DATA(cacheline, pagealigned, inittask)			\
	. = ALIGN(PAGE_SIZE);						\
	.data : AT(ADDR(.data) - LOAD_OFFSET) {				\
		INIT_TASK_DATA(inittask)				\
		NOSAVE_DATA						\
		PAGE_ALIGNED_DATA(pagealigned)				\
		CACHELINE_ALIGNED_DATA(cacheline)			\
		READ_MOSTLY_DATA(cacheline)				\
		DATA_DATA						\
		CONSTRUCTORS						\
	}								\
	BUG_TABLE							\

#define INIT_TEXT_SECTION(inittext_align)				\
	. = ALIGN(inittext_align);					\
	.init.text : AT(ADDR(.init.text) - LOAD_OFFSET) {		\
		_sinittext = .;						\
		INIT_TEXT						\
		_einittext = .;						\
	}

#define INIT_DATA_SECTION(initsetup_align)				\
	.init.data : AT(ADDR(.init.data) - LOAD_OFFSET) {		\
		INIT_DATA						\
		INIT_SETUP(initsetup_align)				\
		INIT_CALLS						\
		CON_INITCALL						\
		INIT_RAM_FS						\
	}

#define BSS_SECTION(sbss_align, bss_align, stop_align)			\
	. = ALIGN(sbss_align);						\
	__bss_start = .;						\
	SBSS(sbss_align)						\
	BSS(bss_align)							\
	. = ALIGN(stop_align);						\
	__bss_stop = .;
