/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _LINUX_SCMI_PROTOCOL_H
#define _LINUX_SCMI_PROTOCOL_H

#include <linux/bitfield.h>
#include <linux/device.h>
#include <linux/notifier.h>
#include <linux/types.h>

#define SCMI_MAX_STR_SIZE		64
#define SCMI_SHORT_NAME_MAX_SIZE	16
#define SCMI_MAX_NUM_RATES		16


struct scmi_revision_info {
	u16 major_ver;
	u16 minor_ver;
	u8 num_protocols;
	u8 num_agents;
	u32 impl_ver;
	char vendor_id[SCMI_SHORT_NAME_MAX_SIZE];
	char sub_vendor_id[SCMI_SHORT_NAME_MAX_SIZE];
};

struct scmi_clock_info {
	char name[SCMI_MAX_STR_SIZE];
	unsigned int enable_latency;
	bool rate_discrete;
	bool rate_changed_notifications;
	bool rate_change_requested_notifications;
	union {
		struct {
			int num_rates;
			u64 rates[SCMI_MAX_NUM_RATES];
		} list;
		struct {
			u64 min_rate;
			u64 max_rate;
			u64 step_size;
		} range;
	};
};

enum scmi_power_scale {
	SCMI_POWER_BOGOWATTS,
	SCMI_POWER_MILLIWATTS,
	SCMI_POWER_MICROWATTS
};

struct scmi_handle;
struct scmi_device;
struct scmi_protocol_handle;


struct scmi_clk_proto_ops {
	int (*count_get)(const struct scmi_protocol_handle *ph);

	const struct scmi_clock_info __must_check *(*info_get)
		(const struct scmi_protocol_handle *ph, u32 clk_id);
	int (*rate_get)(const struct scmi_protocol_handle *ph, u32 clk_id,
			u64 *rate);
	int (*rate_set)(const struct scmi_protocol_handle *ph, u32 clk_id,
			u64 rate);
	int (*enable)(const struct scmi_protocol_handle *ph, u32 clk_id);
	int (*disable)(const struct scmi_protocol_handle *ph, u32 clk_id);
	int (*enable_atomic)(const struct scmi_protocol_handle *ph, u32 clk_id);
	int (*disable_atomic)(const struct scmi_protocol_handle *ph,
			      u32 clk_id);
};

struct scmi_perf_domain_info {
	char name[SCMI_MAX_STR_SIZE];
	bool set_perf;
};


struct scmi_perf_proto_ops {
	int (*num_domains_get)(const struct scmi_protocol_handle *ph);
	const struct scmi_perf_domain_info __must_check *(*info_get)
		(const struct scmi_protocol_handle *ph, u32 domain);
	int (*limits_set)(const struct scmi_protocol_handle *ph, u32 domain,
			  u32 max_perf, u32 min_perf);
	int (*limits_get)(const struct scmi_protocol_handle *ph, u32 domain,
			  u32 *max_perf, u32 *min_perf);
	int (*level_set)(const struct scmi_protocol_handle *ph, u32 domain,
			 u32 level, bool poll);
	int (*level_get)(const struct scmi_protocol_handle *ph, u32 domain,
			 u32 *level, bool poll);
	int (*device_domain_id)(struct device *dev);
	int (*transition_latency_get)(const struct scmi_protocol_handle *ph,
				      struct device *dev);
	int (*device_opps_add)(const struct scmi_protocol_handle *ph,
			       struct device *dev);
	int (*freq_set)(const struct scmi_protocol_handle *ph, u32 domain,
			unsigned long rate, bool poll);
	int (*freq_get)(const struct scmi_protocol_handle *ph, u32 domain,
			unsigned long *rate, bool poll);
	int (*est_power_get)(const struct scmi_protocol_handle *ph, u32 domain,
			     unsigned long *rate, unsigned long *power);
	bool (*fast_switch_possible)(const struct scmi_protocol_handle *ph,
				     struct device *dev);
	enum scmi_power_scale (*power_scale_get)(const struct scmi_protocol_handle *ph);
};


