#ifndef NETDEV_PCS_H
#define NETDEV_PCS_H

#include <linux/phy.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>

struct device_node;
struct ethtool_cmd;
struct fwnode_handle;
struct net_device;
struct phylink;

enum {
	MLO_PAUSE_NONE,
	MLO_PAUSE_RX = BIT(0),
	MLO_PAUSE_TX = BIT(1),
	MLO_PAUSE_TXRX_MASK = MLO_PAUSE_TX | MLO_PAUSE_RX,
	MLO_PAUSE_AN = BIT(2),

	MLO_AN_PHY = 0,	
	MLO_AN_FIXED,	
	MLO_AN_INBAND,	

	
	PHYLINK_PCS_NEG_NONE = 0,
	PHYLINK_PCS_NEG_ENABLED = BIT(4),
	PHYLINK_PCS_NEG_OUTBAND = BIT(5),
	PHYLINK_PCS_NEG_INBAND = BIT(6),
	PHYLINK_PCS_NEG_INBAND_DISABLED = PHYLINK_PCS_NEG_INBAND,
	PHYLINK_PCS_NEG_INBAND_ENABLED = PHYLINK_PCS_NEG_INBAND |
					 PHYLINK_PCS_NEG_ENABLED,

	
	MAC_SYM_PAUSE	= BIT(0),
	MAC_ASYM_PAUSE	= BIT(1),
	MAC_10HD	= BIT(2),
	MAC_10FD	= BIT(3),
	MAC_10		= MAC_10HD | MAC_10FD,
	MAC_100HD	= BIT(4),
	MAC_100FD	= BIT(5),
	MAC_100		= MAC_100HD | MAC_100FD,
	MAC_1000HD	= BIT(6),
	MAC_1000FD	= BIT(7),
	MAC_1000	= MAC_1000HD | MAC_1000FD,
	MAC_2500FD	= BIT(8),
	MAC_5000FD	= BIT(9),
	MAC_10000FD	= BIT(10),
	MAC_20000FD	= BIT(11),
	MAC_25000FD	= BIT(12),
	MAC_40000FD	= BIT(13),
	MAC_50000FD	= BIT(14),
	MAC_56000FD	= BIT(15),
	MAC_100000FD	= BIT(16),
	MAC_200000FD	= BIT(17),
	MAC_400000FD	= BIT(18),
};

static inline bool phylink_autoneg_inband(unsigned int mode)
{
	return mode == MLO_AN_INBAND;
}


static inline unsigned int phylink_pcs_neg_mode(unsigned int mode,
						phy_interface_t interface,
						const unsigned long *advertising)
{
	unsigned int neg_mode;

	switch (interface) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_QSGMII:
	case PHY_INTERFACE_MODE_QUSGMII:
	case PHY_INTERFACE_MODE_USXGMII:
		
		if (!phylink_autoneg_inband(mode))
			neg_mode = PHYLINK_PCS_NEG_OUTBAND;
		else
			neg_mode = PHYLINK_PCS_NEG_INBAND_ENABLED;
		break;

	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_2500BASEX:
		
		if (!phylink_autoneg_inband(mode))
			neg_mode = PHYLINK_PCS_NEG_OUTBAND;
		else if (linkmode_test_bit(ETHTOOL_LINK_MODE_Autoneg_BIT,
					   advertising))
			neg_mode = PHYLINK_PCS_NEG_INBAND_ENABLED;
		else
			neg_mode = PHYLINK_PCS_NEG_INBAND_DISABLED;
		break;

	default:
		neg_mode = PHYLINK_PCS_NEG_NONE;
		break;
	}

	return neg_mode;
}


struct phylink_link_state {
	__ETHTOOL_DECLARE_LINK_MODE_MASK(advertising);
	__ETHTOOL_DECLARE_LINK_MODE_MASK(lp_advertising);
	phy_interface_t interface;
	int speed;
	int duplex;
	int pause;
	int rate_matching;
	unsigned int link:1;
	unsigned int an_complete:1;
};

enum phylink_op_type {
	PHYLINK_NETDEV = 0,
	PHYLINK_DEV,
};


