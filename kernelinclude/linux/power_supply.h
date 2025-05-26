/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_POWER_SUPPLY_H__
#define __LINUX_POWER_SUPPLY_H__

#include <linux/device.h>
#include <linux/workqueue.h>
#include <linux/leds.h>
#include <linux/spinlock.h>
#include <linux/notifier.h>





enum {
	POWER_SUPPLY_STATUS_UNKNOWN = 0,
	POWER_SUPPLY_STATUS_CHARGING,
	POWER_SUPPLY_STATUS_DISCHARGING,
	POWER_SUPPLY_STATUS_NOT_CHARGING,
	POWER_SUPPLY_STATUS_FULL,
};


enum {
	POWER_SUPPLY_CHARGE_TYPE_UNKNOWN = 0,
	POWER_SUPPLY_CHARGE_TYPE_NONE,
	POWER_SUPPLY_CHARGE_TYPE_TRICKLE,	
	POWER_SUPPLY_CHARGE_TYPE_FAST,		
	POWER_SUPPLY_CHARGE_TYPE_STANDARD,	
	POWER_SUPPLY_CHARGE_TYPE_ADAPTIVE,	
	POWER_SUPPLY_CHARGE_TYPE_CUSTOM,	
	POWER_SUPPLY_CHARGE_TYPE_LONGLIFE,	
	POWER_SUPPLY_CHARGE_TYPE_BYPASS,	
};

enum {
	POWER_SUPPLY_HEALTH_UNKNOWN = 0,
	POWER_SUPPLY_HEALTH_GOOD,
	POWER_SUPPLY_HEALTH_OVERHEAT,
	POWER_SUPPLY_HEALTH_DEAD,
	POWER_SUPPLY_HEALTH_OVERVOLTAGE,
	POWER_SUPPLY_HEALTH_UNSPEC_FAILURE,
	POWER_SUPPLY_HEALTH_COLD,
	POWER_SUPPLY_HEALTH_WATCHDOG_TIMER_EXPIRE,
	POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE,
	POWER_SUPPLY_HEALTH_OVERCURRENT,
	POWER_SUPPLY_HEALTH_CALIBRATION_REQUIRED,
	POWER_SUPPLY_HEALTH_WARM,
	POWER_SUPPLY_HEALTH_COOL,
	POWER_SUPPLY_HEALTH_HOT,
	POWER_SUPPLY_HEALTH_NO_BATTERY,
};

enum {
	POWER_SUPPLY_TECHNOLOGY_UNKNOWN = 0,
	POWER_SUPPLY_TECHNOLOGY_NiMH,
	POWER_SUPPLY_TECHNOLOGY_LION,
	POWER_SUPPLY_TECHNOLOGY_LIPO,
	POWER_SUPPLY_TECHNOLOGY_LiFe,
	POWER_SUPPLY_TECHNOLOGY_NiCd,
	POWER_SUPPLY_TECHNOLOGY_LiMn,
};

enum {
	POWER_SUPPLY_CAPACITY_LEVEL_UNKNOWN = 0,
	POWER_SUPPLY_CAPACITY_LEVEL_CRITICAL,
	POWER_SUPPLY_CAPACITY_LEVEL_LOW,
	POWER_SUPPLY_CAPACITY_LEVEL_NORMAL,
	POWER_SUPPLY_CAPACITY_LEVEL_HIGH,
	POWER_SUPPLY_CAPACITY_LEVEL_FULL,
};

enum {
	POWER_SUPPLY_SCOPE_UNKNOWN = 0,
	POWER_SUPPLY_SCOPE_SYSTEM,
	POWER_SUPPLY_SCOPE_DEVICE,
};

enum power_supply_property {
	
