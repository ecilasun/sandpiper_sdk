/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __LINUX_MII_H__
#define __LINUX_MII_H__


#include <linux/if.h>
#include <linux/linkmode.h>
#include <uapi/linux/mii.h>

struct ethtool_cmd;

struct mii_if_info {
	int phy_id;
	int advertising;
	int phy_id_mask;
	int reg_num_mask;

	unsigned int full_duplex : 1;	
	unsigned int force_media : 1;	
	unsigned int supports_gmii : 1; 

	struct net_device *dev;
	int (*mdio_read) (struct net_device *dev, int phy_id, int location);
	void (*mdio_write) (struct net_device *dev, int phy_id, int location, int val);
};

extern int mii_link_ok (struct mii_if_info *mii);
extern int mii_nway_restart (struct mii_if_info *mii);
extern void mii_ethtool_gset(struct mii_if_info *mii, struct ethtool_cmd *ecmd);
extern void mii_ethtool_get_link_ksettings(
	struct mii_if_info *mii, struct ethtool_link_ksettings *cmd);
extern int mii_ethtool_sset(struct mii_if_info *mii, struct ethtool_cmd *ecmd);
extern int mii_ethtool_set_link_ksettings(
	struct mii_if_info *mii, const struct ethtool_link_ksettings *cmd);
extern int mii_check_gmii_support(struct mii_if_info *mii);
extern void mii_check_link (struct mii_if_info *mii);
extern unsigned int mii_check_media (struct mii_if_info *mii,
				     unsigned int ok_to_print,
				     unsigned int init_media);
extern int generic_mii_ioctl(struct mii_if_info *mii_if,
			     struct mii_ioctl_data *mii_data, int cmd,
			     unsigned int *duplex_changed);


static inline struct mii_ioctl_data *if_mii(struct ifreq *rq)
{
	return (struct mii_ioctl_data *) &rq->ifr_ifru;
}


static inline unsigned int mii_nway_result (unsigned int negotiated)
{
	unsigned int ret;

	if (negotiated & LPA_100FULL)
		ret = LPA_100FULL;
	else if (negotiated & LPA_100BASE4)
		ret = LPA_100BASE4;
	else if (negotiated & LPA_100HALF)
		ret = LPA_100HALF;
	else if (negotiated & LPA_10FULL)
		ret = LPA_10FULL;
	else
		ret = LPA_10HALF;

	return ret;
}


static inline unsigned int mii_duplex (unsigned int duplex_lock,
				       unsigned int negotiated)
{
	if (duplex_lock)
		return 1;
	if (mii_nway_result(negotiated) & LPA_DUPLEX)
		return 1;
	return 0;
}


static inline u32 ethtool_adv_to_mii_adv_t(u32 ethadv)
{
	u32 result = 0;

	if (ethadv & ADVERTISED_10baseT_Half)
		result |= ADVERTISE_10HALF;
	if (ethadv & ADVERTISED_10baseT_Full)
		result |= ADVERTISE_10FULL;
	if (ethadv & ADVERTISED_100baseT_Half)
		result |= ADVERTISE_100HALF;
	if (ethadv & ADVERTISED_100baseT_Full)
		result |= ADVERTISE_100FULL;
	if (ethadv & ADVERTISED_Pause)
		result |= ADVERTISE_PAUSE_CAP;
	if (ethadv & ADVERTISED_Asym_Pause)
		result |= ADVERTISE_PAUSE_ASYM;

	return result;
}


static inline u32 linkmode_adv_to_mii_adv_t(unsigned long *advertising)
{
	u32 result = 0;

	if (linkmode_test_bit(ETHTOOL_LINK_MODE_10baseT_Half_BIT, advertising))
		result |= ADVERTISE_10HALF;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_10baseT_Full_BIT, advertising))
		result |= ADVERTISE_10FULL;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_100baseT_Half_BIT, advertising))
		result |= ADVERTISE_100HALF;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_100baseT_Full_BIT, advertising))
		result |= ADVERTISE_100FULL;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_Pause_BIT, advertising))
		result |= ADVERTISE_PAUSE_CAP;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_Asym_Pause_BIT, advertising))
		result |= ADVERTISE_PAUSE_ASYM;

	return result;
}


static inline u32 mii_adv_to_ethtool_adv_t(u32 adv)
{
	u32 result = 0;

	if (adv & ADVERTISE_10HALF)
		result |= ADVERTISED_10baseT_Half;
	if (adv & ADVERTISE_10FULL)
		result |= ADVERTISED_10baseT_Full;
	if (adv & ADVERTISE_100HALF)
		result |= ADVERTISED_100baseT_Half;
	if (adv & ADVERTISE_100FULL)
		result |= ADVERTISED_100baseT_Full;
	if (adv & ADVERTISE_PAUSE_CAP)
		result |= ADVERTISED_Pause;
	if (adv & ADVERTISE_PAUSE_ASYM)
		result |= ADVERTISED_Asym_Pause;

	return result;
}


