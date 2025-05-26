/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __PHY_H
#define __PHY_H

#include <linux/compiler.h>
#include <linux/spinlock.h>
#include <linux/ethtool.h>
#include <linux/leds.h>
#include <linux/linkmode.h>
#include <linux/netlink.h>
#include <linux/mdio.h>
#include <linux/mii.h>
#include <linux/mii_timestamper.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/mod_devicetable.h>
#include <linux/u64_stats_sync.h>
#include <linux/irqreturn.h>
#include <linux/iopoll.h>
#include <linux/refcount.h>

#include <linux/atomic.h>

#define PHY_DEFAULT_FEATURES	(SUPPORTED_Autoneg | \
				 SUPPORTED_TP | \
				 SUPPORTED_MII)

#define PHY_10BT_FEATURES	(SUPPORTED_10baseT_Half | \
				 SUPPORTED_10baseT_Full)

#define PHY_100BT_FEATURES	(SUPPORTED_100baseT_Half | \
				 SUPPORTED_100baseT_Full)

#define PHY_1000BT_FEATURES	(SUPPORTED_1000baseT_Half | \
				 SUPPORTED_1000baseT_Full)

extern __ETHTOOL_DECLARE_LINK_MODE_MASK(phy_basic_features) __ro_after_init;
extern __ETHTOOL_DECLARE_LINK_MODE_MASK(phy_basic_t1_features) __ro_after_init;
extern __ETHTOOL_DECLARE_LINK_MODE_MASK(phy_basic_t1s_p2mp_features) __ro_after_init;
extern __ETHTOOL_DECLARE_LINK_MODE_MASK(phy_gbit_features) __ro_after_init;
extern __ETHTOOL_DECLARE_LINK_MODE_MASK(phy_gbit_fibre_features) __ro_after_init;
extern __ETHTOOL_DECLARE_LINK_MODE_MASK(phy_gbit_all_ports_features) __ro_after_init;
extern __ETHTOOL_DECLARE_LINK_MODE_MASK(phy_10gbit_features) __ro_after_init;
extern __ETHTOOL_DECLARE_LINK_MODE_MASK(phy_10gbit_fec_features) __ro_after_init;
extern __ETHTOOL_DECLARE_LINK_MODE_MASK(phy_10gbit_full_features) __ro_after_init;
extern __ETHTOOL_DECLARE_LINK_MODE_MASK(phy_eee_cap1_features) __ro_after_init;

#define PHY_BASIC_FEATURES ((unsigned long *)&phy_basic_features)
#define PHY_BASIC_T1_FEATURES ((unsigned long *)&phy_basic_t1_features)
#define PHY_BASIC_T1S_P2MP_FEATURES ((unsigned long *)&phy_basic_t1s_p2mp_features)
#define PHY_GBIT_FEATURES ((unsigned long *)&phy_gbit_features)
#define PHY_GBIT_FIBRE_FEATURES ((unsigned long *)&phy_gbit_fibre_features)
#define PHY_GBIT_ALL_PORTS_FEATURES ((unsigned long *)&phy_gbit_all_ports_features)
#define PHY_10GBIT_FEATURES ((unsigned long *)&phy_10gbit_features)
#define PHY_10GBIT_FEC_FEATURES ((unsigned long *)&phy_10gbit_fec_features)
#define PHY_10GBIT_FULL_FEATURES ((unsigned long *)&phy_10gbit_full_features)
#define PHY_EEE_CAP1_FEATURES ((unsigned long *)&phy_eee_cap1_features)

extern const int phy_basic_ports_array[3];
extern const int phy_fibre_port_array[1];
extern const int phy_all_ports_features_array[7];
extern const int phy_10_100_features_array[4];
extern const int phy_basic_t1_features_array[3];
extern const int phy_basic_t1s_p2mp_features_array[2];
extern const int phy_gbit_features_array[2];
extern const int phy_10gbit_features_array[1];


#define PHY_POLL		-1
#define PHY_MAC_INTERRUPT	-2

#define PHY_IS_INTERNAL		0x00000001
#define PHY_RST_AFTER_CLK_EN	0x00000002
#define PHY_POLL_CABLE_TEST	0x00000004
#define PHY_ALWAYS_CALL_SUSPEND	0x00000008
#define MDIO_DEVICE_IS_PHY	0x80000000


typedef enum {
	PHY_INTERFACE_MODE_NA,
	PHY_INTERFACE_MODE_INTERNAL,
	PHY_INTERFACE_MODE_MII,
	PHY_INTERFACE_MODE_GMII,
	PHY_INTERFACE_MODE_SGMII,
	PHY_INTERFACE_MODE_TBI,
	PHY_INTERFACE_MODE_REVMII,
	PHY_INTERFACE_MODE_RMII,
	PHY_INTERFACE_MODE_REVRMII,
	PHY_INTERFACE_MODE_RGMII,
	PHY_INTERFACE_MODE_RGMII_ID,
	PHY_INTERFACE_MODE_RGMII_RXID,
	PHY_INTERFACE_MODE_RGMII_TXID,
	PHY_INTERFACE_MODE_RTBI,
	PHY_INTERFACE_MODE_SMII,
	PHY_INTERFACE_MODE_XGMII,
	PHY_INTERFACE_MODE_XLGMII,
	PHY_INTERFACE_MODE_MOCA,
	PHY_INTERFACE_MODE_PSGMII,
	PHY_INTERFACE_MODE_QSGMII,
	PHY_INTERFACE_MODE_TRGMII,
	PHY_INTERFACE_MODE_100BASEX,
	PHY_INTERFACE_MODE_1000BASEX,
	PHY_INTERFACE_MODE_2500BASEX,
	PHY_INTERFACE_MODE_5GBASER,
	PHY_INTERFACE_MODE_RXAUI,
	PHY_INTERFACE_MODE_XAUI,
	
	PHY_INTERFACE_MODE_10GBASER,
	PHY_INTERFACE_MODE_25GBASER,
	PHY_INTERFACE_MODE_100GBASER,
	PHY_INTERFACE_MODE_200GBASER,
	PHY_INTERFACE_MODE_400GBASER,
	PHY_INTERFACE_MODE_USXGMII,
	
	PHY_INTERFACE_MODE_10GKR,
	PHY_INTERFACE_MODE_QUSGMII,
	PHY_INTERFACE_MODE_1000BASEKX,
	PHY_INTERFACE_MODE_MAX,
} phy_interface_t;


