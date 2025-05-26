/* SPDX-License-Identifier: GPL-2.0 */


#ifndef I3C_CCC_H
#define I3C_CCC_H

#include <linux/bitops.h>
#include <linux/i3c/device.h>


#define I3C_CCC_DIRECT			BIT(7)

#define I3C_CCC_ID(id, broadcast)	\
	((id) | ((broadcast) ? 0 : I3C_CCC_DIRECT))


#define I3C_CCC_ENEC(broadcast)		I3C_CCC_ID(0x0, broadcast)
#define I3C_CCC_DISEC(broadcast)	I3C_CCC_ID(0x1, broadcast)
#define I3C_CCC_ENTAS(as, broadcast)	I3C_CCC_ID(0x2 + (as), broadcast)
#define I3C_CCC_RSTDAA(broadcast)	I3C_CCC_ID(0x6, broadcast)
#define I3C_CCC_SETMWL(broadcast)	I3C_CCC_ID(0x9, broadcast)
#define I3C_CCC_SETMRL(broadcast)	I3C_CCC_ID(0xa, broadcast)
#define I3C_CCC_SETXTIME(broadcast)	((broadcast) ? 0x28 : 0x98)
#define I3C_CCC_VENDOR(id, broadcast)	((id) + ((broadcast) ? 0x61 : 0xe0))


#define I3C_CCC_ENTDAA			I3C_CCC_ID(0x7, true)
#define I3C_CCC_DEFSLVS			I3C_CCC_ID(0x8, true)
#define I3C_CCC_ENTTM			I3C_CCC_ID(0xb, true)
#define I3C_CCC_ENTHDR(x)		I3C_CCC_ID(0x20 + (x), true)


#define I3C_CCC_SETDASA			I3C_CCC_ID(0x7, false)
#define I3C_CCC_SETNEWDA		I3C_CCC_ID(0x8, false)
#define I3C_CCC_GETMWL			I3C_CCC_ID(0xb, false)
#define I3C_CCC_GETMRL			I3C_CCC_ID(0xc, false)
#define I3C_CCC_GETPID			I3C_CCC_ID(0xd, false)
#define I3C_CCC_GETBCR			I3C_CCC_ID(0xe, false)
#define I3C_CCC_GETDCR			I3C_CCC_ID(0xf, false)
#define I3C_CCC_GETSTATUS		I3C_CCC_ID(0x10, false)
#define I3C_CCC_GETACCMST		I3C_CCC_ID(0x11, false)
#define I3C_CCC_SETBRGTGT		I3C_CCC_ID(0x13, false)
#define I3C_CCC_GETMXDS			I3C_CCC_ID(0x14, false)
#define I3C_CCC_GETHDRCAP		I3C_CCC_ID(0x15, false)
#define I3C_CCC_GETXTIME		I3C_CCC_ID(0x19, false)

#define I3C_CCC_EVENT_SIR		BIT(0)
#define I3C_CCC_EVENT_MR		BIT(1)
#define I3C_CCC_EVENT_HJ		BIT(3)


struct i3c_ccc_events {
	u8 events;
};


struct i3c_ccc_mwl {
	__be16 len;
};


struct i3c_ccc_mrl {
	__be16 read_len;
	u8 ibi_len;
} __packed;


struct i3c_ccc_dev_desc {
	u8 dyn_addr;
	union {
		u8 dcr;
		u8 lvr;
	};
	u8 bcr;
	u8 static_addr;
};


struct i3c_ccc_defslvs {
	u8 count;
	struct i3c_ccc_dev_desc master;
	struct i3c_ccc_dev_desc slaves[];
} __packed;


enum i3c_ccc_test_mode {
	I3C_CCC_EXIT_TEST_MODE,
	I3C_CCC_VENDOR_TEST_MODE,
};


struct i3c_ccc_enttm {
	u8 mode;
};


