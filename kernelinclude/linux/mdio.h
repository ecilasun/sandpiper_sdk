/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __LINUX_MDIO_H__
#define __LINUX_MDIO_H__

#include <uapi/linux/mdio.h>
#include <linux/bitfield.h>
#include <linux/mod_devicetable.h>

struct gpio_desc;
struct mii_bus;
struct reset_control;


enum mdio_mutex_lock_class {
	MDIO_MUTEX_NORMAL,
	MDIO_MUTEX_MUX,
	MDIO_MUTEX_NESTED,
};

struct mdio_device {
	struct device dev;

	struct mii_bus *bus;
	char modalias[MDIO_NAME_SIZE];

	int (*bus_match)(struct device *dev, struct device_driver *drv);
	void (*device_free)(struct mdio_device *mdiodev);
	void (*device_remove)(struct mdio_device *mdiodev);

	
	int addr;
	int flags;
	struct gpio_desc *reset_gpio;
	struct reset_control *reset_ctrl;
	unsigned int reset_assert_delay;
	unsigned int reset_deassert_delay;
};

static inline struct mdio_device *to_mdio_device(const struct device *dev)
{
	return container_of(dev, struct mdio_device, dev);
}


struct mdio_driver_common {
	struct device_driver driver;
	int flags;
};
#define MDIO_DEVICE_FLAG_PHY		1

static inline struct mdio_driver_common *
to_mdio_common_driver(const struct device_driver *driver)
{
	return container_of(driver, struct mdio_driver_common, driver);
}


struct mdio_driver {
	struct mdio_driver_common mdiodrv;

	
	int (*probe)(struct mdio_device *mdiodev);

	
	void (*remove)(struct mdio_device *mdiodev);

	
	void (*shutdown)(struct mdio_device *mdiodev);
};

static inline struct mdio_driver *
to_mdio_driver(const struct device_driver *driver)
{
	return container_of(to_mdio_common_driver(driver), struct mdio_driver,
			    mdiodrv);
}


static inline void mdiodev_set_drvdata(struct mdio_device *mdio, void *data)
{
	dev_set_drvdata(&mdio->dev, data);
}

static inline void *mdiodev_get_drvdata(struct mdio_device *mdio)
{
	return dev_get_drvdata(&mdio->dev);
}

void mdio_device_free(struct mdio_device *mdiodev);
struct mdio_device *mdio_device_create(struct mii_bus *bus, int addr);
int mdio_device_register(struct mdio_device *mdiodev);
void mdio_device_remove(struct mdio_device *mdiodev);
void mdio_device_reset(struct mdio_device *mdiodev, int value);
int mdio_driver_register(struct mdio_driver *drv);
void mdio_driver_unregister(struct mdio_driver *drv);
int mdio_device_bus_match(struct device *dev, struct device_driver *drv);

static inline void mdio_device_get(struct mdio_device *mdiodev)
{
	get_device(&mdiodev->dev);
}

static inline void mdio_device_put(struct mdio_device *mdiodev)
{
	mdio_device_free(mdiodev);
}

static inline bool mdio_phy_id_is_c45(int phy_id)
{
	return (phy_id & MDIO_PHY_ID_C45) && !(phy_id & ~MDIO_PHY_ID_C45_MASK);
}

static inline __u16 mdio_phy_id_prtad(int phy_id)
{
	return (phy_id & MDIO_PHY_ID_PRTAD) >> 5;
}

static inline __u16 mdio_phy_id_devad(int phy_id)
{
	return phy_id & MDIO_PHY_ID_DEVAD;
}


struct mdio_if_info {
	int prtad;
	u32 mmds;
	unsigned mode_support;

	struct net_device *dev;
	int (*mdio_read)(struct net_device *dev, int prtad, int devad,
			 u16 addr);
	int (*mdio_write)(struct net_device *dev, int prtad, int devad,
			  u16 addr, u16 val);
};

#define MDIO_PRTAD_NONE			(-1)
#define MDIO_DEVAD_NONE			(-1)
#define MDIO_SUPPORTS_C22		1
#define MDIO_SUPPORTS_C45		2
#define MDIO_EMULATE_C22		4

