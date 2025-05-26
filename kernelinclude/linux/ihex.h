/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_IHEX_H__
#define __LINUX_IHEX_H__

#include <linux/types.h>
#include <linux/firmware.h>
#include <linux/device.h>


struct ihex_binrec {
	__be32 addr;
	__be16 len;
	uint8_t data[];
} __attribute__((packed));

static inline uint16_t ihex_binrec_size(const struct ihex_binrec *p)
{
	return be16_to_cpu(p->len) + sizeof(*p);
}


static inline const struct ihex_binrec *
__ihex_next_binrec(const struct ihex_binrec *rec)
{
	const void *p = rec;

	return p + ALIGN(ihex_binrec_size(rec), 4);
}

static inline const struct ihex_binrec *
ihex_next_binrec(const struct ihex_binrec *rec)
{
	rec = __ihex_next_binrec(rec);

	return be16_to_cpu(rec->len) ? rec : NULL;
}


static inline int ihex_validate_fw(const struct firmware *fw)
{
	const struct ihex_binrec *end, *rec;

	rec = (const void *)fw->data;
	end = (const void *)&fw->data[fw->size - sizeof(*end)];

	for (; rec <= end; rec = __ihex_next_binrec(rec)) {
		
		if (rec == end && !be16_to_cpu(rec->len))
			return 0;
	}
	return -EINVAL;
}


static inline int request_ihex_firmware(const struct firmware **fw,
					const char *fw_name,
					struct device *dev)
{
	const struct firmware *lfw;
	int ret;

	ret = request_firmware(&lfw, fw_name, dev);
	if (ret)
		return ret;
	ret = ihex_validate_fw(lfw);
	if (ret) {
		dev_err(dev, "Firmware \"%s\" not valid IHEX records\n",
			fw_name);
		release_firmware(lfw);
		return ret;
	}
	*fw = lfw;
	return 0;
}
#endif 