	POWER_SUPPLY_PROP_STATUS = 0,
	POWER_SUPPLY_PROP_CHARGE_TYPE,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_AUTHENTIC,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_CYCLE_COUNT,
	POWER_SUPPLY_PROP_VOLTAGE_MAX,
	POWER_SUPPLY_PROP_VOLTAGE_MIN,
	POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN,
	POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_VOLTAGE_AVG,
	POWER_SUPPLY_PROP_VOLTAGE_OCV,
	POWER_SUPPLY_PROP_VOLTAGE_BOOT,
	POWER_SUPPLY_PROP_CURRENT_MAX,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_CURRENT_AVG,
	POWER_SUPPLY_PROP_CURRENT_BOOT,
	POWER_SUPPLY_PROP_POWER_NOW,
	POWER_SUPPLY_PROP_POWER_AVG,
	POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN,
	POWER_SUPPLY_PROP_CHARGE_EMPTY_DESIGN,
	POWER_SUPPLY_PROP_CHARGE_FULL,
	POWER_SUPPLY_PROP_CHARGE_EMPTY,
	POWER_SUPPLY_PROP_CHARGE_NOW,
	POWER_SUPPLY_PROP_CHARGE_AVG,
	POWER_SUPPLY_PROP_CHARGE_COUNTER,
	POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT,
	POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX,
	POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE,
	POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE_MAX,
	POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT,
	POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT_MAX,
	POWER_SUPPLY_PROP_CHARGE_CONTROL_START_THRESHOLD, 
	POWER_SUPPLY_PROP_CHARGE_CONTROL_END_THRESHOLD, 
	POWER_SUPPLY_PROP_CHARGE_BEHAVIOUR,
	POWER_SUPPLY_PROP_INPUT_CURRENT_LIMIT,
	POWER_SUPPLY_PROP_INPUT_VOLTAGE_LIMIT,
	POWER_SUPPLY_PROP_INPUT_POWER_LIMIT,
	POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN,
	POWER_SUPPLY_PROP_ENERGY_EMPTY_DESIGN,
	POWER_SUPPLY_PROP_ENERGY_FULL,
	POWER_SUPPLY_PROP_ENERGY_EMPTY,
	POWER_SUPPLY_PROP_ENERGY_NOW,
	POWER_SUPPLY_PROP_ENERGY_AVG,
	POWER_SUPPLY_PROP_CAPACITY, 
	POWER_SUPPLY_PROP_CAPACITY_ALERT_MIN, 
	POWER_SUPPLY_PROP_CAPACITY_ALERT_MAX, 
	POWER_SUPPLY_PROP_CAPACITY_ERROR_MARGIN, 
	POWER_SUPPLY_PROP_CAPACITY_LEVEL,
	POWER_SUPPLY_PROP_TEMP,
	POWER_SUPPLY_PROP_TEMP_MAX,
	POWER_SUPPLY_PROP_TEMP_MIN,
	POWER_SUPPLY_PROP_TEMP_ALERT_MIN,
	POWER_SUPPLY_PROP_TEMP_ALERT_MAX,
	POWER_SUPPLY_PROP_TEMP_AMBIENT,
	POWER_SUPPLY_PROP_TEMP_AMBIENT_ALERT_MIN,
	POWER_SUPPLY_PROP_TEMP_AMBIENT_ALERT_MAX,
	POWER_SUPPLY_PROP_TIME_TO_EMPTY_NOW,
	POWER_SUPPLY_PROP_TIME_TO_EMPTY_AVG,
	POWER_SUPPLY_PROP_TIME_TO_FULL_NOW,
	POWER_SUPPLY_PROP_TIME_TO_FULL_AVG,
	POWER_SUPPLY_PROP_TYPE, 
	POWER_SUPPLY_PROP_USB_TYPE,
	POWER_SUPPLY_PROP_SCOPE,
	POWER_SUPPLY_PROP_PRECHARGE_CURRENT,
	POWER_SUPPLY_PROP_CHARGE_TERM_CURRENT,
	POWER_SUPPLY_PROP_CALIBRATE,
	POWER_SUPPLY_PROP_MANUFACTURE_YEAR,
	POWER_SUPPLY_PROP_MANUFACTURE_MONTH,
	POWER_SUPPLY_PROP_MANUFACTURE_DAY,
	
