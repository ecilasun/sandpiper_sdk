/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __QCOM_SOCINFO_H__
#define __QCOM_SOCINFO_H__


#define SMEM_HW_SW_BUILD_ID		137

#define SMEM_SOCINFO_BUILD_ID_LENGTH	32
#define SMEM_SOCINFO_CHIP_ID_LENGTH	32


struct socinfo {
	__le32 fmt;
	__le32 id;
	__le32 ver;
	char build_id[SMEM_SOCINFO_BUILD_ID_LENGTH];
	
	__le32 raw_id;
	__le32 raw_ver;
	
	__le32 hw_plat;
	
	__le32 plat_ver;
	
	__le32 accessory_chip;
	
	__le32 hw_plat_subtype;
	
	__le32 pmic_model;
	__le32 pmic_die_rev;
	
	__le32 pmic_model_1;
	__le32 pmic_die_rev_1;
	__le32 pmic_model_2;
	__le32 pmic_die_rev_2;
	
	__le32 foundry_id;
	
	__le32 serial_num;
	
	__le32 num_pmics;
	__le32 pmic_array_offset;
	
	__le32 chip_family;
	__le32 raw_device_family;
	__le32 raw_device_num;
	
	__le32 nproduct_id;
	char chip_id[SMEM_SOCINFO_CHIP_ID_LENGTH];
	
	__le32 num_clusters;
	__le32 ncluster_array_offset;
	__le32 num_subset_parts;
	__le32 nsubset_parts_array_offset;
	
	__le32 nmodem_supported;
	
	__le32  feature_code;
	__le32  pcode;
	__le32  npartnamemap_offset;
	__le32  nnum_partname_mapping;
	
	__le32 oem_variant;
	
	__le32 num_kvps;
	__le32 kvps_offset;
	
	__le32 num_func_clusters;
	__le32 boot_cluster;
	__le32 boot_core;
};

#endif
