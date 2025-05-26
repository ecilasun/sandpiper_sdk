/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _PLDMFW_H_
#define _PLDMFW_H_

#include <linux/list.h>
#include <linux/firmware.h>

#define PLDM_DEVICE_UPDATE_CONTINUE_AFTER_FAIL BIT(0)

#define PLDM_STRING_TYPE_UNKNOWN	0
#define PLDM_STRING_TYPE_ASCII		1
#define PLDM_STRING_TYPE_UTF8		2
#define PLDM_STRING_TYPE_UTF16		3
#define PLDM_STRING_TYPE_UTF16LE	4
#define PLDM_STRING_TYPE_UTF16BE	5

struct pldmfw_record {
	struct list_head entry;

	
	struct list_head descs;

	
	const u8 *version_string;
	u8 version_type;
	u8 version_len;

	
	u16 package_data_len;

	
	u32 device_update_flags;

	
	const u8 *package_data;

	
	unsigned long *component_bitmap;
	u16 component_bitmap_len;
};


#define PLDM_DESC_ID_PCI_VENDOR_ID	0x0000
#define PLDM_DESC_ID_IANA_ENTERPRISE_ID	0x0001
#define PLDM_DESC_ID_UUID		0x0002
#define PLDM_DESC_ID_PNP_VENDOR_ID	0x0003
#define PLDM_DESC_ID_ACPI_VENDOR_ID	0x0004
#define PLDM_DESC_ID_PCI_DEVICE_ID	0x0100
#define PLDM_DESC_ID_PCI_SUBVENDOR_ID	0x0101
#define PLDM_DESC_ID_PCI_SUBDEV_ID	0x0102
#define PLDM_DESC_ID_PCI_REVISION_ID	0x0103
#define PLDM_DESC_ID_PNP_PRODUCT_ID	0x0104
#define PLDM_DESC_ID_ACPI_PRODUCT_ID	0x0105
#define PLDM_DESC_ID_VENDOR_DEFINED	0xFFFF

struct pldmfw_desc_tlv {
	struct list_head entry;

	const u8 *data;
	u16 type;
	u16 size;
};

#define PLDM_CLASSIFICATION_UNKNOWN		0x0000
#define PLDM_CLASSIFICATION_OTHER		0x0001
#define PLDM_CLASSIFICATION_DRIVER		0x0002
#define PLDM_CLASSIFICATION_CONFIG_SW		0x0003
#define PLDM_CLASSIFICATION_APP_SW		0x0004
#define PLDM_CLASSIFICATION_INSTRUMENTATION	0x0005
#define PLDM_CLASSIFICATION_BIOS		0x0006
#define PLDM_CLASSIFICATION_DIAGNOSTIC_SW	0x0007
#define PLDM_CLASSIFICATION_OS			0x0008
#define PLDM_CLASSIFICATION_MIDDLEWARE		0x0009
#define PLDM_CLASSIFICATION_FIRMWARE		0x000A
#define PLDM_CLASSIFICATION_CODE		0x000B
#define PLDM_CLASSIFICATION_SERVICE_PACK	0x000C
#define PLDM_CLASSIFICATION_SOFTWARE_BUNDLE	0x000D

#define PLDM_ACTIVATION_METHOD_AUTO		BIT(0)
#define PLDM_ACTIVATION_METHOD_SELF_CONTAINED	BIT(1)
#define PLDM_ACTIVATION_METHOD_MEDIUM_SPECIFIC	BIT(2)
#define PLDM_ACTIVATION_METHOD_REBOOT		BIT(3)
#define PLDM_ACTIVATION_METHOD_DC_CYCLE		BIT(4)
#define PLDM_ACTIVATION_METHOD_AC_CYCLE		BIT(5)

#define PLDMFW_COMPONENT_OPTION_FORCE_UPDATE		BIT(0)
#define PLDMFW_COMPONENT_OPTION_USE_COMPARISON_STAMP	BIT(1)

struct pldmfw_component {
	struct list_head entry;

	
	u16 classification;
	u16 identifier;

	u16 options;
	u16 activation_method;

	u32 comparison_stamp;

	u32 component_size;
	const u8 *component_data;

	
	const u8 *version_string;
	u8 version_type;
	u8 version_len;

	
	u8 index;

};


#define PLDM_TRANSFER_FLAG_START		BIT(0)
#define PLDM_TRANSFER_FLAG_MIDDLE		BIT(1)
#define PLDM_TRANSFER_FLAG_END			BIT(2)

struct pldmfw_ops;


struct pldmfw {
	const struct pldmfw_ops *ops;
	struct device *dev;
};

bool pldmfw_op_pci_match_record(struct pldmfw *context, struct pldmfw_record *record);


struct pldmfw_ops {
	bool (*match_record)(struct pldmfw *context, struct pldmfw_record *record);
	int (*send_package_data)(struct pldmfw *context, const u8 *data, u16 length);
	int (*send_component_table)(struct pldmfw *context, struct pldmfw_component *component,
				    u8 transfer_flag);
	int (*flash_component)(struct pldmfw *context, struct pldmfw_component *component);
	int (*finalize_update)(struct pldmfw *context);
};

int pldmfw_flash_image(struct pldmfw *context, const struct firmware *fw);

#endif