struct scmi_power_proto_ops {
	int (*num_domains_get)(const struct scmi_protocol_handle *ph);
	const char *(*name_get)(const struct scmi_protocol_handle *ph,
				u32 domain);
#define SCMI_POWER_STATE_TYPE_SHIFT	30
#define SCMI_POWER_STATE_ID_MASK	(BIT(28) - 1)
#define SCMI_POWER_STATE_PARAM(type, id) \
	((((type) & BIT(0)) << SCMI_POWER_STATE_TYPE_SHIFT) | \
		((id) & SCMI_POWER_STATE_ID_MASK))
#define SCMI_POWER_STATE_GENERIC_ON	SCMI_POWER_STATE_PARAM(0, 0)
#define SCMI_POWER_STATE_GENERIC_OFF	SCMI_POWER_STATE_PARAM(1, 0)
	int (*state_set)(const struct scmi_protocol_handle *ph, u32 domain,
			 u32 state);
	int (*state_get)(const struct scmi_protocol_handle *ph, u32 domain,
			 u32 *state);
};


struct scmi_sensor_reading {
	long long value;
	unsigned long long timestamp;
};


struct scmi_range_attrs {
	long long min_range;
	long long max_range;
};


struct scmi_sensor_axis_info {
	unsigned int id;
	unsigned int type;
	int scale;
	char name[SCMI_MAX_STR_SIZE];
	bool extended_attrs;
	unsigned int resolution;
	int exponent;
	struct scmi_range_attrs attrs;
};


struct scmi_sensor_intervals_info {
	bool segmented;
	unsigned int count;
#define SCMI_SENS_INTVL_SEGMENT_LOW	0
#define SCMI_SENS_INTVL_SEGMENT_HIGH	1
#define SCMI_SENS_INTVL_SEGMENT_STEP	2
	unsigned int *desc;
#define SCMI_SENS_INTVL_GET_SECS(x)		FIELD_GET(GENMASK(20, 5), (x))
#define SCMI_SENS_INTVL_GET_EXP(x)					\
	({								\
		int __signed_exp = FIELD_GET(GENMASK(4, 0), (x));	\
									\
		if (__signed_exp & BIT(4))				\
			__signed_exp |= GENMASK(31, 5);			\
		__signed_exp;						\
	})
#define SCMI_MAX_PREALLOC_POOL			16
	unsigned int prealloc_pool[SCMI_MAX_PREALLOC_POOL];
};


struct scmi_sensor_info {
	unsigned int id;
	unsigned int type;
	int scale;
	unsigned int num_trip_points;
	bool async;
	bool update;
	bool timestamped;
	int tstamp_scale;
	unsigned int num_axis;
	struct scmi_sensor_axis_info *axis;
	struct scmi_sensor_intervals_info intervals;
	unsigned int sensor_config;
#define SCMI_SENS_CFG_UPDATE_SECS_MASK		GENMASK(31, 16)
#define SCMI_SENS_CFG_GET_UPDATE_SECS(x)				\
	FIELD_GET(SCMI_SENS_CFG_UPDATE_SECS_MASK, (x))

#define SCMI_SENS_CFG_UPDATE_EXP_MASK		GENMASK(15, 11)
#define SCMI_SENS_CFG_GET_UPDATE_EXP(x)					\
	({								\
		int __signed_exp =					\
			FIELD_GET(SCMI_SENS_CFG_UPDATE_EXP_MASK, (x));	\
									\
		if (__signed_exp & BIT(4))				\
			__signed_exp |= GENMASK(31, 5);			\
		__signed_exp;						\
	})

#define SCMI_SENS_CFG_ROUND_MASK		GENMASK(10, 9)
#define SCMI_SENS_CFG_ROUND_AUTO		2
#define SCMI_SENS_CFG_ROUND_UP			1
#define SCMI_SENS_CFG_ROUND_DOWN		0

#define SCMI_SENS_CFG_TSTAMP_ENABLED_MASK	BIT(1)
#define SCMI_SENS_CFG_TSTAMP_ENABLE		1
#define SCMI_SENS_CFG_TSTAMP_DISABLE		0
#define SCMI_SENS_CFG_IS_TSTAMP_ENABLED(x)				\
	FIELD_GET(SCMI_SENS_CFG_TSTAMP_ENABLED_MASK, (x))

#define SCMI_SENS_CFG_SENSOR_ENABLED_MASK	BIT(0)
#define SCMI_SENS_CFG_SENSOR_ENABLE		1
#define SCMI_SENS_CFG_SENSOR_DISABLE		0
	char name[SCMI_MAX_STR_SIZE];
#define SCMI_SENS_CFG_IS_ENABLED(x)		FIELD_GET(BIT(0), (x))
	bool extended_scalar_attrs;
	unsigned int sensor_power;
	unsigned int resolution;
	int exponent;
	struct scmi_range_attrs scalar_attrs;
};