struct phylink_config {
	struct device *dev;
	enum phylink_op_type type;
	bool poll_fixed_state;
	bool mac_managed_pm;
	bool ovr_an_inband;
	void (*get_fixed_state)(struct phylink_config *config,
				struct phylink_link_state *state);
	DECLARE_PHY_INTERFACE_MASK(supported_interfaces);
	unsigned long mac_capabilities;
};

void phylink_limit_mac_speed(struct phylink_config *config, u32 max_speed);


struct phylink_mac_ops {
	void (*validate)(struct phylink_config *config,
			 unsigned long *supported,
			 struct phylink_link_state *state);
	struct phylink_pcs *(*mac_select_pcs)(struct phylink_config *config,
					      phy_interface_t interface);
	int (*mac_prepare)(struct phylink_config *config, unsigned int mode,
			   phy_interface_t iface);
	void (*mac_config)(struct phylink_config *config, unsigned int mode,
			   const struct phylink_link_state *state);
	int (*mac_finish)(struct phylink_config *config, unsigned int mode,
			  phy_interface_t iface);
	void (*mac_link_down)(struct phylink_config *config, unsigned int mode,
			      phy_interface_t interface);
	void (*mac_link_up)(struct phylink_config *config,
			    struct phy_device *phy, unsigned int mode,
			    phy_interface_t interface, int speed, int duplex,
			    bool tx_pause, bool rx_pause);
};

#if 0 

void validate(struct phylink_config *config, unsigned long *supported,
	      struct phylink_link_state *state);

struct phylink_pcs *mac_select_pcs(struct phylink_config *config,
				   phy_interface_t interface);


int mac_prepare(struct phylink_config *config, unsigned int mode,
		phy_interface_t iface);


void mac_config(struct phylink_config *config, unsigned int mode,
		const struct phylink_link_state *state);


int mac_finish(struct phylink_config *config, unsigned int mode,
		phy_interface_t iface);


void mac_link_down(struct phylink_config *config, unsigned int mode,
		   phy_interface_t interface);


void mac_link_up(struct phylink_config *config, struct phy_device *phy,
		 unsigned int mode, phy_interface_t interface,
		 int speed, int duplex, bool tx_pause, bool rx_pause);
#endif

struct phylink_pcs_ops;


struct phylink_pcs {
	const struct phylink_pcs_ops *ops;
	struct phylink *phylink;
	bool neg_mode;
	bool poll;
};


struct phylink_pcs_ops {
	int (*pcs_validate)(struct phylink_pcs *pcs, unsigned long *supported,
			    const struct phylink_link_state *state);
	int (*pcs_enable)(struct phylink_pcs *pcs);
	void (*pcs_disable)(struct phylink_pcs *pcs);
	void (*pcs_pre_config)(struct phylink_pcs *pcs,
			       phy_interface_t interface);
	int (*pcs_post_config)(struct phylink_pcs *pcs,
			       phy_interface_t interface);
	void (*pcs_get_state)(struct phylink_pcs *pcs,
			      struct phylink_link_state *state);
	int (*pcs_config)(struct phylink_pcs *pcs, unsigned int neg_mode,
			  phy_interface_t interface,
			  const unsigned long *advertising,
			  bool permit_pause_to_mac);
	void (*pcs_an_restart)(struct phylink_pcs *pcs);
	void (*pcs_link_up)(struct phylink_pcs *pcs, unsigned int neg_mode,
			    phy_interface_t interface, int speed, int duplex);
};

#if 0 

int pcs_validate(struct phylink_pcs *pcs, unsigned long *supported,
		 const struct phylink_link_state *state);


int pcs_enable(struct phylink_pcs *pcs);


void pcs_disable(struct phylink_pcs *pcs);


void pcs_get_state(struct phylink_pcs *pcs,
		   struct phylink_link_state *state);


int pcs_config(struct phylink_pcs *pcs, unsigned int neg_mode,
	       phy_interface_t interface, const unsigned long *advertising,
	       bool permit_pause_to_mac);


void pcs_an_restart(struct phylink_pcs *pcs);


void pcs_link_up(struct phylink_pcs *pcs, unsigned int neg_mode,
		 phy_interface_t interface, int speed, int duplex);
#endif

void phylink_caps_to_linkmodes(unsigned long *linkmodes, unsigned long caps);
unsigned long phylink_get_capabilities(phy_interface_t interface,
				       unsigned long mac_capabilities,
				       int rate_matching);
