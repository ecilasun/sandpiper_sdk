/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __LINUX_USB_TCPM_H
#define __LINUX_USB_TCPM_H

#include <linux/bitops.h>
#include <linux/usb/typec.h>
#include "pd.h"

enum typec_cc_status {
	TYPEC_CC_OPEN,
	TYPEC_CC_RA,
	TYPEC_CC_RD,
	TYPEC_CC_RP_DEF,
	TYPEC_CC_RP_1_5,
	TYPEC_CC_RP_3_0,
};


#define SINK_TX_NG	TYPEC_CC_RP_1_5
#define SINK_TX_OK	TYPEC_CC_RP_3_0

enum typec_cc_polarity {
	TYPEC_POLARITY_CC1,
	TYPEC_POLARITY_CC2,
};


#define PD_T_TCPC_TX_TIMEOUT	100		
#define PD_ROLE_SWAP_TIMEOUT	(MSEC_PER_SEC * 10)
#define PD_PPS_CTRL_TIMEOUT	(MSEC_PER_SEC * 10)

enum tcpm_transmit_status {
	TCPC_TX_SUCCESS = 0,
	TCPC_TX_DISCARDED = 1,
	TCPC_TX_FAILED = 2,
};

enum tcpm_transmit_type {
	TCPC_TX_SOP = 0,
	TCPC_TX_SOP_PRIME = 1,
	TCPC_TX_SOP_PRIME_PRIME = 2,
	TCPC_TX_SOP_DEBUG_PRIME = 3,
	TCPC_TX_SOP_DEBUG_PRIME_PRIME = 4,
	TCPC_TX_HARD_RESET = 5,
	TCPC_TX_CABLE_RESET = 6,
	TCPC_TX_BIST_MODE_2 = 7
};


#define TCPC_MUX_USB_ENABLED		BIT(0)	
#define TCPC_MUX_DP_ENABLED		BIT(1)	
#define TCPC_MUX_POLARITY_INVERTED	BIT(2)	


struct tcpc_dev {
	struct fwnode_handle *fwnode;

	int (*init)(struct tcpc_dev *dev);
	int (*get_vbus)(struct tcpc_dev *dev);
	int (*get_current_limit)(struct tcpc_dev *dev);
	int (*set_cc)(struct tcpc_dev *dev, enum typec_cc_status cc);
	int (*apply_rc)(struct tcpc_dev *dev, enum typec_cc_status cc,
			enum typec_cc_polarity polarity);
	int (*get_cc)(struct tcpc_dev *dev, enum typec_cc_status *cc1,
		      enum typec_cc_status *cc2);
	int (*set_polarity)(struct tcpc_dev *dev,
			    enum typec_cc_polarity polarity);
	int (*set_vconn)(struct tcpc_dev *dev, bool on);
	int (*set_vbus)(struct tcpc_dev *dev, bool on, bool charge);
	int (*set_current_limit)(struct tcpc_dev *dev, u32 max_ma, u32 mv);
	int (*set_pd_rx)(struct tcpc_dev *dev, bool on);
	int (*set_roles)(struct tcpc_dev *dev, bool attached,
			 enum typec_role role, enum typec_data_role data);
	int (*start_toggling)(struct tcpc_dev *dev,
			      enum typec_port_type port_type,
			      enum typec_cc_status cc);
	int (*try_role)(struct tcpc_dev *dev, int role);
	int (*pd_transmit)(struct tcpc_dev *dev, enum tcpm_transmit_type type,
			   const struct pd_message *msg, unsigned int negotiated_rev);
	int (*set_bist_data)(struct tcpc_dev *dev, bool on);
	int (*enable_frs)(struct tcpc_dev *dev, bool enable);
	void (*frs_sourcing_vbus)(struct tcpc_dev *dev);
	int (*enable_auto_vbus_discharge)(struct tcpc_dev *dev, bool enable);
	int (*set_auto_vbus_discharge_threshold)(struct tcpc_dev *dev, enum typec_pwr_opmode mode,
						 bool pps_active, u32 requested_vbus_voltage);
	bool (*is_vbus_vsafe0v)(struct tcpc_dev *dev);
	void (*set_partner_usb_comm_capable)(struct tcpc_dev *dev, bool enable);
	void (*check_contaminant)(struct tcpc_dev *dev);
};

struct tcpm_port;

struct tcpm_port *tcpm_register_port(struct device *dev, struct tcpc_dev *tcpc);
void tcpm_unregister_port(struct tcpm_port *port);

void tcpm_vbus_change(struct tcpm_port *port);
void tcpm_cc_change(struct tcpm_port *port);
void tcpm_sink_frs(struct tcpm_port *port);
void tcpm_sourcing_vbus(struct tcpm_port *port);
void tcpm_pd_receive(struct tcpm_port *port,
		     const struct pd_message *msg);
void tcpm_pd_transmit_complete(struct tcpm_port *port,
			       enum tcpm_transmit_status status);
void tcpm_pd_hard_reset(struct tcpm_port *port);
void tcpm_tcpc_reset(struct tcpm_port *port);
void tcpm_port_clean(struct tcpm_port *port);
bool tcpm_port_is_toggling(struct tcpm_port *port);

#endif 