enum scmi_sensor_class {
	NONE = 0x0,
	UNSPEC = 0x1,
	TEMPERATURE_C = 0x2,
	TEMPERATURE_F = 0x3,
	TEMPERATURE_K = 0x4,
	VOLTAGE = 0x5,
	CURRENT = 0x6,
	POWER = 0x7,
	ENERGY = 0x8,
	CHARGE = 0x9,
	VOLTAMPERE = 0xA,
	NITS = 0xB,
	LUMENS = 0xC,
	LUX = 0xD,
	CANDELAS = 0xE,
	KPA = 0xF,
	PSI = 0x10,
	NEWTON = 0x11,
	CFM = 0x12,
	RPM = 0x13,
	HERTZ = 0x14,
	SECS = 0x15,
	MINS = 0x16,
	HOURS = 0x17,
	DAYS = 0x18,
	WEEKS = 0x19,
	MILS = 0x1A,
	INCHES = 0x1B,
	FEET = 0x1C,
	CUBIC_INCHES = 0x1D,
	CUBIC_FEET = 0x1E,
	METERS = 0x1F,
	CUBIC_CM = 0x20,
	CUBIC_METERS = 0x21,
	LITERS = 0x22,
	FLUID_OUNCES = 0x23,
	RADIANS = 0x24,
	STERADIANS = 0x25,
	REVOLUTIONS = 0x26,
	CYCLES = 0x27,
	GRAVITIES = 0x28,
	OUNCES = 0x29,
	POUNDS = 0x2A,
	FOOT_POUNDS = 0x2B,
	OUNCE_INCHES = 0x2C,
	GAUSS = 0x2D,
	GILBERTS = 0x2E,
	HENRIES = 0x2F,
	FARADS = 0x30,
	OHMS = 0x31,
	SIEMENS = 0x32,
	MOLES = 0x33,
	BECQUERELS = 0x34,
	PPM = 0x35,
	DECIBELS = 0x36,
	DBA = 0x37,
	DBC = 0x38,
	GRAYS = 0x39,
	SIEVERTS = 0x3A,
	COLOR_TEMP_K = 0x3B,
	BITS = 0x3C,
	BYTES = 0x3D,
	WORDS = 0x3E,
	DWORDS = 0x3F,
	QWORDS = 0x40,
	PERCENTAGE = 0x41,
	PASCALS = 0x42,
	COUNTS = 0x43,
	GRAMS = 0x44,
	NEWTON_METERS = 0x45,
	HITS = 0x46,
	MISSES = 0x47,
	RETRIES = 0x48,
	OVERRUNS = 0x49,
	UNDERRUNS = 0x4A,
	COLLISIONS = 0x4B,
	PACKETS = 0x4C,
	MESSAGES = 0x4D,
	CHARS = 0x4E,
	ERRORS = 0x4F,
	CORRECTED_ERRS = 0x50,
	UNCORRECTABLE_ERRS = 0x51,
	SQ_MILS = 0x52,
	SQ_INCHES = 0x53,
	SQ_FEET = 0x54,
	SQ_CM = 0x55,
	SQ_METERS = 0x56,
	RADIANS_SEC = 0x57,
	BPM = 0x58,
	METERS_SEC_SQUARED = 0x59,
	METERS_SEC = 0x5A,
	CUBIC_METERS_SEC = 0x5B,
	MM_MERCURY = 0x5C,
	RADIANS_SEC_SQUARED = 0x5D,
	OEM_UNIT = 0xFF
};


