/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM rcu

#if !defined(_TRACE_RCU_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_RCU_H

#include <linux/tracepoint.h>

#ifdef CONFIG_RCU_TRACE
#define TRACE_EVENT_RCU TRACE_EVENT
#else
#define TRACE_EVENT_RCU TRACE_EVENT_NOP
#endif


TRACE_EVENT(rcu_utilization,

	TP_PROTO(const char *s),

	TP_ARGS(s),

	TP_STRUCT__entry(
		__field(const char *, s)
	),

	TP_fast_assign(
		__entry->s = s;
	),

	TP_printk("%s", __entry->s)
);

#if defined(CONFIG_TREE_RCU)


TRACE_EVENT_RCU(rcu_grace_period,

	TP_PROTO(const char *rcuname, unsigned long gp_seq, const char *gpevent),

	TP_ARGS(rcuname, gp_seq, gpevent),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(long, gp_seq)
		__field(const char *, gpevent)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->gp_seq = (long)gp_seq;
		__entry->gpevent = gpevent;
	),

	TP_printk("%s %ld %s",
		  __entry->rcuname, __entry->gp_seq, __entry->gpevent)
);


TRACE_EVENT_RCU(rcu_future_grace_period,

	TP_PROTO(const char *rcuname, unsigned long gp_seq,
		 unsigned long gp_seq_req, u8 level, int grplo, int grphi,
		 const char *gpevent),

	TP_ARGS(rcuname, gp_seq, gp_seq_req, level, grplo, grphi, gpevent),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(long, gp_seq)
		__field(long, gp_seq_req)
		__field(u8, level)
		__field(int, grplo)
		__field(int, grphi)
		__field(const char *, gpevent)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->gp_seq = (long)gp_seq;
		__entry->gp_seq_req = (long)gp_seq_req;
		__entry->level = level;
		__entry->grplo = grplo;
		__entry->grphi = grphi;
		__entry->gpevent = gpevent;
	),

	TP_printk("%s %ld %ld %u %d %d %s",
		  __entry->rcuname, (long)__entry->gp_seq, (long)__entry->gp_seq_req, __entry->level,
		  __entry->grplo, __entry->grphi, __entry->gpevent)
);


TRACE_EVENT_RCU(rcu_grace_period_init,

	TP_PROTO(const char *rcuname, unsigned long gp_seq, u8 level,
		 int grplo, int grphi, unsigned long qsmask),

	TP_ARGS(rcuname, gp_seq, level, grplo, grphi, qsmask),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(long, gp_seq)
		__field(u8, level)
		__field(int, grplo)
		__field(int, grphi)
		__field(unsigned long, qsmask)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->gp_seq = (long)gp_seq;
		__entry->level = level;
		__entry->grplo = grplo;
		__entry->grphi = grphi;
		__entry->qsmask = qsmask;
	),

	TP_printk("%s %ld %u %d %d %lx",
		  __entry->rcuname, __entry->gp_seq, __entry->level,
		  __entry->grplo, __entry->grphi, __entry->qsmask)
);


TRACE_EVENT_RCU(rcu_exp_grace_period,

	TP_PROTO(const char *rcuname, unsigned long gpseq, const char *gpevent),

	TP_ARGS(rcuname, gpseq, gpevent),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(long, gpseq)
		__field(const char *, gpevent)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->gpseq = (long)gpseq;
		__entry->gpevent = gpevent;
	),

	TP_printk("%s %ld %s",
		  __entry->rcuname, __entry->gpseq, __entry->gpevent)
);


TRACE_EVENT_RCU(rcu_exp_funnel_lock,

	TP_PROTO(const char *rcuname, u8 level, int grplo, int grphi,
		 const char *gpevent),

	TP_ARGS(rcuname, level, grplo, grphi, gpevent),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(u8, level)
		__field(int, grplo)
		__field(int, grphi)
		__field(const char *, gpevent)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->level = level;
		__entry->grplo = grplo;
		__entry->grphi = grphi;
		__entry->gpevent = gpevent;
	),

	TP_printk("%s %d %d %d %s",
		  __entry->rcuname, __entry->level, __entry->grplo,
		  __entry->grphi, __entry->gpevent)
);

#ifdef CONFIG_RCU_NOCB_CPU

TRACE_EVENT_RCU(rcu_nocb_wake,

	TP_PROTO(const char *rcuname, int cpu, const char *reason),

	TP_ARGS(rcuname, cpu, reason),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(int, cpu)
		__field(const char *, reason)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->cpu = cpu;
		__entry->reason = reason;
	),

	TP_printk("%s %d %s", __entry->rcuname, __entry->cpu, __entry->reason)
);
#endif


TRACE_EVENT_RCU(rcu_preempt_task,

	TP_PROTO(const char *rcuname, int pid, unsigned long gp_seq),

	TP_ARGS(rcuname, pid, gp_seq),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(long, gp_seq)
		__field(int, pid)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->gp_seq = (long)gp_seq;
		__entry->pid = pid;
	),

	TP_printk("%s %ld %d",
		  __entry->rcuname, __entry->gp_seq, __entry->pid)
);