void phylink_validate_mask_caps(unsigned long *supported,
				struct phylink_link_state *state,
				unsigned long caps);
void phylink_generic_validate(struct phylink_config *config,
			      unsigned long *supported,
			      struct phylink_link_state *state);

struct phylink *phylink_create(struct phylink_config *,
			       const struct fwnode_handle *,
			       phy_interface_t,
			       const struct phylink_mac_ops *);
void phylink_destroy(struct phylink *);
bool phylink_expects_phy(struct phylink *pl);

int phylink_connect_phy(struct phylink *, struct phy_device *);
int phylink_of_phy_connect(struct phylink *, struct device_node *, u32 flags);
int phylink_fwnode_phy_connect(struct phylink *pl,
			       const struct fwnode_handle *fwnode,
			       u32 flags);
void phylink_disconnect_phy(struct phylink *);

void phylink_mac_change(struct phylink *, bool up);
void phylink_pcs_change(struct phylink_pcs *, bool up);

void phylink_start(struct phylink *);
void phylink_stop(struct phylink *);

void phylink_suspend(struct phylink *pl, bool mac_wol);
void phylink_resume(struct phylink *pl);

void phylink_ethtool_get_wol(struct phylink *, struct ethtool_wolinfo *);
int phylink_ethtool_set_wol(struct phylink *, struct ethtool_wolinfo *);

int phylink_ethtool_ksettings_get(struct phylink *,
				  struct ethtool_link_ksettings *);
int phylink_ethtool_ksettings_set(struct phylink *,
				  const struct ethtool_link_ksettings *);
int phylink_ethtool_nway_reset(struct phylink *);
void phylink_ethtool_get_pauseparam(struct phylink *,
				    struct ethtool_pauseparam *);
int phylink_ethtool_set_pauseparam(struct phylink *,
				   struct ethtool_pauseparam *);
int phylink_get_eee_err(struct phylink *);
int phylink_init_eee(struct phylink *, bool);
int phylink_ethtool_get_eee(struct phylink *, struct ethtool_eee *);
int phylink_ethtool_set_eee(struct phylink *, struct ethtool_eee *);
int phylink_mii_ioctl(struct phylink *, struct ifreq *, int);
int phylink_speed_down(struct phylink *pl, bool sync);
int phylink_speed_up(struct phylink *pl);

#define phylink_zero(bm) \
	bitmap_zero(bm, __ETHTOOL_LINK_MODE_MASK_NBITS)
#define __phylink_do_bit(op, bm, mode) \
	op(ETHTOOL_LINK_MODE_ ## mode ## _BIT, bm)

#define phylink_set(bm, mode)	__phylink_do_bit(__set_bit, bm, mode)
#define phylink_clear(bm, mode)	__phylink_do_bit(__clear_bit, bm, mode)
#define phylink_test(bm, mode)	__phylink_do_bit(test_bit, bm, mode)

void phylink_set_port_modes(unsigned long *bits);


static inline int phylink_get_link_timer_ns(phy_interface_t interface)
{
	switch (interface) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_QSGMII:
	case PHY_INTERFACE_MODE_USXGMII:
		return 1600000;

	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_2500BASEX:
		return 10000000;

	default:
		return -EINVAL;
	}
}

void phylink_mii_c22_pcs_decode_state(struct phylink_link_state *state,
				      u16 bmsr, u16 lpa);
void phylink_mii_c22_pcs_get_state(struct mdio_device *pcs,
				   struct phylink_link_state *state);
int phylink_mii_c22_pcs_encode_advertisement(phy_interface_t interface,
					     const unsigned long *advertising);
int phylink_mii_c22_pcs_config(struct mdio_device *pcs,
			       phy_interface_t interface,
			       const unsigned long *advertising,
			       unsigned int neg_mode);
void phylink_mii_c22_pcs_an_restart(struct mdio_device *pcs);

void phylink_resolve_c73(struct phylink_link_state *state);

void phylink_mii_c45_pcs_get_state(struct mdio_device *pcs,
				   struct phylink_link_state *state);

void phylink_decode_usxgmii_word(struct phylink_link_state *state,
				 uint16_t lpa);
#endif
