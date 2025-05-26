/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _LINUX_TCP_H
#define _LINUX_TCP_H


#include <linux/skbuff.h>
#include <linux/win_minmax.h>
#include <net/sock.h>
#include <net/inet_connection_sock.h>
#include <net/inet_timewait_sock.h>
#include <uapi/linux/tcp.h>

static inline struct tcphdr *tcp_hdr(const struct sk_buff *skb)
{
	return (struct tcphdr *)skb_transport_header(skb);
}

static inline unsigned int __tcp_hdrlen(const struct tcphdr *th)
{
	return th->doff * 4;
}

static inline unsigned int tcp_hdrlen(const struct sk_buff *skb)
{
	return __tcp_hdrlen(tcp_hdr(skb));
}

static inline struct tcphdr *inner_tcp_hdr(const struct sk_buff *skb)
{
	return (struct tcphdr *)skb_inner_transport_header(skb);
}

static inline unsigned int inner_tcp_hdrlen(const struct sk_buff *skb)
{
	return inner_tcp_hdr(skb)->doff * 4;
}


static inline int skb_tcp_all_headers(const struct sk_buff *skb)
{
	return skb_transport_offset(skb) + tcp_hdrlen(skb);
}


static inline int skb_inner_tcp_all_headers(const struct sk_buff *skb)
{
	return skb_inner_transport_offset(skb) + inner_tcp_hdrlen(skb);
}

static inline unsigned int tcp_optlen(const struct sk_buff *skb)
{
	return (tcp_hdr(skb)->doff - 5) * 4;
}


#define TCP_FASTOPEN_COOKIE_MIN	4	
#define TCP_FASTOPEN_COOKIE_MAX	16	
#define TCP_FASTOPEN_COOKIE_SIZE 8	


struct tcp_fastopen_cookie {
	__le64	val[DIV_ROUND_UP(TCP_FASTOPEN_COOKIE_MAX, sizeof(u64))];
	s8	len;
	bool	exp;	
};


struct tcp_sack_block_wire {
	__be32	start_seq;
	__be32	end_seq;
};

struct tcp_sack_block {
	u32	start_seq;
	u32	end_seq;
};


#define TCP_SACK_SEEN     (1 << 0)   
#define TCP_DSACK_SEEN    (1 << 2)   

struct tcp_options_received {

	int	ts_recent_stamp;
	u32	ts_recent;	
	u32	rcv_tsval;	
	u32	rcv_tsecr;	
	u16 	saw_tstamp : 1,	
		tstamp_ok : 1,	
		dsack : 1,	
		wscale_ok : 1,	
		sack_ok : 3,	
		smc_ok : 1,	
		snd_wscale : 4,	
		rcv_wscale : 4;	
	u8	saw_unknown:1,	
		unused:7;
	u8	num_sacks;	
	u16	user_mss;	
	u16	mss_clamp;	
};

static inline void tcp_clear_options(struct tcp_options_received *rx_opt)
{
	rx_opt->tstamp_ok = rx_opt->sack_ok = 0;
	rx_opt->wscale_ok = rx_opt->snd_wscale = 0;
#if IS_ENABLED(CONFIG_SMC)
	rx_opt->smc_ok = 0;
#endif
}


#define TCP_NUM_SACKS 4

struct tcp_request_sock_ops;

struct tcp_request_sock {
	struct inet_request_sock 	req;
	const struct tcp_request_sock_ops *af_specific;
	u64				snt_synack; 
	bool				tfo_listener;
	bool				is_mptcp;
#if IS_ENABLED(CONFIG_MPTCP)
	bool				drop_req;
#endif
	u32				txhash;
	u32				rcv_isn;
	u32				snt_isn;
	u32				ts_off;
	u32				last_oow_ack_time; 
	u32				rcv_nxt; 
	u8				syn_tos;
};

static inline struct tcp_request_sock *tcp_rsk(const struct request_sock *req)
{
	return (struct tcp_request_sock *)req;
}

#define TCP_RMEM_TO_WIN_SCALE 8

struct tcp_sock {
	
	struct inet_connection_sock	inet_conn;
	u16	tcp_header_len;	
	u16	gso_segs;	


	__be32	pred_flags;


	u64	bytes_received;	
	u32	segs_in;	
	u32	data_segs_in;	
 	u32	rcv_nxt;	
	u32	copied_seq;	
	u32	rcv_wup;	
 	u32	snd_nxt;	
	u32	segs_out;	
	u32	data_segs_out;	
	u64	bytes_sent;	
	u64	bytes_acked;	
	u32	dsack_dups;	
 	u32	snd_una;	
 	u32	snd_sml;	
	u32	rcv_tstamp;	
	u32	lsndtime;	
	u32	last_oow_ack_time;  
	u32	compressed_ack_rcv_nxt;

	u32	tsoffset;	

	struct list_head tsq_node; 
	struct list_head tsorted_sent_queue; 