#define DECLARE_PHY_INTERFACE_MASK(name) \
	DECLARE_BITMAP(name, PHY_INTERFACE_MODE_MAX)

static inline void phy_interface_zero(unsigned long *intf)
{
	bitmap_zero(intf, PHY_INTERFACE_MODE_MAX);
}

static inline bool phy_interface_empty(const unsigned long *intf)
{
	return bitmap_empty(intf, PHY_INTERFACE_MODE_MAX);
}

static inline void phy_interface_and(unsigned long *dst, const unsigned long *a,
				     const unsigned long *b)
{
	bitmap_and(dst, a, b, PHY_INTERFACE_MODE_MAX);
}

static inline void phy_interface_or(unsigned long *dst, const unsigned long *a,
				    const unsigned long *b)
{
	bitmap_or(dst, a, b, PHY_INTERFACE_MODE_MAX);
}

static inline void phy_interface_set_rgmii(unsigned long *intf)
{
	__set_bit(PHY_INTERFACE_MODE_RGMII, intf);
	__set_bit(PHY_INTERFACE_MODE_RGMII_ID, intf);
	__set_bit(PHY_INTERFACE_MODE_RGMII_RXID, intf);
	__set_bit(PHY_INTERFACE_MODE_RGMII_TXID, intf);
}


unsigned int phy_supported_speeds(struct phy_device *phy,
				      unsigned int *speeds,
				      unsigned int size);


static inline const char *phy_modes(phy_interface_t interface)
{
	switch (interface) {
	case PHY_INTERFACE_MODE_NA:
		return "";
	case PHY_INTERFACE_MODE_INTERNAL:
		return "internal";
	case PHY_INTERFACE_MODE_MII:
		return "mii";
	case PHY_INTERFACE_MODE_GMII:
		return "gmii";
	case PHY_INTERFACE_MODE_SGMII:
		return "sgmii";
	case PHY_INTERFACE_MODE_TBI:
		return "tbi";
	case PHY_INTERFACE_MODE_REVMII:
		return "rev-mii";
	case PHY_INTERFACE_MODE_RMII:
		return "rmii";
	case PHY_INTERFACE_MODE_REVRMII:
		return "rev-rmii";
	case PHY_INTERFACE_MODE_RGMII:
		return "rgmii";
	case PHY_INTERFACE_MODE_RGMII_ID:
		return "rgmii-id";
	case PHY_INTERFACE_MODE_RGMII_RXID:
		return "rgmii-rxid";
	case PHY_INTERFACE_MODE_RGMII_TXID:
		return "rgmii-txid";
	case PHY_INTERFACE_MODE_RTBI:
		return "rtbi";
	case PHY_INTERFACE_MODE_SMII:
		return "smii";
	case PHY_INTERFACE_MODE_XGMII:
		return "xgmii";
	case PHY_INTERFACE_MODE_XLGMII:
		return "xlgmii";
	case PHY_INTERFACE_MODE_MOCA:
		return "moca";
	case PHY_INTERFACE_MODE_PSGMII:
		return "psgmii";
	case PHY_INTERFACE_MODE_QSGMII:
		return "qsgmii";
	case PHY_INTERFACE_MODE_TRGMII:
		return "trgmii";
	case PHY_INTERFACE_MODE_1000BASEX:
		return "1000base-x";
	case PHY_INTERFACE_MODE_1000BASEKX:
		return "1000base-kx";
	case PHY_INTERFACE_MODE_2500BASEX:
		return "2500base-x";
	case PHY_INTERFACE_MODE_5GBASER:
		return "5gbase-r";
	case PHY_INTERFACE_MODE_RXAUI:
		return "rxaui";
	case PHY_INTERFACE_MODE_XAUI:
		return "xaui";
	case PHY_INTERFACE_MODE_10GBASER:
		return "10gbase-r";
	case PHY_INTERFACE_MODE_25GBASER:
		return "25gbase-r";
	case PHY_INTERFACE_MODE_100GBASER:
		return "100gbase-r";
	case PHY_INTERFACE_MODE_200GBASER:
		return "200gbase-r";
	case PHY_INTERFACE_MODE_400GBASER:
		return "400gbase-r";
	case PHY_INTERFACE_MODE_USXGMII:
		return "usxgmii";
	case PHY_INTERFACE_MODE_10GKR:
		return "10gbase-kr";
	case PHY_INTERFACE_MODE_100BASEX:
		return "100base-x";
	case PHY_INTERFACE_MODE_QUSGMII:
		return "qusgmii";
	default:
		return "unknown";
	}
}

#define PHY_INIT_TIMEOUT	100000
#define PHY_FORCE_TIMEOUT	10

#define PHY_MAX_ADDR	32


#define PHY_ID_FMT "%s:%02x"

#define MII_BUS_ID_SIZE	61

struct device;
struct kernel_hwtstamp_config;
struct phylink;
struct sfp_bus;
struct sfp_upstream_ops;
struct sk_buff;


struct mdio_bus_stats {
	u64_stats_t transfers;
	u64_stats_t errors;
	u64_stats_t writes;
	u64_stats_t reads;
	
	struct u64_stats_sync syncp;
};


struct phy_package_shared {
	int addr;
	refcount_t refcnt;
	unsigned long flags;
	size_t priv_size;

	
	
	void *priv;
};


#define PHY_SHARED_F_INIT_DONE  0
#define PHY_SHARED_F_PROBE_DONE 1


struct mii_bus {
	struct module *owner;
	const char *name;
	char id[MII_BUS_ID_SIZE];
	void *priv;
	
	int (*read)(struct mii_bus *bus, int addr, int regnum);
	
	int (*write)(struct mii_bus *bus, int addr, int regnum, u16 val);
	
	int (*read_c45)(struct mii_bus *bus, int addr, int devnum, int regnum);
	
	int (*write_c45)(struct mii_bus *bus, int addr, int devnum,
			 int regnum, u16 val);
	
	int (*reset)(struct mii_bus *bus);

	
	struct mdio_bus_stats stats[PHY_MAX_ADDR];

	
	struct mutex mdio_lock;

	
	struct device *parent;
	
	enum {
		MDIOBUS_ALLOCATED = 1,
		MDIOBUS_REGISTERED,
		MDIOBUS_UNREGISTERED,
		MDIOBUS_RELEASED,
	} state;

	
	struct device dev;

	
	struct mdio_device *mdio_map[PHY_MAX_ADDR];

	
	u32 phy_mask;

	
	u32 phy_ignore_ta_mask;

	
	int irq[PHY_MAX_ADDR];

	
	int reset_delay_us;
	