struct ethtool_cmd;
struct ethtool_pauseparam;
extern int mdio45_probe(struct mdio_if_info *mdio, int prtad);
extern int mdio_set_flag(const struct mdio_if_info *mdio,
			 int prtad, int devad, u16 addr, int mask,
			 bool sense);
extern int mdio45_links_ok(const struct mdio_if_info *mdio, u32 mmds);
extern int mdio45_nway_restart(const struct mdio_if_info *mdio);
extern void mdio45_ethtool_gset_npage(const struct mdio_if_info *mdio,
				      struct ethtool_cmd *ecmd,
				      u32 npage_adv, u32 npage_lpa);
extern void
mdio45_ethtool_ksettings_get_npage(const struct mdio_if_info *mdio,
				   struct ethtool_link_ksettings *cmd,
				   u32 npage_adv, u32 npage_lpa);


static inline void mdio45_ethtool_gset(const struct mdio_if_info *mdio,
				       struct ethtool_cmd *ecmd)
{
	mdio45_ethtool_gset_npage(mdio, ecmd, 0, 0);
}


static inline void
mdio45_ethtool_ksettings_get(const struct mdio_if_info *mdio,
			     struct ethtool_link_ksettings *cmd)
{
	mdio45_ethtool_ksettings_get_npage(mdio, cmd, 0, 0);
}

extern int mdio_mii_ioctl(const struct mdio_if_info *mdio,
			  struct mii_ioctl_data *mii_data, int cmd);


static inline u32 mmd_eee_cap_to_ethtool_sup_t(u16 eee_cap)
{
	u32 supported = 0;

	if (eee_cap & MDIO_EEE_100TX)
		supported |= SUPPORTED_100baseT_Full;
	if (eee_cap & MDIO_EEE_1000T)
		supported |= SUPPORTED_1000baseT_Full;
	if (eee_cap & MDIO_EEE_10GT)
		supported |= SUPPORTED_10000baseT_Full;
	if (eee_cap & MDIO_EEE_1000KX)
		supported |= SUPPORTED_1000baseKX_Full;
	if (eee_cap & MDIO_EEE_10GKX4)
		supported |= SUPPORTED_10000baseKX4_Full;
	if (eee_cap & MDIO_EEE_10GKR)
		supported |= SUPPORTED_10000baseKR_Full;

	return supported;
}


static inline u32 mmd_eee_adv_to_ethtool_adv_t(u16 eee_adv)
{
	u32 adv = 0;

	if (eee_adv & MDIO_EEE_100TX)
		adv |= ADVERTISED_100baseT_Full;
	if (eee_adv & MDIO_EEE_1000T)
		adv |= ADVERTISED_1000baseT_Full;
	if (eee_adv & MDIO_EEE_10GT)
		adv |= ADVERTISED_10000baseT_Full;
	if (eee_adv & MDIO_EEE_1000KX)
		adv |= ADVERTISED_1000baseKX_Full;
	if (eee_adv & MDIO_EEE_10GKX4)
		adv |= ADVERTISED_10000baseKX4_Full;
	if (eee_adv & MDIO_EEE_10GKR)
		adv |= ADVERTISED_10000baseKR_Full;

	return adv;
}


static inline u16 ethtool_adv_to_mmd_eee_adv_t(u32 adv)
{
	u16 reg = 0;

	if (adv & ADVERTISED_100baseT_Full)
		reg |= MDIO_EEE_100TX;
	if (adv & ADVERTISED_1000baseT_Full)
		reg |= MDIO_EEE_1000T;
	if (adv & ADVERTISED_10000baseT_Full)
		reg |= MDIO_EEE_10GT;
	if (adv & ADVERTISED_1000baseKX_Full)
		reg |= MDIO_EEE_1000KX;
	if (adv & ADVERTISED_10000baseKX4_Full)
		reg |= MDIO_EEE_10GKX4;
	if (adv & ADVERTISED_10000baseKR_Full)
		reg |= MDIO_EEE_10GKR;

	return reg;
}


