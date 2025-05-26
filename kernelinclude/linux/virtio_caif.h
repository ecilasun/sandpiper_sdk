

#ifndef VIRTIO_CAIF_H
#define VIRTIO_CAIF_H

#include <linux/types.h>
struct virtio_caif_transf_config {
	__virtio16 headroom;
	__virtio16 tailroom;
	__virtio32 mtu;
	u8 reserved[4];
};

struct virtio_caif_config {
	struct virtio_caif_transf_config uplink, downlink;
	u8 reserved[8];
};
#endif
