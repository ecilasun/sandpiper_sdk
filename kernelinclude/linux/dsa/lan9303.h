
#include <linux/if_ether.h>

struct lan9303;

struct lan9303_phy_ops {
	
	int	(*phy_read)(struct lan9303 *chip, int port, int regnum);
	int	(*phy_write)(struct lan9303 *chip, int port,
			     int regnum, u16 val);
};

#define LAN9303_NUM_ALR_RECORDS 512
struct lan9303_alr_cache_entry {
	u8  mac_addr[ETH_ALEN];
	u8  port_map;         
	u8  stp_override;     
};

struct lan9303 {
	struct device *dev;
	struct regmap *regmap;
	struct regmap_irq_chip_data *irq_data;
	struct gpio_desc *reset_gpio;
	u32 reset_duration; 
	int phy_addr_base;
	struct dsa_switch *ds;
	struct mutex indirect_mutex; 
	struct mutex alr_mutex; 
	const struct lan9303_phy_ops *ops;
	bool is_bridged; 

	
	u32 swe_port_state;
	
	struct lan9303_alr_cache_entry alr_cache[LAN9303_NUM_ALR_RECORDS];
};