TRACE_EVENT_RCU(rcu_unlock_preempted_task,

	TP_PROTO(const char *rcuname, unsigned long gp_seq, int pid),

	TP_ARGS(rcuname, gp_seq, pid),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(long, gp_seq)
		__field(int, pid)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->gp_seq = (long)gp_seq;
		__entry->pid = pid;
	),

	TP_printk("%s %ld %d", __entry->rcuname, __entry->gp_seq, __entry->pid)
);


TRACE_EVENT_RCU(rcu_quiescent_state_report,

	TP_PROTO(const char *rcuname, unsigned long gp_seq,
		 unsigned long mask, unsigned long qsmask,
		 u8 level, int grplo, int grphi, int gp_tasks),

	TP_ARGS(rcuname, gp_seq, mask, qsmask, level, grplo, grphi, gp_tasks),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(long, gp_seq)
		__field(unsigned long, mask)
		__field(unsigned long, qsmask)
		__field(u8, level)
		__field(int, grplo)
		__field(int, grphi)
		__field(u8, gp_tasks)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->gp_seq = (long)gp_seq;
		__entry->mask = mask;
		__entry->qsmask = qsmask;
		__entry->level = level;
		__entry->grplo = grplo;
		__entry->grphi = grphi;
		__entry->gp_tasks = gp_tasks;
	),

	TP_printk("%s %ld %lx>%lx %u %d %d %u",
		  __entry->rcuname, __entry->gp_seq,
		  __entry->mask, __entry->qsmask, __entry->level,
		  __entry->grplo, __entry->grphi, __entry->gp_tasks)
);


TRACE_EVENT_RCU(rcu_fqs,

	TP_PROTO(const char *rcuname, unsigned long gp_seq, int cpu, const char *qsevent),

	TP_ARGS(rcuname, gp_seq, cpu, qsevent),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(long, gp_seq)
		__field(int, cpu)
		__field(const char *, qsevent)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->gp_seq = (long)gp_seq;
		__entry->cpu = cpu;
		__entry->qsevent = qsevent;
	),

	TP_printk("%s %ld %d %s",
		  __entry->rcuname, __entry->gp_seq,
		  __entry->cpu, __entry->qsevent)
);


TRACE_EVENT(rcu_stall_warning,

	TP_PROTO(const char *rcuname, const char *msg),

	TP_ARGS(rcuname, msg),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(const char *, msg)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->msg = msg;
	),

	TP_printk("%s %s",
		  __entry->rcuname, __entry->msg)
);

#endif 


TRACE_EVENT_RCU(rcu_dyntick,

	TP_PROTO(const char *polarity, long oldnesting, long newnesting, int dynticks),

	TP_ARGS(polarity, oldnesting, newnesting, dynticks),

	TP_STRUCT__entry(
		__field(const char *, polarity)
		__field(long, oldnesting)
		__field(long, newnesting)
		__field(int, dynticks)
	),

	TP_fast_assign(
		__entry->polarity = polarity;
		__entry->oldnesting = oldnesting;
		__entry->newnesting = newnesting;
		__entry->dynticks = dynticks;
	),

	TP_printk("%s %lx %lx %#3x", __entry->polarity,
		  __entry->oldnesting, __entry->newnesting,
		  __entry->dynticks & 0xfff)
);


TRACE_EVENT_RCU(rcu_callback,

	TP_PROTO(const char *rcuname, struct rcu_head *rhp, long qlen),

	TP_ARGS(rcuname, rhp, qlen),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(void *, rhp)
		__field(void *, func)
		__field(long, qlen)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->rhp = rhp;
		__entry->func = rhp->func;
		__entry->qlen = qlen;
	),

	TP_printk("%s rhp=%p func=%ps %ld",
		  __entry->rcuname, __entry->rhp, __entry->func,
		  __entry->qlen)
);

TRACE_EVENT_RCU(rcu_segcb_stats,

		TP_PROTO(struct rcu_segcblist *rs, const char *ctx),

		TP_ARGS(rs, ctx),

		TP_STRUCT__entry(
			__field(const char *, ctx)
			__array(unsigned long, gp_seq, RCU_CBLIST_NSEGS)
			__array(long, seglen, RCU_CBLIST_NSEGS)
		),

		TP_fast_assign(
			__entry->ctx = ctx;
			memcpy(__entry->seglen, rs->seglen, RCU_CBLIST_NSEGS * sizeof(long));
			memcpy(__entry->gp_seq, rs->gp_seq, RCU_CBLIST_NSEGS * sizeof(unsigned long));

		),

		TP_printk("%s seglen: (DONE=%ld, WAIT=%ld, NEXT_READY=%ld, NEXT=%ld) "
			  "gp_seq: (DONE=%lu, WAIT=%lu, NEXT_READY=%lu, NEXT=%lu)", __entry->ctx,
			  __entry->seglen[0], __entry->seglen[1], __entry->seglen[2], __entry->seglen[3],
			  __entry->gp_seq[0], __entry->gp_seq[1], __entry->gp_seq[2], __entry->gp_seq[3])

);


