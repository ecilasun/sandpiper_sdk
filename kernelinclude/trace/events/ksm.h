/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM ksm

#if !defined(_TRACE_KSM_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_KSM_H

#include <linux/tracepoint.h>


DECLARE_EVENT_CLASS(ksm_scan_template,

	TP_PROTO(int seq, u32 rmap_entries),

	TP_ARGS(seq, rmap_entries),

	TP_STRUCT__entry(
		__field(int,	seq)
		__field(u32,	rmap_entries)
	),

	TP_fast_assign(
		__entry->seq		= seq;
		__entry->rmap_entries	= rmap_entries;
	),

	TP_printk("seq %d rmap size %d",
			__entry->seq, __entry->rmap_entries)
);


DEFINE_EVENT(ksm_scan_template, ksm_start_scan,

	TP_PROTO(int seq, u32 rmap_entries),

	TP_ARGS(seq, rmap_entries)
);


DEFINE_EVENT(ksm_scan_template, ksm_stop_scan,

	TP_PROTO(int seq, u32 rmap_entries),

	TP_ARGS(seq, rmap_entries)
);


DECLARE_EVENT_CLASS(ksm_enter_exit_template,

	TP_PROTO(void *mm),

	TP_ARGS(mm),

	TP_STRUCT__entry(
		__field(void *,		mm)
	),

	TP_fast_assign(
		__entry->mm	= mm;
	),

	TP_printk("mm %p", __entry->mm)
);


DEFINE_EVENT(ksm_enter_exit_template, ksm_enter,

	TP_PROTO(void *mm),

	TP_ARGS(mm)
);


DEFINE_EVENT(ksm_enter_exit_template, ksm_exit,

	TP_PROTO(void *mm),

	TP_ARGS(mm)
);


TRACE_EVENT(ksm_merge_one_page,

	TP_PROTO(unsigned long pfn, void *rmap_item, void *mm, int err),

	TP_ARGS(pfn, rmap_item, mm, err),

	TP_STRUCT__entry(
		__field(unsigned long,	pfn)
		__field(void *,		rmap_item)
		__field(void *,		mm)
		__field(int,		err)
	),

	TP_fast_assign(
		__entry->pfn		= pfn;
		__entry->rmap_item	= rmap_item;
		__entry->mm		= mm;
		__entry->err		= err;
	),

	TP_printk("ksm pfn %lu rmap_item %p mm %p error %d",
			__entry->pfn, __entry->rmap_item, __entry->mm, __entry->err)
);


TRACE_EVENT(ksm_merge_with_ksm_page,

	TP_PROTO(void *ksm_page, unsigned long pfn, void *rmap_item, void *mm, int err),

	TP_ARGS(ksm_page, pfn, rmap_item, mm, err),

	TP_STRUCT__entry(
		__field(void *,		ksm_page)
		__field(unsigned long,	pfn)
		__field(void *,		rmap_item)
		__field(void *,		mm)
		__field(int,		err)
	),

	TP_fast_assign(
		__entry->ksm_page	= ksm_page;
		__entry->pfn		= pfn;
		__entry->rmap_item	= rmap_item;
		__entry->mm		= mm;
		__entry->err		= err;
	),

	TP_printk("%spfn %lu rmap_item %p mm %p error %d",
		  (__entry->ksm_page ? "ksm " : ""),
		  __entry->pfn, __entry->rmap_item, __entry->mm, __entry->err)
);


TRACE_EVENT(ksm_remove_ksm_page,

	TP_PROTO(unsigned long pfn),

	TP_ARGS(pfn),

	TP_STRUCT__entry(
		__field(unsigned long, pfn)
	),

	TP_fast_assign(
		__entry->pfn = pfn;
	),

	TP_printk("pfn %lu", __entry->pfn)
);


TRACE_EVENT(ksm_remove_rmap_item,

	TP_PROTO(unsigned long pfn, void *rmap_item, void *mm),

	TP_ARGS(pfn, rmap_item, mm),

	TP_STRUCT__entry(
		__field(unsigned long,	pfn)
		__field(void *,		rmap_item)
		__field(void *,		mm)
	),

	TP_fast_assign(
		__entry->pfn		= pfn;
		__entry->rmap_item	= rmap_item;
		__entry->mm		= mm;
	),

	TP_printk("pfn %lu rmap_item %p mm %p",
			__entry->pfn, __entry->rmap_item, __entry->mm)
);

#endif 


#include <trace/define_trace.h>
