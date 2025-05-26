

#ifndef _DVB_DEMUX_H_
#define _DVB_DEMUX_H_

#include <linux/time.h>
#include <linux/timer.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>

#include <media/demux.h>


enum dvb_dmx_filter_type {
	DMX_TYPE_TS,
	DMX_TYPE_SEC,
};


enum dvb_dmx_state {
	DMX_STATE_FREE,
	DMX_STATE_ALLOCATED,
	DMX_STATE_READY,
	DMX_STATE_GO,
};

#define DVB_DEMUX_MASK_MAX 18

#define MAX_PID 0x1fff

#define SPEED_PKTS_INTERVAL 50000



struct dvb_demux_filter {
	struct dmx_section_filter filter;
	u8 maskandmode[DMX_MAX_FILTER_SIZE];
	u8 maskandnotmode[DMX_MAX_FILTER_SIZE];
	bool doneq;

	struct dvb_demux_filter *next;
	struct dvb_demux_feed *feed;
	int index;
	enum dvb_dmx_state state;
	enum dvb_dmx_filter_type type;

	
	u16 hw_handle;
};


struct dvb_demux_feed {
	union {
		struct dmx_ts_feed ts;
		struct dmx_section_feed sec;
	} feed;

	union {
		dmx_ts_cb ts;
		dmx_section_cb sec;
	} cb;

	struct dvb_demux *demux;
	void *priv;
	enum dvb_dmx_filter_type type;
	enum dvb_dmx_state state;
	u16 pid;

	ktime_t timeout;
	struct dvb_demux_filter *filter;

	u32 buffer_flags;

	enum ts_filter_type ts_type;
	enum dmx_ts_pes pes_type;

	int cc;
	bool pusi_seen;

	u16 peslen;

	struct list_head list_head;
	unsigned int index;
};


struct dvb_demux {
	struct dmx_demux dmx;
	void *priv;
	int filternum;
	int feednum;
	int (*start_feed)(struct dvb_demux_feed *feed);
	int (*stop_feed)(struct dvb_demux_feed *feed);
	int (*write_to_decoder)(struct dvb_demux_feed *feed,
				 const u8 *buf, size_t len);
	u32 (*check_crc32)(struct dvb_demux_feed *feed,
			    const u8 *buf, size_t len);
	void (*memcopy)(struct dvb_demux_feed *feed, u8 *dst,
			 const u8 *src, size_t len);

	int users;
#define MAX_DVB_DEMUX_USERS 10
	struct dvb_demux_filter *filter;
	struct dvb_demux_feed *feed;

	struct list_head frontend_list;

	struct dvb_demux_feed *pesfilter[DMX_PES_OTHER];
	u16 pids[DMX_PES_OTHER];

#define DMX_MAX_PID 0x2000
	struct list_head feed_list;
	u8 tsbuf[204];
	int tsbufp;

	struct mutex mutex;
	spinlock_t lock;

	uint8_t *cnt_storage; 

	ktime_t speed_last_time; 
	uint32_t speed_pkts_cnt; 

	
	int playing;
	int recording;
};


int dvb_dmx_init(struct dvb_demux *demux);


void dvb_dmx_release(struct dvb_demux *demux);


void dvb_dmx_swfilter_packets(struct dvb_demux *demux, const u8 *buf,
			      size_t count);


void dvb_dmx_swfilter(struct dvb_demux *demux, const u8 *buf, size_t count);


void dvb_dmx_swfilter_204(struct dvb_demux *demux, const u8 *buf,
			  size_t count);


void dvb_dmx_swfilter_raw(struct dvb_demux *demux, const u8 *buf,
			  size_t count);

#endif 
