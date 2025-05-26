

#ifndef _DVB_NET_H_
#define _DVB_NET_H_

#include <linux/module.h>

#include <media/dvbdev.h>

struct net_device;

#define DVB_NET_DEVICES_MAX 10

#ifdef CONFIG_DVB_NET



struct dvb_net {
	struct dvb_device *dvbdev;
	struct net_device *device[DVB_NET_DEVICES_MAX];
	int state[DVB_NET_DEVICES_MAX];
	unsigned int exit:1;
	struct dmx_demux *demux;
	struct mutex ioctl_mutex;
	struct mutex remove_mutex;
};


int dvb_net_init(struct dvb_adapter *adap, struct dvb_net *dvbnet,
		  struct dmx_demux *dmxdemux);


void dvb_net_release(struct dvb_net *dvbnet);

#else

struct dvb_net {
	struct dvb_device *dvbdev;
};

static inline void dvb_net_release(struct dvb_net *dvbnet)
{
}

static inline int dvb_net_init(struct dvb_adapter *adap,
			       struct dvb_net *dvbnet, struct dmx_demux *dmx)
{
	return 0;
}

#endif 

#endif