	int reset_post_delay_us;
	
	struct gpio_desc *reset_gpiod;

	
	struct mutex shared_lock;

	
	struct phy_package_shared *shared[PHY_MAX_ADDR];
};
#define to_mii_bus(d) container_of(d, struct mii_bus, dev)

struct mii_bus *mdiobus_alloc_size(size_t size);


static inline struct mii_bus *mdiobus_alloc(void)
{
	return mdiobus_alloc_size(0);
}

int __mdiobus_register(struct mii_bus *bus, struct module *owner);
int __devm_mdiobus_register(struct device *dev, struct mii_bus *bus,
			    struct module *owner);
#define mdiobus_register(bus) __mdiobus_register(bus, THIS_MODULE)
#define devm_mdiobus_register(dev, bus) \
		__devm_mdiobus_register(dev, bus, THIS_MODULE)

void mdiobus_unregister(struct mii_bus *bus);
void mdiobus_free(struct mii_bus *bus);
struct mii_bus *devm_mdiobus_alloc_size(struct device *dev, int sizeof_priv);
static inline struct mii_bus *devm_mdiobus_alloc(struct device *dev)
{
	return devm_mdiobus_alloc_size(dev, 0);
}

struct mii_bus *mdio_find_bus(const char *mdio_name);
struct phy_device *mdiobus_scan_c22(struct mii_bus *bus, int addr);

#define PHY_INTERRUPT_DISABLED	false
#define PHY_INTERRUPT_ENABLED	true


enum phy_state {
	PHY_DOWN = 0,
	PHY_READY,
	PHY_HALTED,
	PHY_ERROR,
	PHY_UP,
	PHY_RUNNING,
	PHY_NOLINK,
	PHY_CABLETEST,
};

#define MDIO_MMD_NUM 32


struct phy_c45_device_ids {
	u32 devices_in_package;
	u32 mmds_present;
	u32 device_ids[MDIO_MMD_NUM];
};

struct macsec_context;
struct macsec_ops;


struct phy_device {
	struct mdio_device mdio;

	
	
	struct phy_driver *drv;

	struct device_link *devlink;

	u32 phy_id;

	struct phy_c45_device_ids c45_ids;
	unsigned is_c45:1;
	unsigned is_internal:1;
	unsigned is_pseudo_fixed_link:1;
	unsigned is_gigabit_capable:1;
	unsigned has_fixups:1;
	unsigned suspended:1;
	unsigned suspended_by_mdio_bus:1;
	unsigned sysfs_links:1;
	unsigned loopback_enabled:1;
	unsigned downshifted_rate:1;
	unsigned is_on_sfp_module:1;
	unsigned mac_managed_pm:1;
	unsigned wol_enabled:1;

	unsigned autoneg:1;
	
	unsigned link:1;
	unsigned autoneg_complete:1;

	
	unsigned interrupts:1;
	unsigned irq_suspended:1;
	unsigned irq_rerun:1;

	int rate_matching;

	enum phy_state state;

	u32 dev_flags;

	phy_interface_t interface;

	
	int speed;
	int duplex;
	int port;
	int pause;
	int asym_pause;
	u8 master_slave_get;
	u8 master_slave_set;
	u8 master_slave_state;

	
	
	__ETHTOOL_DECLARE_LINK_MODE_MASK(supported);
	__ETHTOOL_DECLARE_LINK_MODE_MASK(advertising);
	__ETHTOOL_DECLARE_LINK_MODE_MASK(lp_advertising);
	
	__ETHTOOL_DECLARE_LINK_MODE_MASK(adv_old);
	
	__ETHTOOL_DECLARE_LINK_MODE_MASK(supported_eee);
	__ETHTOOL_DECLARE_LINK_MODE_MASK(advertising_eee);
	bool eee_enabled;

	
	DECLARE_PHY_INTERFACE_MASK(host_interfaces);

	
	u32 eee_broken_modes;

#ifdef CONFIG_LED_TRIGGER_PHY
	struct phy_led_trigger *phy_led_triggers;
	unsigned int phy_num_led_triggers;
	struct phy_led_trigger *last_triggered;

	struct phy_led_trigger *led_link_trigger;
#endif
	struct list_head leds;

	
	int irq;

	
	
	void *priv;

	
	
	struct phy_package_shared *shared;

	
	struct sk_buff *skb;
	void *ehdr;
	struct nlattr *nest;

	
	struct delayed_work state_queue;

	struct mutex lock;

	
	bool sfp_bus_attached;
	struct sfp_bus *sfp_bus;
	struct phylink *phylink;
	struct net_device *attached_dev;
	struct mii_timestamper *mii_ts;
	struct pse_control *psec;

	u8 mdix;
	u8 mdix_ctrl;

	int pma_extable;

	unsigned int link_down_events;

	void (*phy_link_change)(struct phy_device *phydev, bool up);
	void (*adjust_link)(struct net_device *dev);

#if IS_ENABLED(CONFIG_MACSEC)
	
	const struct macsec_ops *macsec_ops;
#endif
};


#define PHY_F_NO_IRQ		0x80000000

static inline struct phy_device *to_phy_device(const struct device *dev)
{
	return container_of(to_mdio_device(dev), struct phy_device, mdio);
}


struct phy_tdr_config {
	u32 first;
	u32 last;
	u32 step;
	s8 pair;
};
#define PHY_PAIR_ALL -1


struct phy_plca_cfg {
	int version;
	int enabled;
	int node_id;
	int node_cnt;
	int to_tmr;
	int burst_cnt;
	int burst_tmr;
};


struct phy_plca_status {
	bool pst;
};


struct phy_led {
	struct list_head list;
	struct phy_device *phydev;
	struct led_classdev led_cdev;
	u8 index;
};

#define to_phy_led(d) container_of(d, struct phy_led, led_cdev)


struct phy_driver {
	struct mdio_driver_common mdiodrv;
	u32 phy_id;
	char *name;
	u32 phy_id_mask;
	const unsigned long * const features;
	u32 flags;
	const void *driver_data;

	
	int (*soft_reset)(struct phy_device *phydev);

	
	int (*config_init)(struct phy_device *phydev);

	
	int (*probe)(struct phy_device *phydev);

	
	int (*get_features)(struct phy_device *phydev);

	
	int (*get_rate_matching)(struct phy_device *phydev,
				   phy_interface_t iface);

	
	
	int (*suspend)(struct phy_device *phydev);
	