static inline u32 linkmode_adv_to_mii_10gbt_adv_t(unsigned long *advertising)
{
	u32 result = 0;

	if (linkmode_test_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
			      advertising))
		result |= MDIO_AN_10GBT_CTRL_ADV2_5G;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_5000baseT_Full_BIT,
			      advertising))
		result |= MDIO_AN_10GBT_CTRL_ADV5G;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_10000baseT_Full_BIT,
			      advertising))
		result |= MDIO_AN_10GBT_CTRL_ADV10G;

	return result;
}


static inline void mii_10gbt_stat_mod_linkmode_lpa_t(unsigned long *advertising,
						     u32 lpa)
{
	linkmode_mod_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT,
			 advertising, lpa & MDIO_AN_10GBT_STAT_LP2_5G);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_5000baseT_Full_BIT,
			 advertising, lpa & MDIO_AN_10GBT_STAT_LP5G);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_10000baseT_Full_BIT,
			 advertising, lpa & MDIO_AN_10GBT_STAT_LP10G);
}


static inline void mii_t1_adv_l_mod_linkmode_t(unsigned long *advertising, u32 lpa)
{
	linkmode_mod_bit(ETHTOOL_LINK_MODE_Pause_BIT, advertising,
			 lpa & MDIO_AN_T1_ADV_L_PAUSE_CAP);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_Asym_Pause_BIT, advertising,
			 lpa & MDIO_AN_T1_ADV_L_PAUSE_ASYM);
}


static inline void mii_t1_adv_m_mod_linkmode_t(unsigned long *advertising, u32 lpa)
{
	linkmode_mod_bit(ETHTOOL_LINK_MODE_10baseT1L_Full_BIT,
			 advertising, lpa & MDIO_AN_T1_ADV_M_B10L);
}


static inline u32 linkmode_adv_to_mii_t1_adv_l_t(unsigned long *advertising)
{
	u32 result = 0;

	if (linkmode_test_bit(ETHTOOL_LINK_MODE_Pause_BIT, advertising))
		result |= MDIO_AN_T1_ADV_L_PAUSE_CAP;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_Asym_Pause_BIT, advertising))
		result |= MDIO_AN_T1_ADV_L_PAUSE_ASYM;

	return result;
}


static inline u32 linkmode_adv_to_mii_t1_adv_m_t(unsigned long *advertising)
{
	u32 result = 0;

	if (linkmode_test_bit(ETHTOOL_LINK_MODE_10baseT1L_Full_BIT, advertising))
		result |= MDIO_AN_T1_ADV_M_B10L;

	return result;
}


static inline void mii_eee_cap1_mod_linkmode_t(unsigned long *adv, u32 val)
{
	linkmode_mod_bit(ETHTOOL_LINK_MODE_100baseT_Full_BIT,
			 adv, val & MDIO_EEE_100TX);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_1000baseT_Full_BIT,
			 adv, val & MDIO_EEE_1000T);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_10000baseT_Full_BIT,
			 adv, val & MDIO_EEE_10GT);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_1000baseKX_Full_BIT,
			 adv, val & MDIO_EEE_1000KX);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_10000baseKX4_Full_BIT,
			 adv, val & MDIO_EEE_10GKX4);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_10000baseKR_Full_BIT,
			 adv, val & MDIO_EEE_10GKR);
}


static inline u32 linkmode_to_mii_eee_cap1_t(unsigned long *adv)
{
	u32 result = 0;

	if (linkmode_test_bit(ETHTOOL_LINK_MODE_100baseT_Full_BIT, adv))
		result |= MDIO_EEE_100TX;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_1000baseT_Full_BIT, adv))
		result |= MDIO_EEE_1000T;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_10000baseT_Full_BIT, adv))
		result |= MDIO_EEE_10GT;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_1000baseKX_Full_BIT, adv))
		result |= MDIO_EEE_1000KX;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_10000baseKX4_Full_BIT, adv))
		result |= MDIO_EEE_10GKX4;
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_10000baseKR_Full_BIT, adv))
		result |= MDIO_EEE_10GKR;

	return result;
}