struct scmi_sensor_proto_ops {
	int (*count_get)(const struct scmi_protocol_handle *ph);
	const struct scmi_sensor_info __must_check *(*info_get)
		(const struct scmi_protocol_handle *ph, u32 sensor_id);
	int (*trip_point_config)(const struct scmi_protocol_handle *ph,
				 u32 sensor_id, u8 trip_id, u64 trip_value);
	int (*reading_get)(const struct scmi_protocol_handle *ph, u32 sensor_id,
			   u64 *value);
	int (*reading_get_timestamped)(const struct scmi_protocol_handle *ph,
				       u32 sensor_id, u8 count,
				       struct scmi_sensor_reading *readings);
	int (*config_get)(const struct scmi_protocol_handle *ph,
			  u32 sensor_id, u32 *sensor_config);
	int (*config_set)(const struct scmi_protocol_handle *ph,
			  u32 sensor_id, u32 sensor_config);
};


struct scmi_reset_proto_ops {
	int (*num_domains_get)(const struct scmi_protocol_handle *ph);
	const char *(*name_get)(const struct scmi_protocol_handle *ph,
				u32 domain);
	int (*latency_get)(const struct scmi_protocol_handle *ph, u32 domain);
	int (*reset)(const struct scmi_protocol_handle *ph, u32 domain);
	int (*assert)(const struct scmi_protocol_handle *ph, u32 domain);
	int (*deassert)(const struct scmi_protocol_handle *ph, u32 domain);
};

enum scmi_voltage_level_mode {
	SCMI_VOLTAGE_LEVEL_SET_AUTO,
	SCMI_VOLTAGE_LEVEL_SET_SYNC,
};


struct scmi_voltage_info {
	unsigned int id;
	bool segmented;
	bool negative_volts_allowed;
	bool async_level_set;
	char name[SCMI_MAX_STR_SIZE];
	unsigned int num_levels;
#define SCMI_VOLTAGE_SEGMENT_LOW	0
#define SCMI_VOLTAGE_SEGMENT_HIGH	1
#define SCMI_VOLTAGE_SEGMENT_STEP	2
	int *levels_uv;
};


struct scmi_voltage_proto_ops {
	int (*num_domains_get)(const struct scmi_protocol_handle *ph);
	const struct scmi_voltage_info __must_check *(*info_get)
		(const struct scmi_protocol_handle *ph, u32 domain_id);
	int (*config_set)(const struct scmi_protocol_handle *ph, u32 domain_id,
			  u32 config);
#define	SCMI_VOLTAGE_ARCH_STATE_OFF		0x0
#define	SCMI_VOLTAGE_ARCH_STATE_ON		0x7
	int (*config_get)(const struct scmi_protocol_handle *ph, u32 domain_id,
			  u32 *config);
	int (*level_set)(const struct scmi_protocol_handle *ph, u32 domain_id,
			 enum scmi_voltage_level_mode mode, s32 volt_uV);
	int (*level_get)(const struct scmi_protocol_handle *ph, u32 domain_id,
			 s32 *volt_uV);
};


struct scmi_powercap_info {
	unsigned int id;
	bool notify_powercap_cap_change;
	bool notify_powercap_measurement_change;
	bool async_powercap_cap_set;
	bool powercap_cap_config;
	bool powercap_monitoring;
	bool powercap_pai_config;
	bool powercap_scale_mw;
	bool powercap_scale_uw;
	bool fastchannels;
	char name[SCMI_MAX_STR_SIZE];
	unsigned int min_pai;
	unsigned int max_pai;
	unsigned int pai_step;
	unsigned int min_power_cap;
	unsigned int max_power_cap;
	unsigned int power_cap_step;
	unsigned int sustainable_power;
	unsigned int accuracy;
#define SCMI_POWERCAP_ROOT_ZONE_ID     0xFFFFFFFFUL
	unsigned int parent_id;
	struct scmi_fc_info *fc_info;
};


