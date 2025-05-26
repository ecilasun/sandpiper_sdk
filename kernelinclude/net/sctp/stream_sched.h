/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __sctp_stream_sched_h__
#define __sctp_stream_sched_h__

struct sctp_sched_ops {
	
	int (*set)(struct sctp_stream *stream, __u16 sid, __u16 value,
		   gfp_t gfp);
	int (*get)(struct sctp_stream *stream, __u16 sid, __u16 *value);

	
	int (*init)(struct sctp_stream *stream);
	
	int (*init_sid)(struct sctp_stream *stream, __u16 sid, gfp_t gfp);
	
	void (*free_sid)(struct sctp_stream *stream, __u16 sid);

	
	void (*enqueue)(struct sctp_outq *q, struct sctp_datamsg *msg);
	
	struct sctp_chunk *(*dequeue)(struct sctp_outq *q);
	
	void (*dequeue_done)(struct sctp_outq *q, struct sctp_chunk *chunk);
	
	void (*sched_all)(struct sctp_stream *steam);
	
	void (*unsched_all)(struct sctp_stream *steam);
};

int sctp_sched_set_sched(struct sctp_association *asoc,
			 enum sctp_sched_type sched);
int sctp_sched_get_sched(struct sctp_association *asoc);
int sctp_sched_set_value(struct sctp_association *asoc, __u16 sid,
			 __u16 value, gfp_t gfp);
int sctp_sched_get_value(struct sctp_association *asoc, __u16 sid,
			 __u16 *value);
void sctp_sched_dequeue_done(struct sctp_outq *q, struct sctp_chunk *ch);

void sctp_sched_dequeue_common(struct sctp_outq *q, struct sctp_chunk *ch);
int sctp_sched_init_sid(struct sctp_stream *stream, __u16 sid, gfp_t gfp);
struct sctp_sched_ops *sctp_sched_ops_from_stream(struct sctp_stream *stream);

void sctp_sched_ops_register(enum sctp_sched_type sched,
			     struct sctp_sched_ops *sched_ops);
void sctp_sched_ops_prio_init(void);
void sctp_sched_ops_rr_init(void);
void sctp_sched_ops_fc_init(void);
void sctp_sched_ops_wfq_init(void);

#endif 