static inline void mii_10base_t1_adv_mod_linkmode_t(unsigned long *adv, u16 val)
{
	linkmode_mod_bit(ETHTOOL_LINK_MODE_10baseT1L_Full_BIT,
			 adv, val & MDIO_AN_10BT1_AN_CTRL_ADV_EEE_T1L);
}


static inline u32 linkmode_adv_to_mii_10base_t1_t(unsigned long *adv)
{
	u32 result = 0;

	if (linkmode_test_bit(ETHTOOL_LINK_MODE_10baseT1L_Full_BIT, adv))
		result |= MDIO_AN_10BT1_AN_CTRL_ADV_EEE_T1L;

	return result;
}


static inline void mii_c73_mod_linkmode(unsigned long *adv, u16 *lpa)
{
	linkmode_mod_bit(ETHTOOL_LINK_MODE_Pause_BIT,
			 adv, lpa[0] & MDIO_AN_C73_0_PAUSE);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_Asym_Pause_BIT,
			 adv, lpa[0] & MDIO_AN_C73_0_ASM_DIR);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_1000baseKX_Full_BIT,
			 adv, lpa[1] & MDIO_AN_C73_1_1000BASE_KX);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_10000baseKX4_Full_BIT,
			 adv, lpa[1] & MDIO_AN_C73_1_10GBASE_KX4);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_40000baseKR4_Full_BIT,
			 adv, lpa[1] & MDIO_AN_C73_1_40GBASE_KR4);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_40000baseCR4_Full_BIT,
			 adv, lpa[1] & MDIO_AN_C73_1_40GBASE_CR4);
	
	linkmode_mod_bit(ETHTOOL_LINK_MODE_100000baseKR4_Full_BIT,
			 adv, lpa[1] & MDIO_AN_C73_1_100GBASE_KR4);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_100000baseCR4_Full_BIT,
			 adv, lpa[1] & MDIO_AN_C73_1_100GBASE_CR4);
	
	
	linkmode_mod_bit(ETHTOOL_LINK_MODE_25000baseKR_Full_BIT,
			 adv, lpa[1] & MDIO_AN_C73_1_25GBASE_R);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_25000baseCR_Full_BIT,
			 adv, lpa[1] & MDIO_AN_C73_1_25GBASE_R);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_10000baseKR_Full_BIT,
			 adv, lpa[1] & MDIO_AN_C73_1_10GBASE_KR);
	linkmode_mod_bit(ETHTOOL_LINK_MODE_2500baseX_Full_BIT,
			 adv, lpa[2] & MDIO_AN_C73_2_2500BASE_KX);
	
}

int __mdiobus_read(struct mii_bus *bus, int addr, u32 regnum);
int __mdiobus_write(struct mii_bus *bus, int addr, u32 regnum, u16 val);
int __mdiobus_modify(struct mii_bus *bus, int addr, u32 regnum, u16 mask,
		     u16 set);
int __mdiobus_modify_changed(struct mii_bus *bus, int addr, u32 regnum,
			     u16 mask, u16 set);

int mdiobus_read(struct mii_bus *bus, int addr, u32 regnum);
int mdiobus_read_nested(struct mii_bus *bus, int addr, u32 regnum);
int mdiobus_write(struct mii_bus *bus, int addr, u32 regnum, u16 val);
int mdiobus_write_nested(struct mii_bus *bus, int addr, u32 regnum, u16 val);
int mdiobus_modify(struct mii_bus *bus, int addr, u32 regnum, u16 mask,
		   u16 set);
int mdiobus_modify_changed(struct mii_bus *bus, int addr, u32 regnum,
			   u16 mask, u16 set);
int __mdiobus_c45_read(struct mii_bus *bus, int addr, int devad, u32 regnum);
int mdiobus_c45_read(struct mii_bus *bus, int addr, int devad, u32 regnum);
int mdiobus_c45_read_nested(struct mii_bus *bus, int addr, int devad,
			     u32 regnum);
int __mdiobus_c45_write(struct mii_bus *bus, int addr,  int devad, u32 regnum,
			u16 val);