	int (*resume)(struct phy_device *phydev);

	
	int (*config_aneg)(struct phy_device *phydev);

	
	int (*aneg_done)(struct phy_device *phydev);

	
	int (*read_status)(struct phy_device *phydev);

	
	int (*config_intr)(struct phy_device *phydev);

	
	irqreturn_t (*handle_interrupt)(struct phy_device *phydev);

	
	void (*remove)(struct phy_device *phydev);

	
	int (*match_phy_device)(struct phy_device *phydev);

	
	int (*set_wol)(struct phy_device *dev, struct ethtool_wolinfo *wol);

	
	void (*get_wol)(struct phy_device *dev, struct ethtool_wolinfo *wol);

	
	void (*link_change_notify)(struct phy_device *dev);

	
	int (*read_mmd)(struct phy_device *dev, int devnum, u16 regnum);

	
	int (*write_mmd)(struct phy_device *dev, int devnum, u16 regnum,
			 u16 val);

	
	int (*read_page)(struct phy_device *dev);
	
	int (*write_page)(struct phy_device *dev, int page);

	
	int (*module_info)(struct phy_device *dev,
			   struct ethtool_modinfo *modinfo);

	
	int (*module_eeprom)(struct phy_device *dev,
			     struct ethtool_eeprom *ee, u8 *data);

	
	int (*cable_test_start)(struct phy_device *dev);

	
	int (*cable_test_tdr_start)(struct phy_device *dev,
				    const struct phy_tdr_config *config);

	
	int (*cable_test_get_status)(struct phy_device *dev, bool *finished);

	
	
	int (*get_sset_count)(struct phy_device *dev);
	
	void (*get_strings)(struct phy_device *dev, u8 *data);
	
	void (*get_stats)(struct phy_device *dev,
			  struct ethtool_stats *stats, u64 *data);

	
	
	int (*get_tunable)(struct phy_device *dev,
			   struct ethtool_tunable *tuna, void *data);
	
	int (*set_tunable)(struct phy_device *dev,
			    struct ethtool_tunable *tuna,
			    const void *data);
	
	int (*set_loopback)(struct phy_device *dev, bool enable);
	
	int (*get_sqi)(struct phy_device *dev);
	
	int (*get_sqi_max)(struct phy_device *dev);

	
	
	int (*get_plca_cfg)(struct phy_device *dev,
			    struct phy_plca_cfg *plca_cfg);
	
	int (*set_plca_cfg)(struct phy_device *dev,
			    const struct phy_plca_cfg *plca_cfg);
	
	int (*get_plca_status)(struct phy_device *dev,
			       struct phy_plca_status *plca_st);

	
	int (*led_brightness_set)(struct phy_device *dev,
				  u8 index, enum led_brightness value);

	
	int (*led_blink_set)(struct phy_device *dev, u8 index,
			     unsigned long *delay_on,
			     unsigned long *delay_off);
	
	int (*led_hw_is_supported)(struct phy_device *dev, u8 index,
				   unsigned long rules);
	
	int (*led_hw_control_set)(struct phy_device *dev, u8 index,
				  unsigned long rules);
	
	int (*led_hw_control_get)(struct phy_device *dev, u8 index,
				  unsigned long *rules);

};
#define to_phy_driver(d) container_of(to_mdio_common_driver(d),		\
				      struct phy_driver, mdiodrv)

#define PHY_ANY_ID "MATCH ANY PHY"
#define PHY_ANY_UID 0xffffffff

#define PHY_ID_MATCH_EXACT(id) .phy_id = (id), .phy_id_mask = GENMASK(31, 0)
#define PHY_ID_MATCH_MODEL(id) .phy_id = (id), .phy_id_mask = GENMASK(31, 4)
#define PHY_ID_MATCH_VENDOR(id) .phy_id = (id), .phy_id_mask = GENMASK(31, 10)


static inline bool phy_id_compare(u32 id1, u32 id2, u32 mask)
{
	return !((id1 ^ id2) & mask);
}


static inline bool phydev_id_compare(struct phy_device *phydev, u32 id)
{
	return phy_id_compare(id, phydev->phy_id, phydev->drv->phy_id_mask);
}


struct phy_fixup {
	struct list_head list;
	char bus_id[MII_BUS_ID_SIZE + 3];
	u32 phy_uid;
	u32 phy_uid_mask;
	int (*run)(struct phy_device *phydev);
};

const char *phy_speed_to_str(int speed);
const char *phy_duplex_to_str(unsigned int duplex);
const char *phy_rate_matching_to_str(int rate_matching);

int phy_interface_num_ports(phy_interface_t interface);


struct phy_setting {
	u32 speed;
	u8 duplex;
	u8 bit;
};

const struct phy_setting *
phy_lookup_setting(int speed, int duplex, const unsigned long *mask,
		   bool exact);
size_t phy_speeds(unsigned int *speeds, size_t size,
		  unsigned long *mask);
void of_set_phy_supported(struct phy_device *phydev);
void of_set_phy_eee_broken(struct phy_device *phydev);
int phy_speed_down_core(struct phy_device *phydev);


static inline bool phy_is_started(struct phy_device *phydev)
{
	return phydev->state >= PHY_UP;
}

void phy_resolve_aneg_pause(struct phy_device *phydev);
void phy_resolve_aneg_linkmode(struct phy_device *phydev);
void phy_check_downshift(struct phy_device *phydev);


static inline int phy_read(struct phy_device *phydev, u32 regnum)
{
	return mdiobus_read(phydev->mdio.bus, phydev->mdio.addr, regnum);
}

#define phy_read_poll_timeout(phydev, regnum, val, cond, sleep_us, \
				timeout_us, sleep_before_read) \
({ \
	int __ret, __val; \
	__ret = read_poll_timeout(__val = phy_read, val, \
				  __val < 0 || (cond), \
		sleep_us, timeout_us, sleep_before_read, phydev, regnum); \
	if (__val < 0) \
		__ret = __val; \
	if (__ret) \
		phydev_err(phydev, "%s failed: %d\n", __func__, __ret); \
	__ret; \
})


static inline int __phy_read(struct phy_device *phydev, u32 regnum)
{
	return __mdiobus_read(phydev->mdio.bus, phydev->mdio.addr, regnum);
}


static inline int phy_write(struct phy_device *phydev, u32 regnum, u16 val)
{
	return mdiobus_write(phydev->mdio.bus, phydev->mdio.addr, regnum, val);
}