	POWER_SUPPLY_PROP_MODEL_NAME,
	POWER_SUPPLY_PROP_MANUFACTURER,
	POWER_SUPPLY_PROP_SERIAL_NUMBER,
};

enum power_supply_type {
	POWER_SUPPLY_TYPE_UNKNOWN = 0,
	POWER_SUPPLY_TYPE_BATTERY,
	POWER_SUPPLY_TYPE_UPS,
	POWER_SUPPLY_TYPE_MAINS,
	POWER_SUPPLY_TYPE_USB,			
	POWER_SUPPLY_TYPE_USB_DCP,		
	POWER_SUPPLY_TYPE_USB_CDP,		
	POWER_SUPPLY_TYPE_USB_ACA,		
	POWER_SUPPLY_TYPE_USB_TYPE_C,		
	POWER_SUPPLY_TYPE_USB_PD,		
	POWER_SUPPLY_TYPE_USB_PD_DRP,		
	POWER_SUPPLY_TYPE_APPLE_BRICK_ID,	
	POWER_SUPPLY_TYPE_WIRELESS,		
};

enum power_supply_usb_type {
	POWER_SUPPLY_USB_TYPE_UNKNOWN = 0,
	POWER_SUPPLY_USB_TYPE_SDP,		
	POWER_SUPPLY_USB_TYPE_DCP,		
	POWER_SUPPLY_USB_TYPE_CDP,		
	POWER_SUPPLY_USB_TYPE_ACA,		
	POWER_SUPPLY_USB_TYPE_C,		
	POWER_SUPPLY_USB_TYPE_PD,		
	POWER_SUPPLY_USB_TYPE_PD_DRP,		
	POWER_SUPPLY_USB_TYPE_PD_PPS,		
	POWER_SUPPLY_USB_TYPE_APPLE_BRICK_ID,	
};

enum power_supply_charge_behaviour {
	POWER_SUPPLY_CHARGE_BEHAVIOUR_AUTO = 0,
	POWER_SUPPLY_CHARGE_BEHAVIOUR_INHIBIT_CHARGE,
	POWER_SUPPLY_CHARGE_BEHAVIOUR_FORCE_DISCHARGE,
};

enum power_supply_notifier_events {
	PSY_EVENT_PROP_CHANGED,
};

union power_supply_propval {
	int intval;
	const char *strval;
};

struct device_node;
struct power_supply;


struct power_supply_config {
	struct device_node *of_node;
	struct fwnode_handle *fwnode;

	
	void *drv_data;

	
	const struct attribute_group **attr_grp;

	char **supplied_to;
	size_t num_supplicants;
};


struct power_supply_desc {
	const char *name;
	enum power_supply_type type;
	const enum power_supply_usb_type *usb_types;
	size_t num_usb_types;
	const enum power_supply_property *properties;
	size_t num_properties;

	
	int (*get_property)(struct power_supply *psy,
			    enum power_supply_property psp,
			    union power_supply_propval *val);
	int (*set_property)(struct power_supply *psy,
			    enum power_supply_property psp,
			    const union power_supply_propval *val);
	
	int (*property_is_writeable)(struct power_supply *psy,
				     enum power_supply_property psp);
	void (*external_power_changed)(struct power_supply *psy);
	void (*set_charged)(struct power_supply *psy);

	
	bool no_thermal;
	
	int use_for_apm;
};

struct power_supply {
	const struct power_supply_desc *desc;

	char **supplied_to;
	size_t num_supplicants;