int mdiobus_c45_write(struct mii_bus *bus, int addr,  int devad, u32 regnum,
		      u16 val);
int mdiobus_c45_write_nested(struct mii_bus *bus, int addr,  int devad,
			     u32 regnum, u16 val);
int mdiobus_c45_modify(struct mii_bus *bus, int addr, int devad, u32 regnum,
		       u16 mask, u16 set);

int mdiobus_c45_modify_changed(struct mii_bus *bus, int addr, int devad,
			       u32 regnum, u16 mask, u16 set);

static inline int __mdiodev_read(struct mdio_device *mdiodev, u32 regnum)
{
	return __mdiobus_read(mdiodev->bus, mdiodev->addr, regnum);
}

static inline int __mdiodev_write(struct mdio_device *mdiodev, u32 regnum,
				  u16 val)
{
	return __mdiobus_write(mdiodev->bus, mdiodev->addr, regnum, val);
}

static inline int __mdiodev_modify(struct mdio_device *mdiodev, u32 regnum,
				   u16 mask, u16 set)
{
	return __mdiobus_modify(mdiodev->bus, mdiodev->addr, regnum, mask, set);
}

static inline int __mdiodev_modify_changed(struct mdio_device *mdiodev,
					   u32 regnum, u16 mask, u16 set)
{
	return __mdiobus_modify_changed(mdiodev->bus, mdiodev->addr, regnum,
					mask, set);
}

static inline int mdiodev_read(struct mdio_device *mdiodev, u32 regnum)
{
	return mdiobus_read(mdiodev->bus, mdiodev->addr, regnum);
}

static inline int mdiodev_write(struct mdio_device *mdiodev, u32 regnum,
				u16 val)
{
	return mdiobus_write(mdiodev->bus, mdiodev->addr, regnum, val);
}

static inline int mdiodev_modify(struct mdio_device *mdiodev, u32 regnum,
				 u16 mask, u16 set)
{
	return mdiobus_modify(mdiodev->bus, mdiodev->addr, regnum, mask, set);
}

static inline int mdiodev_modify_changed(struct mdio_device *mdiodev,
					 u32 regnum, u16 mask, u16 set)
{
	return mdiobus_modify_changed(mdiodev->bus, mdiodev->addr, regnum,
				      mask, set);
}

static inline int mdiodev_c45_modify(struct mdio_device *mdiodev, int devad,
				     u32 regnum, u16 mask, u16 set)
{
	return mdiobus_c45_modify(mdiodev->bus, mdiodev->addr, devad, regnum,
				  mask, set);
}

static inline int mdiodev_c45_modify_changed(struct mdio_device *mdiodev,
					     int devad, u32 regnum, u16 mask,
					     u16 set)
{
	return mdiobus_c45_modify_changed(mdiodev->bus, mdiodev->addr, devad,
					  regnum, mask, set);
}

static inline int mdiodev_c45_read(struct mdio_device *mdiodev, int devad,
				   u16 regnum)
{
	return mdiobus_c45_read(mdiodev->bus, mdiodev->addr, devad, regnum);
}

static inline int mdiodev_c45_write(struct mdio_device *mdiodev, u32 devad,
				    u16 regnum, u16 val)
{
	return mdiobus_c45_write(mdiodev->bus, mdiodev->addr, devad, regnum,
				 val);
}

int mdiobus_register_device(struct mdio_device *mdiodev);
int mdiobus_unregister_device(struct mdio_device *mdiodev);
bool mdiobus_is_registered_device(struct mii_bus *bus, int addr);
struct phy_device *mdiobus_get_phy(struct mii_bus *bus, int addr);


#define mdio_module_driver(_mdio_driver)				\
static int __init mdio_module_init(void)				\
{									\
	return mdio_driver_register(&_mdio_driver);			\
}									\
module_init(mdio_module_init);						\
static void __exit mdio_module_exit(void)				\
{									\
	mdio_driver_unregister(&_mdio_driver);				\
}									\
module_exit(mdio_module_exit)

#endif 
