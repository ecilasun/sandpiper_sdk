

#ifndef _DMXDEV_H_
#define _DMXDEV_H_

#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/slab.h>

#include <linux/dvb/dmx.h>

#include <media/dvbdev.h>
#include <media/demux.h>
#include <media/dvb_ringbuffer.h>
#include <media/dvb_vb2.h>


enum dmxdev_type {
	DMXDEV_TYPE_NONE,
	DMXDEV_TYPE_SEC,
	DMXDEV_TYPE_PES,
};


enum dmxdev_state {
	DMXDEV_STATE_FREE,
	DMXDEV_STATE_ALLOCATED,
	DMXDEV_STATE_SET,
	DMXDEV_STATE_GO,
	DMXDEV_STATE_DONE,
	DMXDEV_STATE_TIMEDOUT
};



struct dmxdev_feed {
	u16 pid;
	struct dmx_ts_feed *ts;
	struct list_head next;
};


struct dmxdev_filter {
	union {
		struct dmx_section_filter *sec;
	} filter;

	union {
		
		struct list_head ts;
		struct dmx_section_feed *sec;
	} feed;

	union {
		struct dmx_sct_filter_params sec;
		struct dmx_pes_filter_params pes;
	} params;

	enum dmxdev_type type;
	enum dmxdev_state state;
	struct dmxdev *dev;
	struct dvb_ringbuffer buffer;
	struct dvb_vb2_ctx vb2_ctx;

	struct mutex mutex;

	
	struct timer_list timer;
	int todo;
	u8 secheader[3];
};


struct dmxdev {
	struct dvb_device *dvbdev;
	struct dvb_device *dvr_dvbdev;

	struct dmxdev_filter *filter;
	struct dmx_demux *demux;

	int filternum;
	int capabilities;

	unsigned int may_do_mmap:1;
	unsigned int exit:1;
#define DMXDEV_CAP_DUPLEX 1
	struct dmx_frontend *dvr_orig_fe;

	struct dvb_ringbuffer dvr_buffer;
#define DVR_BUFFER_SIZE (10*188*1024)

	struct dvb_vb2_ctx dvr_vb2_ctx;

	struct mutex mutex;
	spinlock_t lock;
};


int dvb_dmxdev_init(struct dmxdev *dmxdev, struct dvb_adapter *adap);


void dvb_dmxdev_release(struct dmxdev *dmxdev);

#endif 