struct i3c_ccc_setda {
	u8 addr;
};


struct i3c_ccc_getpid {
	u8 pid[6];
};


struct i3c_ccc_getbcr {
	u8 bcr;
};


struct i3c_ccc_getdcr {
	u8 dcr;
};

#define I3C_CCC_STATUS_PENDING_INT(status)	((status) & GENMASK(3, 0))
#define I3C_CCC_STATUS_PROTOCOL_ERROR		BIT(5)
#define I3C_CCC_STATUS_ACTIVITY_MODE(status)	\
	(((status) & GENMASK(7, 6)) >> 6)


struct i3c_ccc_getstatus {
	__be16 status;
};


struct i3c_ccc_getaccmst {
	u8 newmaster;
};


struct i3c_ccc_bridged_slave_desc {
	u8 addr;
	__be16 id;
} __packed;


struct i3c_ccc_setbrgtgt {
	u8 count;
	struct i3c_ccc_bridged_slave_desc bslaves[];
} __packed;


enum i3c_sdr_max_data_rate {
	I3C_SDR0_FSCL_MAX,
	I3C_SDR1_FSCL_8MHZ,
	I3C_SDR2_FSCL_6MHZ,
	I3C_SDR3_FSCL_4MHZ,
	I3C_SDR4_FSCL_2MHZ,
};


enum i3c_tsco {
	I3C_TSCO_8NS,
	I3C_TSCO_9NS,
	I3C_TSCO_10NS,
	I3C_TSCO_11NS,
	I3C_TSCO_12NS,
};

#define I3C_CCC_MAX_SDR_FSCL_MASK	GENMASK(2, 0)
#define I3C_CCC_MAX_SDR_FSCL(x)		((x) & I3C_CCC_MAX_SDR_FSCL_MASK)


struct i3c_ccc_getmxds {
	u8 maxwr;
	u8 maxrd;
	u8 maxrdturn[3];
} __packed;

#define I3C_CCC_HDR_MODE(mode)		BIT(mode)


struct i3c_ccc_gethdrcap {
	u8 modes;
} __packed;


enum i3c_ccc_setxtime_subcmd {
	I3C_CCC_SETXTIME_ST = 0x7f,
	I3C_CCC_SETXTIME_DT = 0xbf,
	I3C_CCC_SETXTIME_ENTER_ASYNC_MODE0 = 0xdf,
	I3C_CCC_SETXTIME_ENTER_ASYNC_MODE1 = 0xef,
	I3C_CCC_SETXTIME_ENTER_ASYNC_MODE2 = 0xf7,
	I3C_CCC_SETXTIME_ENTER_ASYNC_MODE3 = 0xfb,
	I3C_CCC_SETXTIME_ASYNC_TRIGGER = 0xfd,
	I3C_CCC_SETXTIME_TPH = 0x3f,
	I3C_CCC_SETXTIME_TU = 0x9f,
	I3C_CCC_SETXTIME_ODR = 0x8f,
};


struct i3c_ccc_setxtime {
	u8 subcmd;
	u8 data[];
} __packed;

#define I3C_CCC_GETXTIME_SYNC_MODE	BIT(0)
#define I3C_CCC_GETXTIME_ASYNC_MODE(x)	BIT((x) + 1)
#define I3C_CCC_GETXTIME_OVERFLOW	BIT(7)


struct i3c_ccc_getxtime {
	u8 supported_modes;
	u8 state;
	u8 frequency;
	u8 inaccuracy;
} __packed;


struct i3c_ccc_cmd_payload {
	u16 len;
	void *data;
};


struct i3c_ccc_cmd_dest {
	u8 addr;
	struct i3c_ccc_cmd_payload payload;
};


struct i3c_ccc_cmd {
	u8 rnw;
	u8 id;
	unsigned int ndests;
	struct i3c_ccc_cmd_dest *dests;
	enum i3c_error_code err;
};

#endif 
