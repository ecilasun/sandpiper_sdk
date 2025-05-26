/* SPDX-License-Identifier: GPL-2.0 */


#ifndef K3_EVENT_ROUTER_
#define K3_EVENT_ROUTER_

#include <linux/types.h>

struct k3_event_route_data {
	void *priv;
	int (*set_event)(void *priv, u32 event);
};

#endif 
