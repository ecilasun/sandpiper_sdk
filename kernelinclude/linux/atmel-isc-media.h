/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_ATMEL_ISC_MEDIA_H__
#define __LINUX_ATMEL_ISC_MEDIA_H__



enum atmel_isc_ctrl_id {
	
	ISC_CID_R_GAIN = (V4L2_CID_USER_ATMEL_ISC_BASE + 0),
	
	ISC_CID_B_GAIN,
	
	ISC_CID_GR_GAIN,
	
	ISC_CID_GB_GAIN,
	
	ISC_CID_R_OFFSET,
	
	ISC_CID_B_OFFSET,
	
	ISC_CID_GR_OFFSET,
	
	ISC_CID_GB_OFFSET,
};

#endif