	char **supplied_from;
	size_t num_supplies;
	struct device_node *of_node;

	
	void *drv_data;

	
	struct device dev;
	struct work_struct changed_work;
	struct delayed_work deferred_register_work;
	spinlock_t changed_lock;
	bool changed;
	bool initialized;
	bool removing;
	atomic_t use_cnt;
	struct power_supply_battery_info *battery_info;
#ifdef CONFIG_THERMAL
	struct thermal_zone_device *tzd;
	struct thermal_cooling_device *tcd;
#endif

#ifdef CONFIG_LEDS_TRIGGERS
	struct led_trigger *charging_full_trig;
	char *charging_full_trig_name;
	struct led_trigger *charging_trig;
	char *charging_trig_name;
	struct led_trigger *full_trig;
	char *full_trig_name;
	struct led_trigger *online_trig;
	char *online_trig_name;
	struct led_trigger *charging_blink_full_solid_trig;
	char *charging_blink_full_solid_trig_name;
#endif
};



struct power_supply_info {
	const char *name;
	int technology;
	int voltage_max_design;
	int voltage_min_design;
	int charge_full_design;
	int charge_empty_design;
	int energy_full_design;
	int energy_empty_design;
	int use_for_apm;
};

struct power_supply_battery_ocv_table {
	int ocv;	
	int capacity;	
};

struct power_supply_resistance_temp_table {
	int temp;	
	int resistance;	
};

struct power_supply_vbat_ri_table {
	int vbat_uv;	
	int ri_uohm;	
};


struct power_supply_maintenance_charge_table {
	int charge_current_max_ua;
	int charge_voltage_max_uv;
	int charge_safety_timer_minutes;
};

#define POWER_SUPPLY_OCV_TEMP_MAX 20



struct power_supply_battery_info {
	unsigned int technology;
	int energy_full_design_uwh;
	int charge_full_design_uah;
	int voltage_min_design_uv;
	int voltage_max_design_uv;
	int tricklecharge_current_ua;
	int precharge_current_ua;
	int precharge_voltage_max_uv;
	int charge_term_current_ua;
	int charge_restart_voltage_uv;
	int overvoltage_limit_uv;
	int constant_charge_current_max_ua;
	int constant_charge_voltage_max_uv;
	struct power_supply_maintenance_charge_table *maintenance_charge;
	int maintenance_charge_size;
	int alert_low_temp_charge_current_ua;
	int alert_low_temp_charge_voltage_uv;
	int alert_high_temp_charge_current_ua;
	int alert_high_temp_charge_voltage_uv;
	int factory_internal_resistance_uohm;
	int factory_internal_resistance_charging_uohm;
	int ocv_temp[POWER_SUPPLY_OCV_TEMP_MAX];
	int temp_ambient_alert_min;
	int temp_ambient_alert_max;
	int temp_alert_min;
	int temp_alert_max;
	int temp_min;
	int temp_max;
	struct power_supply_battery_ocv_table *ocv_table[POWER_SUPPLY_OCV_TEMP_MAX];
	int ocv_table_size[POWER_SUPPLY_OCV_TEMP_MAX];
	struct power_supply_resistance_temp_table *resist_table;
	int resist_table_size;
	struct power_supply_vbat_ri_table *vbat2ri_discharging;
	int vbat2ri_discharging_size;
	struct power_supply_vbat_ri_table *vbat2ri_charging;
	int vbat2ri_charging_size;
	int bti_resistance_ohm;
	int bti_resistance_tolerance;
};

extern struct blocking_notifier_head power_supply_notifier;
extern int power_supply_reg_notifier(struct notifier_block *nb);
extern void power_supply_unreg_notifier(struct notifier_block *nb);
#if IS_ENABLED(CONFIG_POWER_SUPPLY)
extern struct power_supply *power_supply_get_by_name(const char *name);
extern void power_supply_put(struct power_supply *psy);
#else
static inline void power_supply_put(struct power_supply *psy) {}
static inline struct power_supply *power_supply_get_by_name(const char *name)
{ return NULL; }
#endif
#ifdef CONFIG_OF
extern struct power_supply *power_supply_get_by_phandle(struct device_node *np,
							const char *property);
extern struct power_supply *devm_power_supply_get_by_phandle(
				    struct device *dev, const char *property);