struct scmi_powercap_proto_ops {
	int (*num_domains_get)(const struct scmi_protocol_handle *ph);
	const struct scmi_powercap_info __must_check *(*info_get)
		(const struct scmi_protocol_handle *ph, u32 domain_id);
	int (*cap_get)(const struct scmi_protocol_handle *ph, u32 domain_id,
		       u32 *power_cap);
	int (*cap_set)(const struct scmi_protocol_handle *ph, u32 domain_id,
		       u32 power_cap, bool ignore_dresp);
	int (*cap_enable_set)(const struct scmi_protocol_handle *ph,
			      u32 domain_id, bool enable);
	int (*cap_enable_get)(const struct scmi_protocol_handle *ph,
			      u32 domain_id, bool *enable);
	int (*pai_get)(const struct scmi_protocol_handle *ph, u32 domain_id,
		       u32 *pai);
	int (*pai_set)(const struct scmi_protocol_handle *ph, u32 domain_id,
		       u32 pai);
	int (*measurements_get)(const struct scmi_protocol_handle *ph,
				u32 domain_id, u32 *average_power, u32 *pai);
	int (*measurements_threshold_set)(const struct scmi_protocol_handle *ph,
					  u32 domain_id, u32 power_thresh_low,
					  u32 power_thresh_high);
	int (*measurements_threshold_get)(const struct scmi_protocol_handle *ph,
					  u32 domain_id, u32 *power_thresh_low,
					  u32 *power_thresh_high);
};


struct scmi_notify_ops {
	int (*devm_event_notifier_register)(struct scmi_device *sdev,
					    u8 proto_id, u8 evt_id,
					    const u32 *src_id,
					    struct notifier_block *nb);
	int (*devm_event_notifier_unregister)(struct scmi_device *sdev,
					      u8 proto_id, u8 evt_id,
					      const u32 *src_id,
					      struct notifier_block *nb);
	int (*event_notifier_register)(const struct scmi_handle *handle,
				       u8 proto_id, u8 evt_id,
				       const u32 *src_id,
				       struct notifier_block *nb);
	int (*event_notifier_unregister)(const struct scmi_handle *handle,
					 u8 proto_id, u8 evt_id,
					 const u32 *src_id,
					 struct notifier_block *nb);
};


struct scmi_handle {
	struct device *dev;
	struct scmi_revision_info *version;

	int __must_check (*devm_protocol_acquire)(struct scmi_device *sdev,
						  u8 proto);
	const void __must_check *
		(*devm_protocol_get)(struct scmi_device *sdev, u8 proto,
				     struct scmi_protocol_handle **ph);
	void (*devm_protocol_put)(struct scmi_device *sdev, u8 proto);
	bool (*is_transport_atomic)(const struct scmi_handle *handle,
				    unsigned int *atomic_threshold);

	const struct scmi_notify_ops *notify_ops;
};

enum scmi_std_protocol {
	SCMI_PROTOCOL_BASE = 0x10,
	SCMI_PROTOCOL_POWER = 0x11,
	SCMI_PROTOCOL_SYSTEM = 0x12,
	SCMI_PROTOCOL_PERF = 0x13,
	SCMI_PROTOCOL_CLOCK = 0x14,
	SCMI_PROTOCOL_SENSOR = 0x15,
	SCMI_PROTOCOL_RESET = 0x16,
	SCMI_PROTOCOL_VOLTAGE = 0x17,
	SCMI_PROTOCOL_POWERCAP = 0x18,
};

enum scmi_system_events {
	SCMI_SYSTEM_SHUTDOWN,
	SCMI_SYSTEM_COLDRESET,
	SCMI_SYSTEM_WARMRESET,
	SCMI_SYSTEM_POWERUP,
	SCMI_SYSTEM_SUSPEND,
	SCMI_SYSTEM_MAX
};

struct scmi_device {
	u32 id;
	u8 protocol_id;
	const char *name;
	struct device dev;
	struct scmi_handle *handle;
};

#define to_scmi_dev(d) container_of(d, struct scmi_device, dev)

struct scmi_device_id {
	u8 protocol_id;
	const char *name;
};

struct scmi_driver {
	const char *name;
	int (*probe)(struct scmi_device *sdev);
	void (*remove)(struct scmi_device *sdev);
	const struct scmi_device_id *id_table;

	struct device_driver driver;
};

#define to_scmi_driver(d) container_of(d, struct scmi_driver, driver)

#if IS_REACHABLE(CONFIG_ARM_SCMI_PROTOCOL)
int scmi_driver_register(struct scmi_driver *driver,
			 struct module *owner, const char *mod_name);
