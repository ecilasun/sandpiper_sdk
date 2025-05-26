/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _V4L2_FWNODE_H
#define _V4L2_FWNODE_H

#include <linux/errno.h>
#include <linux/fwnode.h>
#include <linux/list.h>
#include <linux/types.h>

#include <media/v4l2-mediabus.h>


struct v4l2_fwnode_endpoint {
	struct fwnode_endpoint base;
	enum v4l2_mbus_type bus_type;
	struct {
		struct v4l2_mbus_config_parallel parallel;
		struct v4l2_mbus_config_mipi_csi1 mipi_csi1;
		struct v4l2_mbus_config_mipi_csi2 mipi_csi2;
	} bus;
	u64 *link_frequencies;
	unsigned int nr_of_link_frequencies;
};


#define V4L2_FWNODE_PROPERTY_UNSET   (-1U)


enum v4l2_fwnode_orientation {
	V4L2_FWNODE_ORIENTATION_FRONT,
	V4L2_FWNODE_ORIENTATION_BACK,
	V4L2_FWNODE_ORIENTATION_EXTERNAL
};


struct v4l2_fwnode_device_properties {
	enum v4l2_fwnode_orientation orientation;
	unsigned int rotation;
};


struct v4l2_fwnode_link {
	struct fwnode_handle *local_node;
	unsigned int local_port;
	unsigned int local_id;
	struct fwnode_handle *remote_node;
	unsigned int remote_port;
	unsigned int remote_id;
};


enum v4l2_connector_type {
	V4L2_CONN_UNKNOWN,
	V4L2_CONN_COMPOSITE,
	V4L2_CONN_SVIDEO,
};


struct v4l2_connector_link {
	struct list_head head;
	struct v4l2_fwnode_link fwnode_link;
};


struct v4l2_fwnode_connector_analog {
	v4l2_std_id sdtv_stds;
};


struct v4l2_fwnode_connector {
	const char *name;
	const char *label;
	enum v4l2_connector_type type;
	struct list_head links;
	unsigned int nr_of_links;

	union {
		struct v4l2_fwnode_connector_analog analog;
		
	} connector;
};


enum v4l2_fwnode_bus_type {
	V4L2_FWNODE_BUS_TYPE_GUESS = 0,
	V4L2_FWNODE_BUS_TYPE_CSI2_CPHY,
	V4L2_FWNODE_BUS_TYPE_CSI1,
	V4L2_FWNODE_BUS_TYPE_CCP2,
	V4L2_FWNODE_BUS_TYPE_CSI2_DPHY,
	V4L2_FWNODE_BUS_TYPE_PARALLEL,
	V4L2_FWNODE_BUS_TYPE_BT656,
	V4L2_FWNODE_BUS_TYPE_DPI,
	NR_OF_V4L2_FWNODE_BUS_TYPE
};


int v4l2_fwnode_endpoint_parse(struct fwnode_handle *fwnode,
			       struct v4l2_fwnode_endpoint *vep);


void v4l2_fwnode_endpoint_free(struct v4l2_fwnode_endpoint *vep);


int v4l2_fwnode_endpoint_alloc_parse(struct fwnode_handle *fwnode,
				     struct v4l2_fwnode_endpoint *vep);


int v4l2_fwnode_parse_link(struct fwnode_handle *fwnode,
			   struct v4l2_fwnode_link *link);


void v4l2_fwnode_put_link(struct v4l2_fwnode_link *link);


void v4l2_fwnode_connector_free(struct v4l2_fwnode_connector *connector);


int v4l2_fwnode_connector_parse(struct fwnode_handle *fwnode,
				struct v4l2_fwnode_connector *connector);


int v4l2_fwnode_connector_add_link(struct fwnode_handle *fwnode,
				   struct v4l2_fwnode_connector *connector);


int v4l2_fwnode_device_parse(struct device *dev,
			     struct v4l2_fwnode_device_properties *props);




#define v4l2_connector_first_link(v4l2c)				       \
	list_first_entry_or_null(&(v4l2c)->links,			       \
				 struct v4l2_connector_link, head)


#define v4l2_connector_last_link(v4l2c)					       \
	list_last_entry(&(v4l2c)->links, struct v4l2_connector_link, head)

#endif 