#else 
static inline struct power_supply *
power_supply_get_by_phandle(struct device_node *np, const char *property)
{ return NULL; }
static inline struct power_supply *
devm_power_supply_get_by_phandle(struct device *dev, const char *property)
{ return NULL; }
#endif 

extern const enum power_supply_property power_supply_battery_info_properties[];
extern const size_t power_supply_battery_info_properties_size;
extern int power_supply_get_battery_info(struct power_supply *psy,
					 struct power_supply_battery_info **info_out);
extern void power_supply_put_battery_info(struct power_supply *psy,
					  struct power_supply_battery_info *info);
extern bool power_supply_battery_info_has_prop(struct power_supply_battery_info *info,
					       enum power_supply_property psp);
extern int power_supply_battery_info_get_prop(struct power_supply_battery_info *info,
					      enum power_supply_property psp,
					      union power_supply_propval *val);
extern int power_supply_ocv2cap_simple(struct power_supply_battery_ocv_table *table,
				       int table_len, int ocv);
extern struct power_supply_battery_ocv_table *
power_supply_find_ocv2cap_table(struct power_supply_battery_info *info,
				int temp, int *table_len);
extern int power_supply_batinfo_ocv2cap(struct power_supply_battery_info *info,
					int ocv, int temp);
extern int
power_supply_temp2resist_simple(struct power_supply_resistance_temp_table *table,
				int table_len, int temp);
extern int power_supply_vbat2ri(struct power_supply_battery_info *info,
				int vbat_uv, bool charging);
extern struct power_supply_maintenance_charge_table *
power_supply_get_maintenance_charging_setting(struct power_supply_battery_info *info, int index);
extern bool power_supply_battery_bti_in_range(struct power_supply_battery_info *info,
					      int resistance);
extern void power_supply_changed(struct power_supply *psy);
extern int power_supply_am_i_supplied(struct power_supply *psy);
int power_supply_get_property_from_supplier(struct power_supply *psy,
					    enum power_supply_property psp,
					    union power_supply_propval *val);
extern int power_supply_set_battery_charged(struct power_supply *psy);

static inline bool
power_supply_supports_maintenance_charging(struct power_supply_battery_info *info)
{
	struct power_supply_maintenance_charge_table *mt;

	mt = power_supply_get_maintenance_charging_setting(info, 0);

	return (mt != NULL);
}

static inline bool
power_supply_supports_vbat2ri(struct power_supply_battery_info *info)
{
	return ((info->vbat2ri_discharging != NULL) &&
		info->vbat2ri_discharging_size > 0);
}

static inline bool
power_supply_supports_temp2ri(struct power_supply_battery_info *info)
{
	return ((info->resist_table != NULL) &&
		info->resist_table_size > 0);
}

#ifdef CONFIG_POWER_SUPPLY
extern int power_supply_is_system_supplied(void);
#else
static inline int power_supply_is_system_supplied(void) { return -ENOSYS; }
#endif

extern int power_supply_get_property(struct power_supply *psy,
			    enum power_supply_property psp,
			    union power_supply_propval *val);
#if IS_ENABLED(CONFIG_POWER_SUPPLY)
extern int power_supply_set_property(struct power_supply *psy,
			    enum power_supply_property psp,
			    const union power_supply_propval *val);
#else
static inline int power_supply_set_property(struct power_supply *psy,
			    enum power_supply_property psp,
			    const union power_supply_propval *val)
{ return 0; }
#endif
extern int power_supply_property_is_writeable(struct power_supply *psy,
					enum power_supply_property psp);
extern void power_supply_external_power_changed(struct power_supply *psy);

extern struct power_supply *__must_check
power_supply_register(struct device *parent,
				 const struct power_supply_desc *desc,
				 const struct power_supply_config *cfg);
extern struct power_supply *__must_check
power_supply_register_no_ws(struct device *parent,
				 const struct power_supply_desc *desc,
				 const struct power_supply_config *cfg);
extern struct power_supply *__must_check
devm_power_supply_register(struct device *parent,
				 const struct power_supply_desc *desc,
				 const struct power_supply_config *cfg);