void scmi_driver_unregister(struct scmi_driver *driver);
#else
static inline int
scmi_driver_register(struct scmi_driver *driver, struct module *owner,
		     const char *mod_name)
{
	return -EINVAL;
}

static inline void scmi_driver_unregister(struct scmi_driver *driver) {}
#endif 

#define scmi_register(driver) \
	scmi_driver_register(driver, THIS_MODULE, KBUILD_MODNAME)
#define scmi_unregister(driver) \
	scmi_driver_unregister(driver)


#define module_scmi_driver(__scmi_driver)	\
	module_driver(__scmi_driver, scmi_register, scmi_unregister)


#define module_scmi_protocol(__scmi_protocol)	\
	module_driver(__scmi_protocol,		\
		      scmi_protocol_register, scmi_protocol_unregister)

struct scmi_protocol;
int scmi_protocol_register(const struct scmi_protocol *proto);
void scmi_protocol_unregister(const struct scmi_protocol *proto);


enum scmi_notification_events {
	SCMI_EVENT_POWER_STATE_CHANGED = 0x0,
	SCMI_EVENT_CLOCK_RATE_CHANGED = 0x0,
	SCMI_EVENT_CLOCK_RATE_CHANGE_REQUESTED = 0x1,
	SCMI_EVENT_PERFORMANCE_LIMITS_CHANGED = 0x0,
	SCMI_EVENT_PERFORMANCE_LEVEL_CHANGED = 0x1,
	SCMI_EVENT_SENSOR_TRIP_POINT_EVENT = 0x0,
	SCMI_EVENT_SENSOR_UPDATE = 0x1,
	SCMI_EVENT_RESET_ISSUED = 0x0,
	SCMI_EVENT_BASE_ERROR_EVENT = 0x0,
	SCMI_EVENT_SYSTEM_POWER_STATE_NOTIFIER = 0x0,
	SCMI_EVENT_POWERCAP_CAP_CHANGED = 0x0,
	SCMI_EVENT_POWERCAP_MEASUREMENTS_CHANGED = 0x1,
};

struct scmi_power_state_changed_report {
	ktime_t		timestamp;
	unsigned int	agent_id;
	unsigned int	domain_id;
	unsigned int	power_state;
};

struct scmi_clock_rate_notif_report {
	ktime_t			timestamp;
	unsigned int		agent_id;
	unsigned int		clock_id;
	unsigned long long	rate;
};

struct scmi_system_power_state_notifier_report {
	ktime_t		timestamp;
	unsigned int	agent_id;
#define SCMI_SYSPOWER_IS_REQUEST_GRACEFUL(flags)	((flags) & BIT(0))
	unsigned int	flags;
	unsigned int	system_state;
	unsigned int	timeout;
};

struct scmi_perf_limits_report {
	ktime_t		timestamp;
	unsigned int	agent_id;
	unsigned int	domain_id;
	unsigned int	range_max;
	unsigned int	range_min;
};

struct scmi_perf_level_report {
	ktime_t		timestamp;
	unsigned int	agent_id;
	unsigned int	domain_id;
	unsigned int	performance_level;
};

struct scmi_sensor_trip_point_report {
	ktime_t		timestamp;
	unsigned int	agent_id;
	unsigned int	sensor_id;
	unsigned int	trip_point_desc;
};

struct scmi_sensor_update_report {
	ktime_t				timestamp;
	unsigned int			agent_id;
	unsigned int			sensor_id;
	unsigned int			readings_count;
	struct scmi_sensor_reading	readings[];
};

struct scmi_reset_issued_report {
	ktime_t		timestamp;
	unsigned int	agent_id;
	unsigned int	domain_id;
	unsigned int	reset_state;
};

struct scmi_base_error_report {
	ktime_t			timestamp;
	unsigned int		agent_id;
	bool			fatal;
	unsigned int		cmd_count;
	unsigned long long	reports[];
};

struct scmi_powercap_cap_changed_report {
	ktime_t		timestamp;
	unsigned int	agent_id;
	unsigned int	domain_id;
	unsigned int	power_cap;
	unsigned int	pai;
};

struct scmi_powercap_meas_changed_report {
	ktime_t		timestamp;
	unsigned int	agent_id;
	unsigned int	domain_id;
	unsigned int	power;
};
#endif 