static inline int __phy_write(struct phy_device *phydev, u32 regnum, u16 val)
{
	return __mdiobus_write(phydev->mdio.bus, phydev->mdio.addr, regnum,
			       val);
}


static inline int __phy_modify_changed(struct phy_device *phydev, u32 regnum,
				       u16 mask, u16 set)
{
	return __mdiobus_modify_changed(phydev->mdio.bus, phydev->mdio.addr,
					regnum, mask, set);
}


int phy_read_mmd(struct phy_device *phydev, int devad, u32 regnum);


#define phy_read_mmd_poll_timeout(phydev, devaddr, regnum, val, cond, \
				  sleep_us, timeout_us, sleep_before_read) \
({ \
	int __ret, __val; \
	__ret = read_poll_timeout(__val = phy_read_mmd, val, \
				  __val < 0 || (cond), \
				  sleep_us, timeout_us, sleep_before_read, \
				  phydev, devaddr, regnum); \
	if (__val < 0) \
		__ret = __val; \
	if (__ret) \
		phydev_err(phydev, "%s failed: %d\n", __func__, __ret); \
	__ret; \
})


int __phy_read_mmd(struct phy_device *phydev, int devad, u32 regnum);


int phy_write_mmd(struct phy_device *phydev, int devad, u32 regnum, u16 val);


int __phy_write_mmd(struct phy_device *phydev, int devad, u32 regnum, u16 val);

int __phy_modify_changed(struct phy_device *phydev, u32 regnum, u16 mask,
			 u16 set);
int phy_modify_changed(struct phy_device *phydev, u32 regnum, u16 mask,
		       u16 set);
int __phy_modify(struct phy_device *phydev, u32 regnum, u16 mask, u16 set);
int phy_modify(struct phy_device *phydev, u32 regnum, u16 mask, u16 set);

int __phy_modify_mmd_changed(struct phy_device *phydev, int devad, u32 regnum,
			     u16 mask, u16 set);
int phy_modify_mmd_changed(struct phy_device *phydev, int devad, u32 regnum,
			   u16 mask, u16 set);
int __phy_modify_mmd(struct phy_device *phydev, int devad, u32 regnum,
		     u16 mask, u16 set);
int phy_modify_mmd(struct phy_device *phydev, int devad, u32 regnum,
		   u16 mask, u16 set);


static inline int __phy_set_bits(struct phy_device *phydev, u32 regnum, u16 val)
{
	return __phy_modify(phydev, regnum, 0, val);
}


static inline int __phy_clear_bits(struct phy_device *phydev, u32 regnum,
				   u16 val)
{
	return __phy_modify(phydev, regnum, val, 0);
}


static inline int phy_set_bits(struct phy_device *phydev, u32 regnum, u16 val)
{
	return phy_modify(phydev, regnum, 0, val);
}


static inline int phy_clear_bits(struct phy_device *phydev, u32 regnum, u16 val)
{
	return phy_modify(phydev, regnum, val, 0);
}


static inline int __phy_set_bits_mmd(struct phy_device *phydev, int devad,
		u32 regnum, u16 val)
{
	return __phy_modify_mmd(phydev, devad, regnum, 0, val);
}


static inline int __phy_clear_bits_mmd(struct phy_device *phydev, int devad,
		u32 regnum, u16 val)
{
	return __phy_modify_mmd(phydev, devad, regnum, val, 0);
}


static inline int phy_set_bits_mmd(struct phy_device *phydev, int devad,
		u32 regnum, u16 val)
{
	return phy_modify_mmd(phydev, devad, regnum, 0, val);
}


static inline int phy_clear_bits_mmd(struct phy_device *phydev, int devad,
		u32 regnum, u16 val)
{
	return phy_modify_mmd(phydev, devad, regnum, val, 0);
}


static inline bool phy_interrupt_is_valid(struct phy_device *phydev)
{
	return phydev->irq != PHY_POLL && phydev->irq != PHY_MAC_INTERRUPT;
}


static inline bool phy_polling_mode(struct phy_device *phydev)
{
	if (phydev->state == PHY_CABLETEST)
		if (phydev->drv->flags & PHY_POLL_CABLE_TEST)
			return true;

	return phydev->irq == PHY_POLL;
}


static inline bool phy_has_hwtstamp(struct phy_device *phydev)
{
	return phydev && phydev->mii_ts && phydev->mii_ts->hwtstamp;
}


static inline bool phy_has_rxtstamp(struct phy_device *phydev)
{
	return phydev && phydev->mii_ts && phydev->mii_ts->rxtstamp;
}


static inline bool phy_has_tsinfo(struct phy_device *phydev)
{
	return phydev && phydev->mii_ts && phydev->mii_ts->ts_info;
}


static inline bool phy_has_txtstamp(struct phy_device *phydev)
{
	return phydev && phydev->mii_ts && phydev->mii_ts->txtstamp;
}

static inline int phy_hwtstamp(struct phy_device *phydev, struct ifreq *ifr)
{
	return phydev->mii_ts->hwtstamp(phydev->mii_ts, ifr);
}

static inline bool phy_rxtstamp(struct phy_device *phydev, struct sk_buff *skb,
				int type)
{
	return phydev->mii_ts->rxtstamp(phydev->mii_ts, skb, type);
}

static inline int phy_ts_info(struct phy_device *phydev,
			      struct ethtool_ts_info *tsinfo)
{
	return phydev->mii_ts->ts_info(phydev->mii_ts, tsinfo);
}

static inline void phy_txtstamp(struct phy_device *phydev, struct sk_buff *skb,
				int type)
{
	phydev->mii_ts->txtstamp(phydev->mii_ts, skb, type);
}


static inline bool phy_is_internal(struct phy_device *phydev)
{
	return phydev->is_internal;
}


static inline bool phy_on_sfp(struct phy_device *phydev)
{
	return phydev->is_on_sfp_module;
}


static inline bool phy_interface_mode_is_rgmii(phy_interface_t mode)
{
	return mode >= PHY_INTERFACE_MODE_RGMII &&
		mode <= PHY_INTERFACE_MODE_RGMII_TXID;
};


static inline bool phy_interface_mode_is_8023z(phy_interface_t mode)
{
	return mode == PHY_INTERFACE_MODE_1000BASEX ||
	       mode == PHY_INTERFACE_MODE_2500BASEX;
}


static inline bool phy_interface_is_rgmii(struct phy_device *phydev)
{
	return phy_interface_mode_is_rgmii(phydev->interface);
};