extern struct power_supply *__must_check
devm_power_supply_register_no_ws(struct device *parent,
				 const struct power_supply_desc *desc,
				 const struct power_supply_config *cfg);
extern void power_supply_unregister(struct power_supply *psy);
extern int power_supply_powers(struct power_supply *psy, struct device *dev);

#define to_power_supply(device) container_of(device, struct power_supply, dev)

extern void *power_supply_get_drvdata(struct power_supply *psy);

extern struct class *power_supply_class;

static inline bool power_supply_is_amp_property(enum power_supply_property psp)
{
	switch (psp) {
	case POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN:
	case POWER_SUPPLY_PROP_CHARGE_EMPTY_DESIGN:
	case POWER_SUPPLY_PROP_CHARGE_FULL:
	case POWER_SUPPLY_PROP_CHARGE_EMPTY:
	case POWER_SUPPLY_PROP_CHARGE_NOW:
	case POWER_SUPPLY_PROP_CHARGE_AVG:
	case POWER_SUPPLY_PROP_CHARGE_COUNTER:
	case POWER_SUPPLY_PROP_PRECHARGE_CURRENT:
	case POWER_SUPPLY_PROP_CHARGE_TERM_CURRENT:
	case POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT:
	case POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX:
	case POWER_SUPPLY_PROP_CURRENT_MAX:
	case POWER_SUPPLY_PROP_CURRENT_NOW:
	case POWER_SUPPLY_PROP_CURRENT_AVG:
	case POWER_SUPPLY_PROP_CURRENT_BOOT:
		return true;
	default:
		break;
	}

	return false;
}

static inline bool power_supply_is_watt_property(enum power_supply_property psp)
{
	switch (psp) {
	case POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN:
	case POWER_SUPPLY_PROP_ENERGY_EMPTY_DESIGN:
	case POWER_SUPPLY_PROP_ENERGY_FULL:
	case POWER_SUPPLY_PROP_ENERGY_EMPTY:
	case POWER_SUPPLY_PROP_ENERGY_NOW:
	case POWER_SUPPLY_PROP_ENERGY_AVG:
	case POWER_SUPPLY_PROP_VOLTAGE_MAX:
	case POWER_SUPPLY_PROP_VOLTAGE_MIN:
	case POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN:
	case POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN:
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
	case POWER_SUPPLY_PROP_VOLTAGE_AVG:
	case POWER_SUPPLY_PROP_VOLTAGE_OCV:
	case POWER_SUPPLY_PROP_VOLTAGE_BOOT:
	case POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE:
	case POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE_MAX:
	case POWER_SUPPLY_PROP_POWER_NOW:
		return true;
	default:
		break;
	}

	return false;
}

#ifdef CONFIG_POWER_SUPPLY_HWMON
int power_supply_add_hwmon_sysfs(struct power_supply *psy);
void power_supply_remove_hwmon_sysfs(struct power_supply *psy);
#else
static inline int power_supply_add_hwmon_sysfs(struct power_supply *psy)
{
	return 0;
}

static inline
void power_supply_remove_hwmon_sysfs(struct power_supply *psy) {}
#endif

#ifdef CONFIG_SYSFS
ssize_t power_supply_charge_behaviour_show(struct device *dev,
					   unsigned int available_behaviours,
					   enum power_supply_charge_behaviour behaviour,
					   char *buf);

int power_supply_charge_behaviour_parse(unsigned int available_behaviours, const char *buf);
#else
static inline
ssize_t power_supply_charge_behaviour_show(struct device *dev,
					   unsigned int available_behaviours,
					   enum power_supply_charge_behaviour behaviour,
					   char *buf)
{
	return -EOPNOTSUPP;
}

static inline int power_supply_charge_behaviour_parse(unsigned int available_behaviours,
						      const char *buf)
{
	return -EOPNOTSUPP;
}
#endif

#endif 