static inline u32 ethtool_adv_to_mii_ctrl1000_t(u32 ethadv)
{
	u32 result = 0;

	if (ethadv & ADVERTISED_1000baseT_Half)
		result |= ADVERTISE_1000HALF;
	if (ethadv & ADVERTISED_1000baseT_Full)
		result |= ADVERTISE_1000FULL;

	return result;
}


static inline u32 linkmode_adv_to_mii_ctrl1000_t(unsigned long *advertising)
{
	u32 result = 0;

	if (linkmode_test_bit(ETHTOOL_LINK_MODE_1000baseT_Half_BIT,
			      advertising))
		result |= ADVERTISE_1000HALF;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_1000baseT_Full_BIT,
			      advertising))
		result |= ADVERTISE_1000FULL;

	return result;
}


static inline u32 mii_ctrl1000_to_ethtool_adv_t(u32 adv)
{
	u32 result = 0;

	if (adv & ADVERTISE_1000HALF)
		result |= ADVERTISED_1000baseT_Half;
	if (adv & ADVERTISE_1000FULL)
		result |= ADVERTISED_1000baseT_Full;

	return result;
}


static inline u32 mii_lpa_to_ethtool_lpa_t(u32 lpa)
{
	u32 result = 0;

	if (lpa & LPA_LPACK)
		result |= ADVERTISED_Autoneg;

	return result | mii_adv_to_ethtool_adv_t(lpa);
}


static inline u32 mii_stat1000_to_ethtool_lpa_t(u32 lpa)
{
	u32 result = 0;

	if (lpa & LPA_1000HALF)
		result |= ADVERTISED_1000baseT_Half;
	if (lpa & LPA_1000FULL)
		result |= ADVERTISED_1000baseT_Full;

	return result;
}


static inline void mii_stat1000_mod_linkmode_lpa_t(unsigned long *advertising,
						   u32 lpa)
{
	linkmode_mod_bit(ETHTOOL_LINK_MODE_1000baseT_Half_BIT,
			 advertising, lpa & LPA_1000HALF);

	linkmode_mod_bit(ETHTOOL_LINK_MODE_1000baseT_Full_BIT,
			 advertising, lpa & LPA_1000FULL);
}


static inline u32 ethtool_adv_to_mii_adv_x(u32 ethadv)
{
	u32 result = 0;

	if (ethadv & ADVERTISED_1000baseT_Half)
		result |= ADVERTISE_1000XHALF;
	if (ethadv & ADVERTISED_1000baseT_Full)
		result |= ADVERTISE_1000XFULL;
	if (ethadv & ADVERTISED_Pause)
		result |= ADVERTISE_1000XPAUSE;
	if (ethadv & ADVERTISED_Asym_Pause)
		result |= ADVERTISE_1000XPSE_ASYM;

	return result;
}


static inline u32 mii_adv_to_ethtool_adv_x(u32 adv)
{
	u32 result = 0;

	if (adv & ADVERTISE_1000XHALF)
		result |= ADVERTISED_1000baseT_Half;
	if (adv & ADVERTISE_1000XFULL)
		result |= ADVERTISED_1000baseT_Full;
	if (adv & ADVERTISE_1000XPAUSE)
		result |= ADVERTISED_Pause;
	if (adv & ADVERTISE_1000XPSE_ASYM)
		result |= ADVERTISED_Asym_Pause;

	return result;
}


static inline void mii_adv_mod_linkmode_adv_t(unsigned long *advertising,
					      u32 adv)
{
	linkmode_mod_bit(ETHTOOL_LINK_MODE_10baseT_Half_BIT,
			 advertising, adv & ADVERTISE_10HALF);

	linkmode_mod_bit(ETHTOOL_LINK_MODE_10baseT_Full_BIT,
			 advertising, adv & ADVERTISE_10FULL);

	linkmode_mod_bit(ETHTOOL_LINK_MODE_100baseT_Half_BIT,
			 advertising, adv & ADVERTISE_100HALF);

	linkmode_mod_bit(ETHTOOL_LINK_MODE_100baseT_Full_BIT,
			 advertising, adv & ADVERTISE_100FULL);

	linkmode_mod_bit(ETHTOOL_LINK_MODE_Pause_BIT, advertising,
			 adv & ADVERTISE_PAUSE_CAP);

	linkmode_mod_bit(ETHTOOL_LINK_MODE_Asym_Pause_BIT,
			 advertising, adv & ADVERTISE_PAUSE_ASYM);
}