	u32	snd_wl1;	
	u32	snd_wnd;	
	u32	max_window;	
	u32	mss_cache;	

	u32	window_clamp;	
	u32	rcv_ssthresh;	
	u8	scaling_ratio;	
	
	struct tcp_rack {
		u64 mstamp; 
		u32 rtt_us;  
		u32 end_seq; 
		u32 last_delivered; 
		u8 reo_wnd_steps;   
#define TCP_RACK_RECOVERY_THRESH 16
		u8 reo_wnd_persist:5, 
		   dsack_seen:1, 
		   advanced:1;	 
	} rack;
	u16	advmss;		
	u8	compressed_ack;
	u8	dup_ack_counter:2,
		tlp_retrans:1,	
		unused:5;
	u32	chrono_start;	
	u32	chrono_stat[3];	
	u8	chrono_type:2,	
		rate_app_limited:1,  
		fastopen_connect:1, 
		fastopen_no_cookie:1, 
		is_sack_reneg:1,    
		fastopen_client_fail:2; 
	u8	nonagle     : 4,
		thin_lto    : 1,
		recvmsg_inq : 1,
		repair      : 1,
		frto        : 1;
	u8	repair_queue;
	u8	save_syn:2,	
		syn_data:1,	
		syn_fastopen:1,	
		syn_fastopen_exp:1,
		syn_fastopen_ch:1, 
		syn_data_acked:1,
		is_cwnd_limited:1;
	u32	tlp_high_seq;	

	u32	tcp_tx_delay;	
	u64	tcp_wstamp_ns;	
	u64	tcp_clock_cache; 


	u64	tcp_mstamp;	
	u32	srtt_us;	
	u32	mdev_us;	
	u32	mdev_max_us;	
	u32	rttvar_us;	
	u32	rtt_seq;	
	struct  minmax rtt_min;

	u32	packets_out;	
	u32	retrans_out;	
	u32	max_packets_out;  
	u32	cwnd_usage_seq;  

	u16	urg_data;	
	u8	ecn_flags;	
	u8	keepalive_probes; 
	u32	reordering;	
	u32	reord_seen;	
	u32	snd_up;		


	struct tcp_options_received rx_opt;


 	u32	snd_ssthresh;	
 	u32	snd_cwnd;	
	u32	snd_cwnd_cnt;	
	u32	snd_cwnd_clamp; 
	u32	snd_cwnd_used;
	u32	snd_cwnd_stamp;
	u32	prior_cwnd;	
	u32	prr_delivered;	
	u32	prr_out;	
	u32	delivered;	
	u32	delivered_ce;	
	u32	lost;		
	u32	app_limited;	
	u64	first_tx_mstamp;  
	u64	delivered_mstamp; 
	u32	rate_delivered;    
	u32	rate_interval_us;  

 	u32	rcv_wnd;	
	u32	write_seq;	
	u32	notsent_lowat;	
	u32	pushed_seq;	
	u32	lost_out;	
	u32	sacked_out;	

	struct hrtimer	pacing_timer;
	struct hrtimer	compressed_ack_timer;

	
	struct sk_buff* lost_skb_hint;
	struct sk_buff *retransmit_skb_hint;

	
	struct rb_root	out_of_order_queue;
	struct sk_buff	*ooo_last_skb; 

	
	struct tcp_sack_block duplicate_sack[1]; 
	struct tcp_sack_block selective_acks[4]; 

	struct tcp_sack_block recv_sack_cache[4];

	struct sk_buff *highest_sack;   

	int     lost_cnt_hint;

	u32	prior_ssthresh; 
	u32	high_seq;	

	u32	retrans_stamp;	
	u32	undo_marker;	
	int	undo_retrans;	
	u64	bytes_retrans;	
	u32	total_retrans;	

	u32	urg_seq;	
	unsigned int		keepalive_time;	  
	unsigned int		keepalive_intvl;  

	int			linger2;



#ifdef CONFIG_BPF
	u8	bpf_sock_ops_cb_flags;  
	u8	bpf_chg_cc_inprogress:1; 
#define BPF_SOCK_OPS_TEST_FLAG(TP, ARG) (TP->bpf_sock_ops_cb_flags & ARG)
#else
#define BPF_SOCK_OPS_TEST_FLAG(TP, ARG) 0
#endif

	u16 timeout_rehash;	

	u32 rcv_ooopack; 


	u32 rcv_rtt_last_tsecr;
	struct {
		u32	rtt_us;
		u32	seq;
		u64	time;
	} rcv_rtt_est;


	struct {
		u32	space;
		u32	seq;
		u64	time;
	} rcvq_space;


	struct {
		u32		  probe_seq_start;
		u32		  probe_seq_end;
	} mtu_probe;
	u32     plb_rehash;     
	u32	mtu_info; 
#if IS_ENABLED(CONFIG_MPTCP)
	bool	is_mptcp;
#endif
#if IS_ENABLED(CONFIG_SMC)
	bool	(*smc_hs_congested)(const struct sock *sk);
	bool	syn_smc;	
#endif

#ifdef CONFIG_TCP_MD5SIG