static inline bool phy_is_pseudo_fixed_link(struct phy_device *phydev)
{
	return phydev->is_pseudo_fixed_link;
}

int phy_save_page(struct phy_device *phydev);
int phy_select_page(struct phy_device *phydev, int page);
int phy_restore_page(struct phy_device *phydev, int oldpage, int ret);
int phy_read_paged(struct phy_device *phydev, int page, u32 regnum);
int phy_write_paged(struct phy_device *phydev, int page, u32 regnum, u16 val);
int phy_modify_paged_changed(struct phy_device *phydev, int page, u32 regnum,
			     u16 mask, u16 set);
int phy_modify_paged(struct phy_device *phydev, int page, u32 regnum,
		     u16 mask, u16 set);

struct phy_device *phy_device_create(struct mii_bus *bus, int addr, u32 phy_id,
				     bool is_c45,
				     struct phy_c45_device_ids *c45_ids);
#if IS_ENABLED(CONFIG_PHYLIB)
int fwnode_get_phy_id(struct fwnode_handle *fwnode, u32 *phy_id);
struct mdio_device *fwnode_mdio_find_device(struct fwnode_handle *fwnode);
struct phy_device *fwnode_phy_find_device(struct fwnode_handle *phy_fwnode);
struct phy_device *device_phy_find_device(struct device *dev);
struct fwnode_handle *fwnode_get_phy_node(const struct fwnode_handle *fwnode);
struct phy_device *get_phy_device(struct mii_bus *bus, int addr, bool is_c45);
int phy_device_register(struct phy_device *phy);
void phy_device_free(struct phy_device *phydev);
#else
static inline int fwnode_get_phy_id(struct fwnode_handle *fwnode, u32 *phy_id)
{
	return 0;
}
static inline
struct mdio_device *fwnode_mdio_find_device(struct fwnode_handle *fwnode)
{
	return 0;
}

static inline
struct phy_device *fwnode_phy_find_device(struct fwnode_handle *phy_fwnode)
{
	return NULL;
}

static inline struct phy_device *device_phy_find_device(struct device *dev)
{
	return NULL;
}

static inline
struct fwnode_handle *fwnode_get_phy_node(struct fwnode_handle *fwnode)
{
	return NULL;
}

static inline
struct phy_device *get_phy_device(struct mii_bus *bus, int addr, bool is_c45)
{
	return NULL;
}

static inline int phy_device_register(struct phy_device *phy)
{
	return 0;
}

static inline void phy_device_free(struct phy_device *phydev) { }
#endif 
void phy_device_remove(struct phy_device *phydev);
int phy_get_c45_ids(struct phy_device *phydev);
int phy_init_hw(struct phy_device *phydev);
int phy_suspend(struct phy_device *phydev);
int phy_resume(struct phy_device *phydev);
int __phy_resume(struct phy_device *phydev);
int phy_loopback(struct phy_device *phydev, bool enable);
void phy_sfp_attach(void *upstream, struct sfp_bus *bus);
void phy_sfp_detach(void *upstream, struct sfp_bus *bus);
int phy_sfp_probe(struct phy_device *phydev,
	          const struct sfp_upstream_ops *ops);
struct phy_device *phy_attach(struct net_device *dev, const char *bus_id,
			      phy_interface_t interface);
struct phy_device *phy_find_first(struct mii_bus *bus);
int phy_attach_direct(struct net_device *dev, struct phy_device *phydev,
		      u32 flags, phy_interface_t interface);
int phy_connect_direct(struct net_device *dev, struct phy_device *phydev,
		       void (*handler)(struct net_device *),
		       phy_interface_t interface);
struct phy_device *phy_connect(struct net_device *dev, const char *bus_id,
			       void (*handler)(struct net_device *),
			       phy_interface_t interface);
void phy_disconnect(struct phy_device *phydev);
void phy_detach(struct phy_device *phydev);
void phy_start(struct phy_device *phydev);
void phy_stop(struct phy_device *phydev);
int phy_config_aneg(struct phy_device *phydev);
int phy_start_aneg(struct phy_device *phydev);
int phy_aneg_done(struct phy_device *phydev);
int phy_speed_down(struct phy_device *phydev, bool sync);
int phy_speed_up(struct phy_device *phydev);
bool phy_check_valid(int speed, int duplex, unsigned long *features);

int phy_restart_aneg(struct phy_device *phydev);
int phy_reset_after_clk_enable(struct phy_device *phydev);

#if IS_ENABLED(CONFIG_PHYLIB)
int phy_start_cable_test(struct phy_device *phydev,
			 struct netlink_ext_ack *extack);
int phy_start_cable_test_tdr(struct phy_device *phydev,
			     struct netlink_ext_ack *extack,
			     const struct phy_tdr_config *config);
#else
static inline
int phy_start_cable_test(struct phy_device *phydev,
			 struct netlink_ext_ack *extack)
{
	NL_SET_ERR_MSG(extack, "Kernel not compiled with PHYLIB support");
	return -EOPNOTSUPP;
}
static inline
int phy_start_cable_test_tdr(struct phy_device *phydev,
			     struct netlink_ext_ack *extack,
			     const struct phy_tdr_config *config)
{
	NL_SET_ERR_MSG(extack, "Kernel not compiled with PHYLIB support");
	return -EOPNOTSUPP;
}
#endif

static inline void phy_device_reset(struct phy_device *phydev, int value)
{
	mdio_device_reset(&phydev->mdio, value);
}

