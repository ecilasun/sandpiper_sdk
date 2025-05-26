/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _UFS_QUIRKS_H_
#define _UFS_QUIRKS_H_


#define STR_PRFX_EQUAL(s1, s2) !strncmp(s1, s2, strlen(s1))

#define UFS_ANY_VENDOR 0xFFFF
#define UFS_ANY_MODEL  "ANY_MODEL"

#define UFS_VENDOR_MICRON      0x12C
#define UFS_VENDOR_SAMSUNG     0x1CE
#define UFS_VENDOR_SKHYNIX     0x1AD
#define UFS_VENDOR_TOSHIBA     0x198
#define UFS_VENDOR_WDC         0x145


struct ufs_dev_quirk {
	u16 wmanufacturerid;
	const u8 *model;
	unsigned int quirk;
};


#define UFS_DEVICE_QUIRK_RECOVERY_FROM_DL_NAC_ERRORS (1 << 2)


#define UFS_DEVICE_QUIRK_PA_TACTIVATE	(1 << 4)


#define UFS_DEVICE_QUIRK_DELAY_BEFORE_LPM	(1 << 6)


#define UFS_DEVICE_QUIRK_HOST_PA_TACTIVATE	(1 << 7)


#define UFS_DEVICE_QUIRK_HOST_PA_SAVECONFIGTIME	(1 << 8)


#define UFS_DEVICE_QUIRK_HOST_VS_DEBUGSAVECONFIGTIME	(1 << 9)


#define UFS_DEVICE_QUIRK_SUPPORT_EXTENDED_FEATURES (1 << 10)


#define UFS_DEVICE_QUIRK_DELAY_AFTER_LPM        (1 << 11)

#endif 