	const struct tcp_sock_af_ops	*af_specific;


	struct tcp_md5sig_info	__rcu *md5sig_info;
#endif


	struct tcp_fastopen_request *fastopen_req;
	
	struct request_sock __rcu *fastopen_rsk;
	struct saved_syn *saved_syn;
};

enum tsq_enum {
	TSQ_THROTTLED,
	TSQ_QUEUED,
	TCP_TSQ_DEFERRED,	   
	TCP_WRITE_TIMER_DEFERRED,  
	TCP_DELACK_TIMER_DEFERRED, 
	TCP_MTU_REDUCED_DEFERRED,  
};

enum tsq_flags {
	TSQF_THROTTLED			= (1UL << TSQ_THROTTLED),
	TSQF_QUEUED			= (1UL << TSQ_QUEUED),
	TCPF_TSQ_DEFERRED		= (1UL << TCP_TSQ_DEFERRED),
	TCPF_WRITE_TIMER_DEFERRED	= (1UL << TCP_WRITE_TIMER_DEFERRED),
	TCPF_DELACK_TIMER_DEFERRED	= (1UL << TCP_DELACK_TIMER_DEFERRED),
	TCPF_MTU_REDUCED_DEFERRED	= (1UL << TCP_MTU_REDUCED_DEFERRED),
};

#define tcp_sk(ptr) container_of_const(ptr, struct tcp_sock, inet_conn.icsk_inet.sk)


#define tcp_sk_rw(ptr) container_of(ptr, struct tcp_sock, inet_conn.icsk_inet.sk)

struct tcp_timewait_sock {
	struct inet_timewait_sock tw_sk;
#define tw_rcv_nxt tw_sk.__tw_common.skc_tw_rcv_nxt
#define tw_snd_nxt tw_sk.__tw_common.skc_tw_snd_nxt
	u32			  tw_rcv_wnd;
	u32			  tw_ts_offset;
	u32			  tw_ts_recent;

	
	u32			  tw_last_oow_ack_time;

	int			  tw_ts_recent_stamp;
	u32			  tw_tx_delay;
#ifdef CONFIG_TCP_MD5SIG
	struct tcp_md5sig_key	  *tw_md5_key;
#endif
};

static inline struct tcp_timewait_sock *tcp_twsk(const struct sock *sk)
{
	return (struct tcp_timewait_sock *)sk;
}

static inline bool tcp_passive_fastopen(const struct sock *sk)
{
	return sk->sk_state == TCP_SYN_RECV &&
	       rcu_access_pointer(tcp_sk(sk)->fastopen_rsk) != NULL;
}

static inline void fastopen_queue_tune(struct sock *sk, int backlog)
{
	struct request_sock_queue *queue = &inet_csk(sk)->icsk_accept_queue;
	int somaxconn = READ_ONCE(sock_net(sk)->core.sysctl_somaxconn);

	WRITE_ONCE(queue->fastopenq.max_qlen, min_t(unsigned int, backlog, somaxconn));
}

static inline void tcp_move_syn(struct tcp_sock *tp,
				struct request_sock *req)
{
	tp->saved_syn = req->saved_syn;
	req->saved_syn = NULL;
}

static inline void tcp_saved_syn_free(struct tcp_sock *tp)
{
	kfree(tp->saved_syn);
	tp->saved_syn = NULL;
}

static inline u32 tcp_saved_syn_len(const struct saved_syn *saved_syn)
{
	return saved_syn->mac_hdrlen + saved_syn->network_hdrlen +
		saved_syn->tcp_hdrlen;
}

struct sk_buff *tcp_get_timestamping_opt_stats(const struct sock *sk,
					       const struct sk_buff *orig_skb,
					       const struct sk_buff *ack_skb);

static inline u16 tcp_mss_clamp(const struct tcp_sock *tp, u16 mss)
{
	
	u16 user_mss = READ_ONCE(tp->rx_opt.user_mss);

	return (user_mss && user_mss < mss) ? user_mss : mss;
}

int tcp_skb_shift(struct sk_buff *to, struct sk_buff *from, int pcount,
		  int shiftlen);

void __tcp_sock_set_cork(struct sock *sk, bool on);
void tcp_sock_set_cork(struct sock *sk, bool on);
int tcp_sock_set_keepcnt(struct sock *sk, int val);
int tcp_sock_set_keepidle_locked(struct sock *sk, int val);
int tcp_sock_set_keepidle(struct sock *sk, int val);
int tcp_sock_set_keepintvl(struct sock *sk, int val);
void __tcp_sock_set_nodelay(struct sock *sk, bool on);
void tcp_sock_set_nodelay(struct sock *sk);
void tcp_sock_set_quickack(struct sock *sk, int val);
int tcp_sock_set_syncnt(struct sock *sk, int val);
int tcp_sock_set_user_timeout(struct sock *sk, int val);

#endif	
