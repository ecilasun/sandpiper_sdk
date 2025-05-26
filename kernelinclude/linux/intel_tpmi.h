/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _INTEL_TPMI_H_
#define _INTEL_TPMI_H_


struct intel_tpmi_plat_info {
	u8 package_id;
	u8 bus_number;
	u8 device_number;
	u8 function_number;
};

struct intel_tpmi_plat_info *tpmi_get_platform_data(struct auxiliary_device *auxdev);
struct resource *tpmi_get_resource_at_index(struct auxiliary_device *auxdev, int index);
int tpmi_get_resource_count(struct auxiliary_device *auxdev);

int tpmi_get_feature_status(struct auxiliary_device *auxdev, int feature_id, int *locked,
			    int *disabled);
#endif