static inline void mii_adv_to_linkmode_adv_t(unsigned long *advertising,
					     u32 adv)
{
	linkmode_zero(advertising);

	mii_adv_mod_linkmode_adv_t(advertising, adv);
}


static inline void mii_lpa_to_linkmode_lpa_t(unsigned long *lp_advertising,
					     u32 lpa)
{
	mii_adv_to_linkmode_adv_t(lp_advertising, lpa);

	if (lpa & LPA_LPACK)
		linkmode_set_bit(ETHTOOL_LINK_MODE_Autoneg_BIT,
				 lp_advertising);

}


static inline void mii_lpa_mod_linkmode_lpa_t(unsigned long *lp_advertising,
					      u32 lpa)
{
	mii_adv_mod_linkmode_adv_t(lp_advertising, lpa);

	linkmode_mod_bit(ETHTOOL_LINK_MODE_Autoneg_BIT,
			 lp_advertising, lpa & LPA_LPACK);
}

static inline void mii_ctrl1000_mod_linkmode_adv_t(unsigned long *advertising,
						   u32 ctrl1000)
{
	linkmode_mod_bit(ETHTOOL_LINK_MODE_1000baseT_Half_BIT, advertising,
			 ctrl1000 & ADVERTISE_1000HALF);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_1000baseT_Full_BIT, advertising,
			 ctrl1000 & ADVERTISE_1000FULL);
}


static inline u32 linkmode_adv_to_lcl_adv_t(unsigned long *advertising)
{
	u32 lcl_adv = 0;

	if (linkmode_test_bit(ETHTOOL_LINK_MODE_Pause_BIT,
			      advertising))
		lcl_adv |= ADVERTISE_PAUSE_CAP;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_Asym_Pause_BIT,
			      advertising))
		lcl_adv |= ADVERTISE_PAUSE_ASYM;

	return lcl_adv;
}


static inline void mii_lpa_mod_linkmode_x(unsigned long *linkmodes, u16 lpa,
					 int fd_bit)
{
	linkmode_mod_bit(ETHTOOL_LINK_MODE_Autoneg_BIT, linkmodes,
			 lpa & LPA_LPACK);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_Pause_BIT, linkmodes,
			 lpa & LPA_1000XPAUSE);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_Asym_Pause_BIT, linkmodes,
			 lpa & LPA_1000XPAUSE_ASYM);
	linkmode_mod_bit(fd_bit, linkmodes,
			 lpa & LPA_1000XFULL);
}


static inline u16 linkmode_adv_to_mii_adv_x(const unsigned long *linkmodes,
					    int fd_bit)
{
	u16 adv = 0;

	if (linkmode_test_bit(fd_bit, linkmodes))
		adv |= ADVERTISE_1000XFULL;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_Pause_BIT, linkmodes))
		adv |= ADVERTISE_1000XPAUSE;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_Asym_Pause_BIT, linkmodes))
		adv |= ADVERTISE_1000XPSE_ASYM;

	return adv;
}


static inline u16 mii_advertise_flowctrl(int cap)
{
	u16 adv = 0;

	if (cap & FLOW_CTRL_RX)
		adv = ADVERTISE_PAUSE_CAP | ADVERTISE_PAUSE_ASYM;
	if (cap & FLOW_CTRL_TX)
		adv ^= ADVERTISE_PAUSE_ASYM;

	return adv;
}


static inline u8 mii_resolve_flowctrl_fdx(u16 lcladv, u16 rmtadv)
{
	u8 cap = 0;

	if (lcladv & rmtadv & ADVERTISE_PAUSE_CAP) {
		cap = FLOW_CTRL_TX | FLOW_CTRL_RX;
	} else if (lcladv & rmtadv & ADVERTISE_PAUSE_ASYM) {
		if (lcladv & ADVERTISE_PAUSE_CAP)
			cap = FLOW_CTRL_RX;
		else if (rmtadv & ADVERTISE_PAUSE_CAP)
			cap = FLOW_CTRL_TX;
	}

	return cap;
}


static inline u16 mii_bmcr_encode_fixed(int speed, int duplex)
{
	u16 bmcr;

	switch (speed) {
	case SPEED_2500:
	case SPEED_1000:
		bmcr = BMCR_SPEED1000;
		break;

	case SPEED_100:
		bmcr = BMCR_SPEED100;
		break;

	case SPEED_10:
	default:
		bmcr = BMCR_SPEED10;
		break;
	}

	if (duplex == DUPLEX_FULL)
		bmcr |= BMCR_FULLDPLX;

	return bmcr;
}

#endif 
