/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __USB_TYPEC_DP_H
#define __USB_TYPEC_DP_H

#include <linux/usb/typec_altmode.h>

#define USB_TYPEC_DP_SID	0xff01

#define USB_TYPEC_NVIDIA_VLINK_SID	0x955	
#define USB_TYPEC_DP_MODE	1


enum {
	TYPEC_DP_STATE_A = TYPEC_STATE_MODAL,	
	TYPEC_DP_STATE_B,			
	TYPEC_DP_STATE_C,
	TYPEC_DP_STATE_D,
	TYPEC_DP_STATE_E,
	TYPEC_DP_STATE_F,			
};


struct typec_displayport_data {
	u32 status;
	u32 conf;
};

enum {
	DP_PIN_ASSIGN_A, 
	DP_PIN_ASSIGN_B, 
	DP_PIN_ASSIGN_C,
	DP_PIN_ASSIGN_D,
	DP_PIN_ASSIGN_E,
	DP_PIN_ASSIGN_F, 
};


#define DP_CMD_STATUS_UPDATE		VDO_CMD_VENDOR(0)
#define DP_CMD_CONFIGURE		VDO_CMD_VENDOR(1)


#define DP_CAP_CAPABILITY(_cap_)	((_cap_) & 3)
#define   DP_CAP_UFP_D			1
#define   DP_CAP_DFP_D			2
#define   DP_CAP_DFP_D_AND_UFP_D	3
#define DP_CAP_DP_SIGNALING		BIT(2) 
#define DP_CAP_GEN2			BIT(3) 
#define DP_CAP_RECEPTACLE		BIT(6)
#define DP_CAP_USB			BIT(7)
#define DP_CAP_DFP_D_PIN_ASSIGN(_cap_)	(((_cap_) & GENMASK(15, 8)) >> 8)
#define DP_CAP_UFP_D_PIN_ASSIGN(_cap_)	(((_cap_) & GENMASK(23, 16)) >> 16)

#define DP_CAP_PIN_ASSIGN_UFP_D(_cap_) ((_cap_ & DP_CAP_RECEPTACLE) ? \
			DP_CAP_UFP_D_PIN_ASSIGN(_cap_) : DP_CAP_DFP_D_PIN_ASSIGN(_cap_))
#define DP_CAP_PIN_ASSIGN_DFP_D(_cap_) ((_cap_ & DP_CAP_RECEPTACLE) ? \
			DP_CAP_DFP_D_PIN_ASSIGN(_cap_) : DP_CAP_UFP_D_PIN_ASSIGN(_cap_))


#define DP_STATUS_CONNECTION(_status_)	((_status_) & 3)
#define   DP_STATUS_CON_DISABLED	0
#define   DP_STATUS_CON_DFP_D		1
#define   DP_STATUS_CON_UFP_D		2
#define   DP_STATUS_CON_BOTH		3
#define DP_STATUS_POWER_LOW		BIT(2)
#define DP_STATUS_ENABLED		BIT(3)
#define DP_STATUS_PREFER_MULTI_FUNC	BIT(4)
#define DP_STATUS_SWITCH_TO_USB		BIT(5)
#define DP_STATUS_EXIT_DP_MODE		BIT(6)
#define DP_STATUS_HPD_STATE		BIT(7) 
#define DP_STATUS_IRQ_HPD		BIT(8)


#define DP_CONF_CURRENTLY(_conf_)	((_conf_) & 3)
#define DP_CONF_UFP_U_AS_DFP_D		BIT(0)
#define DP_CONF_UFP_U_AS_UFP_D		BIT(1)
#define DP_CONF_SIGNALING_DP		BIT(2)
#define DP_CONF_SIGNALING_GEN_2		BIT(3) 
#define DP_CONF_PIN_ASSIGNEMENT_SHIFT	8
#define DP_CONF_PIN_ASSIGNEMENT_MASK	GENMASK(15, 8)


#define DP_CONF_SET_PIN_ASSIGN(_a_)	((_a_) << 8)
#define DP_CONF_GET_PIN_ASSIGN(_conf_)	(((_conf_) & GENMASK(15, 8)) >> 8)

#endif 