TRACE_EVENT_RCU(rcu_kvfree_callback,

	TP_PROTO(const char *rcuname, struct rcu_head *rhp, unsigned long offset,
		 long qlen),

	TP_ARGS(rcuname, rhp, offset, qlen),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(void *, rhp)
		__field(unsigned long, offset)
		__field(long, qlen)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->rhp = rhp;
		__entry->offset = offset;
		__entry->qlen = qlen;
	),

	TP_printk("%s rhp=%p func=%ld %ld",
		  __entry->rcuname, __entry->rhp, __entry->offset,
		  __entry->qlen)
);


TRACE_EVENT_RCU(rcu_batch_start,

	TP_PROTO(const char *rcuname, long qlen, long blimit),

	TP_ARGS(rcuname, qlen, blimit),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(long, qlen)
		__field(long, blimit)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->qlen = qlen;
		__entry->blimit = blimit;
	),

	TP_printk("%s CBs=%ld bl=%ld",
		  __entry->rcuname, __entry->qlen, __entry->blimit)
);


TRACE_EVENT_RCU(rcu_invoke_callback,

	TP_PROTO(const char *rcuname, struct rcu_head *rhp),

	TP_ARGS(rcuname, rhp),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(void *, rhp)
		__field(void *, func)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->rhp = rhp;
		__entry->func = rhp->func;
	),

	TP_printk("%s rhp=%p func=%ps",
		  __entry->rcuname, __entry->rhp, __entry->func)
);


TRACE_EVENT_RCU(rcu_invoke_kvfree_callback,

	TP_PROTO(const char *rcuname, struct rcu_head *rhp, unsigned long offset),

	TP_ARGS(rcuname, rhp, offset),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(void *, rhp)
		__field(unsigned long, offset)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->rhp = rhp;
		__entry->offset	= offset;
	),

	TP_printk("%s rhp=%p func=%ld",
		  __entry->rcuname, __entry->rhp, __entry->offset)
);


TRACE_EVENT_RCU(rcu_invoke_kfree_bulk_callback,

	TP_PROTO(const char *rcuname, unsigned long nr_records, void **p),

	TP_ARGS(rcuname, nr_records, p),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(unsigned long, nr_records)
		__field(void **, p)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->nr_records = nr_records;
		__entry->p = p;
	),

	TP_printk("%s bulk=0x%p nr_records=%lu",
		__entry->rcuname, __entry->p, __entry->nr_records)
);


TRACE_EVENT_RCU(rcu_batch_end,

	TP_PROTO(const char *rcuname, int callbacks_invoked,
		 char cb, char nr, char iit, char risk),

	TP_ARGS(rcuname, callbacks_invoked, cb, nr, iit, risk),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(int, callbacks_invoked)
		__field(char, cb)
		__field(char, nr)
		__field(char, iit)
		__field(char, risk)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->callbacks_invoked = callbacks_invoked;
		__entry->cb = cb;
		__entry->nr = nr;
		__entry->iit = iit;
		__entry->risk = risk;
	),

	TP_printk("%s CBs-invoked=%d idle=%c%c%c%c",
		  __entry->rcuname, __entry->callbacks_invoked,
		  __entry->cb ? 'C' : '.',
		  __entry->nr ? 'S' : '.',
		  __entry->iit ? 'I' : '.',
		  __entry->risk ? 'R' : '.')
);


#define RCUTORTURENAME_LEN 8
TRACE_EVENT_RCU(rcu_torture_read,

	TP_PROTO(const char *rcutorturename, struct rcu_head *rhp,
		 unsigned long secs, unsigned long c_old, unsigned long c),

	TP_ARGS(rcutorturename, rhp, secs, c_old, c),

	TP_STRUCT__entry(
		__array(char, rcutorturename, RCUTORTURENAME_LEN)
		__field(struct rcu_head *, rhp)
		__field(unsigned long, secs)
		__field(unsigned long, c_old)
		__field(unsigned long, c)
	),

	TP_fast_assign(
		strscpy(__entry->rcutorturename, rcutorturename, RCUTORTURENAME_LEN);
		__entry->rhp = rhp;
		__entry->secs = secs;
		__entry->c_old = c_old;
		__entry->c = c;
	),

	TP_printk("%s torture read %p %luus c: %lu %lu",
		  __entry->rcutorturename, __entry->rhp,
		  __entry->secs, __entry->c_old, __entry->c)
);


TRACE_EVENT_RCU(rcu_barrier,

	TP_PROTO(const char *rcuname, const char *s, int cpu, int cnt, unsigned long done),

	TP_ARGS(rcuname, s, cpu, cnt, done),

	TP_STRUCT__entry(
		__field(const char *, rcuname)
		__field(const char *, s)
		__field(int, cpu)
		__field(int, cnt)
		__field(unsigned long, done)
	),

	TP_fast_assign(
		__entry->rcuname = rcuname;
		__entry->s = s;
		__entry->cpu = cpu;
		__entry->cnt = cnt;
		__entry->done = done;
	),

	TP_printk("%s %s cpu %d remaining %d # %lu",
		  __entry->rcuname, __entry->s, __entry->cpu, __entry->cnt,
		  __entry->done)
);

#endif 


#include <trace/define_trace.h>