#define phydev_err(_phydev, format, args...)	\
	dev_err(&_phydev->mdio.dev, format, ##args)

#define phydev_err_probe(_phydev, err, format, args...)	\
	dev_err_probe(&_phydev->mdio.dev, err, format, ##args)

#define phydev_info(_phydev, format, args...)	\
	dev_info(&_phydev->mdio.dev, format, ##args)

#define phydev_warn(_phydev, format, args...)	\
	dev_warn(&_phydev->mdio.dev, format, ##args)

#define phydev_dbg(_phydev, format, args...)	\
	dev_dbg(&_phydev->mdio.dev, format, ##args)

static inline const char *phydev_name(const struct phy_device *phydev)
{
	return dev_name(&phydev->mdio.dev);
}

static inline void phy_lock_mdio_bus(struct phy_device *phydev)
{
	mutex_lock(&phydev->mdio.bus->mdio_lock);
}

static inline void phy_unlock_mdio_bus(struct phy_device *phydev)
{
	mutex_unlock(&phydev->mdio.bus->mdio_lock);
}

void phy_attached_print(struct phy_device *phydev, const char *fmt, ...)
	__printf(2, 3);
char *phy_attached_info_irq(struct phy_device *phydev)
	__malloc;
void phy_attached_info(struct phy_device *phydev);


int genphy_read_abilities(struct phy_device *phydev);
int genphy_setup_forced(struct phy_device *phydev);
int genphy_restart_aneg(struct phy_device *phydev);
int genphy_check_and_restart_aneg(struct phy_device *phydev, bool restart);
int genphy_config_eee_advert(struct phy_device *phydev);
int __genphy_config_aneg(struct phy_device *phydev, bool changed);
int genphy_aneg_done(struct phy_device *phydev);
int genphy_update_link(struct phy_device *phydev);
int genphy_read_lpa(struct phy_device *phydev);
int genphy_read_status_fixed(struct phy_device *phydev);
int genphy_read_status(struct phy_device *phydev);
int genphy_read_master_slave(struct phy_device *phydev);
int genphy_suspend(struct phy_device *phydev);
int genphy_resume(struct phy_device *phydev);
int genphy_loopback(struct phy_device *phydev, bool enable);
int genphy_soft_reset(struct phy_device *phydev);
irqreturn_t genphy_handle_interrupt_no_ack(struct phy_device *phydev);

static inline int genphy_config_aneg(struct phy_device *phydev)
{
	return __genphy_config_aneg(phydev, false);
}

static inline int genphy_no_config_intr(struct phy_device *phydev)
{
	return 0;
}
int genphy_read_mmd_unsupported(struct phy_device *phdev, int devad,
				u16 regnum);
int genphy_write_mmd_unsupported(struct phy_device *phdev, int devnum,
				 u16 regnum, u16 val);


int genphy_c37_config_aneg(struct phy_device *phydev);
int genphy_c37_read_status(struct phy_device *phydev);


int genphy_c45_restart_aneg(struct phy_device *phydev);
int genphy_c45_check_and_restart_aneg(struct phy_device *phydev, bool restart);
int genphy_c45_aneg_done(struct phy_device *phydev);
int genphy_c45_read_link(struct phy_device *phydev);
int genphy_c45_read_lpa(struct phy_device *phydev);
int genphy_c45_read_pma(struct phy_device *phydev);
int genphy_c45_pma_setup_forced(struct phy_device *phydev);
int genphy_c45_pma_baset1_setup_master_slave(struct phy_device *phydev);
int genphy_c45_an_config_aneg(struct phy_device *phydev);
int genphy_c45_an_disable_aneg(struct phy_device *phydev);
int genphy_c45_read_mdix(struct phy_device *phydev);
int genphy_c45_pma_read_abilities(struct phy_device *phydev);
int genphy_c45_pma_baset1_read_abilities(struct phy_device *phydev);
int genphy_c45_read_eee_abilities(struct phy_device *phydev);
int genphy_c45_pma_baset1_read_master_slave(struct phy_device *phydev);
int genphy_c45_read_status(struct phy_device *phydev);
int genphy_c45_baset1_read_status(struct phy_device *phydev);
int genphy_c45_config_aneg(struct phy_device *phydev);
int genphy_c45_loopback(struct phy_device *phydev, bool enable);
int genphy_c45_pma_resume(struct phy_device *phydev);
int genphy_c45_pma_suspend(struct phy_device *phydev);
int genphy_c45_fast_retrain(struct phy_device *phydev, bool enable);
int genphy_c45_plca_get_cfg(struct phy_device *phydev,
			    struct phy_plca_cfg *plca_cfg);
int genphy_c45_plca_set_cfg(struct phy_device *phydev,
			    const struct phy_plca_cfg *plca_cfg);
int genphy_c45_plca_get_status(struct phy_device *phydev,
			       struct phy_plca_status *plca_st);
int genphy_c45_eee_is_active(struct phy_device *phydev, unsigned long *adv,
			     unsigned long *lp, bool *is_enabled);
int genphy_c45_ethtool_get_eee(struct phy_device *phydev,
			       struct ethtool_eee *data);
int genphy_c45_ethtool_set_eee(struct phy_device *phydev,
			       struct ethtool_eee *data);
int genphy_c45_write_eee_adv(struct phy_device *phydev, unsigned long *adv);
int genphy_c45_an_config_eee_aneg(struct phy_device *phydev);
int genphy_c45_read_eee_adv(struct phy_device *phydev, unsigned long *adv);


extern struct phy_driver genphy_c45_driver;


int gen10g_config_aneg(struct phy_device *phydev);

static inline int phy_read_status(struct phy_device *phydev)
{
	if (!phydev->drv)
		return -EIO;

	if (phydev->drv->read_status)
		return phydev->drv->read_status(phydev);
	else
		return genphy_read_status(phydev);
}

void phy_driver_unregister(struct phy_driver *drv);
void phy_drivers_unregister(struct phy_driver *drv, int n);
int phy_driver_register(struct phy_driver *new_driver, struct module *owner);
int phy_drivers_register(struct phy_driver *new_driver, int n,
			 struct module *owner);
void phy_error(struct phy_device *phydev);
void phy_state_machine(struct work_struct *work);
void phy_queue_state_machine(struct phy_device *phydev, unsigned long jiffies);
void phy_trigger_machine(struct phy_device *phydev);
void phy_mac_interrupt(struct phy_device *phydev);
void phy_start_machine(struct phy_device *phydev);
void phy_stop_machine(struct phy_device *phydev);
void phy_ethtool_ksettings_get(struct phy_device *phydev,
			       struct ethtool_link_ksettings *cmd);
int phy_ethtool_ksettings_set(struct phy_device *phydev,
			      const struct ethtool_link_ksettings *cmd);
int phy_mii_ioctl(struct phy_device *phydev, struct ifreq *ifr, int cmd);
int phy_do_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
int phy_do_ioctl_running(struct net_device *dev, struct ifreq *ifr, int cmd);
int phy_disable_interrupts(struct phy_device *phydev);
void phy_request_interrupt(struct phy_device *phydev);
void phy_free_interrupt(struct phy_device *phydev);
void phy_print_status(struct phy_device *phydev);
int phy_get_rate_matching(struct phy_device *phydev,
			    phy_interface_t iface);
void phy_set_max_speed(struct phy_device *phydev, u32 max_speed);
void phy_remove_link_mode(struct phy_device *phydev, u32 link_mode);
void phy_advertise_supported(struct phy_device *phydev);
void phy_support_sym_pause(struct phy_device *phydev);
void phy_support_asym_pause(struct phy_device *phydev);
void phy_set_sym_pause(struct phy_device *phydev, bool rx, bool tx,
		       bool autoneg);
void phy_set_asym_pause(struct phy_device *phydev, bool rx, bool tx);
bool phy_validate_pause(struct phy_device *phydev,
			struct ethtool_pauseparam *pp);
void phy_get_pause(struct phy_device *phydev, bool *tx_pause, bool *rx_pause);

s32 phy_get_internal_delay(struct phy_device *phydev, struct device *dev,
			   const int *delay_values, int size, bool is_rx);

void phy_resolve_pause(unsigned long *local_adv, unsigned long *partner_adv,
		       bool *tx_pause, bool *rx_pause);

int phy_register_fixup(const char *bus_id, u32 phy_uid, u32 phy_uid_mask,
		       int (*run)(struct phy_device *));
int phy_register_fixup_for_id(const char *bus_id,
			      int (*run)(struct phy_device *));
int phy_register_fixup_for_uid(u32 phy_uid, u32 phy_uid_mask,
			       int (*run)(struct phy_device *));

int phy_unregister_fixup(const char *bus_id, u32 phy_uid, u32 phy_uid_mask);
int phy_unregister_fixup_for_id(const char *bus_id);
int phy_unregister_fixup_for_uid(u32 phy_uid, u32 phy_uid_mask);

int phy_init_eee(struct phy_device *phydev, bool clk_stop_enable);
int phy_get_eee_err(struct phy_device *phydev);
int phy_ethtool_set_eee(struct phy_device *phydev, struct ethtool_eee *data);
int phy_ethtool_get_eee(struct phy_device *phydev, struct ethtool_eee *data);
int phy_ethtool_set_wol(struct phy_device *phydev, struct ethtool_wolinfo *wol);
void phy_ethtool_get_wol(struct phy_device *phydev,
			 struct ethtool_wolinfo *wol);
int phy_ethtool_get_link_ksettings(struct net_device *ndev,
				   struct ethtool_link_ksettings *cmd);
int phy_ethtool_set_link_ksettings(struct net_device *ndev,
				   const struct ethtool_link_ksettings *cmd);
int phy_ethtool_nway_reset(struct net_device *ndev);
int phy_package_join(struct phy_device *phydev, int addr, size_t priv_size);
void phy_package_leave(struct phy_device *phydev);
int devm_phy_package_join(struct device *dev, struct phy_device *phydev,
			  int addr, size_t priv_size);

int __init mdio_bus_init(void);
void mdio_bus_exit(void);

int phy_ethtool_get_strings(struct phy_device *phydev, u8 *data);
int phy_ethtool_get_sset_count(struct phy_device *phydev);
int phy_ethtool_get_stats(struct phy_device *phydev,
			  struct ethtool_stats *stats, u64 *data);
int phy_ethtool_get_plca_cfg(struct phy_device *phydev,
			     struct phy_plca_cfg *plca_cfg);
int phy_ethtool_set_plca_cfg(struct phy_device *phydev,
			     const struct phy_plca_cfg *plca_cfg,
			     struct netlink_ext_ack *extack);
int phy_ethtool_get_plca_status(struct phy_device *phydev,
				struct phy_plca_status *plca_st);

int __phy_hwtstamp_get(struct phy_device *phydev,
		       struct kernel_hwtstamp_config *config);
int __phy_hwtstamp_set(struct phy_device *phydev,
		       struct kernel_hwtstamp_config *config,
		       struct netlink_ext_ack *extack);

static inline int phy_package_read(struct phy_device *phydev, u32 regnum)
{
	struct phy_package_shared *shared = phydev->shared;

	if (!shared)
		return -EIO;

	return mdiobus_read(phydev->mdio.bus, shared->addr, regnum);
}

static inline int __phy_package_read(struct phy_device *phydev, u32 regnum)
{
	struct phy_package_shared *shared = phydev->shared;

	if (!shared)
		return -EIO;

	return __mdiobus_read(phydev->mdio.bus, shared->addr, regnum);
}

static inline int phy_package_write(struct phy_device *phydev,
				    u32 regnum, u16 val)
{
	struct phy_package_shared *shared = phydev->shared;

	if (!shared)
		return -EIO;

	return mdiobus_write(phydev->mdio.bus, shared->addr, regnum, val);
}

static inline int __phy_package_write(struct phy_device *phydev,
				      u32 regnum, u16 val)
{
	struct phy_package_shared *shared = phydev->shared;

	if (!shared)
		return -EIO;

	return __mdiobus_write(phydev->mdio.bus, shared->addr, regnum, val);
}

static inline bool __phy_package_set_once(struct phy_device *phydev,
					  unsigned int b)
{
	struct phy_package_shared *shared = phydev->shared;

	if (!shared)
		return false;

	return !test_and_set_bit(b, &shared->flags);
}

static inline bool phy_package_init_once(struct phy_device *phydev)
{
	return __phy_package_set_once(phydev, PHY_SHARED_F_INIT_DONE);
}

static inline bool phy_package_probe_once(struct phy_device *phydev)
{
	return __phy_package_set_once(phydev, PHY_SHARED_F_PROBE_DONE);
}

extern struct bus_type mdio_bus_type;

struct mdio_board_info {
	const char	*bus_id;
	char		modalias[MDIO_NAME_SIZE];
	int		mdio_addr;
	const void	*platform_data;
};

#if IS_ENABLED(CONFIG_MDIO_DEVICE)
int mdiobus_register_board_info(const struct mdio_board_info *info,
				unsigned int n);
#else
static inline int mdiobus_register_board_info(const struct mdio_board_info *i,
					      unsigned int n)
{
	return 0;
}
#endif



#define phy_module_driver(__phy_drivers, __count)			\
static int __init phy_module_init(void)					\
{									\
	return phy_drivers_register(__phy_drivers, __count, THIS_MODULE); \
}									\
module_init(phy_module_init);						\
static void __exit phy_module_exit(void)				\
{									\
	phy_drivers_unregister(__phy_drivers, __count);			\
}									\
module_exit(phy_module_exit)

#define module_phy_driver(__phy_drivers)				\
	phy_module_driver(__phy_drivers, ARRAY_SIZE(__phy_drivers))

bool phy_driver_is_genphy(struct phy_device *phydev);
bool phy_driver_is_genphy_10g(struct phy_device *phydev);

#endif 
