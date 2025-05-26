/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef LINUX_APPLE_GMUX_H
#define LINUX_APPLE_GMUX_H

#include <linux/acpi.h>
#include <linux/io.h>
#include <linux/pnp.h>

#define GMUX_ACPI_HID "APP000B"


#define GMUX_PORT_VERSION_MAJOR		0x04
#define GMUX_PORT_VERSION_MINOR		0x05
#define GMUX_PORT_VERSION_RELEASE	0x06
#define GMUX_PORT_SWITCH_DISPLAY	0x10
#define GMUX_PORT_SWITCH_GET_DISPLAY	0x11
#define GMUX_PORT_INTERRUPT_ENABLE	0x14
#define GMUX_PORT_INTERRUPT_STATUS	0x16
#define GMUX_PORT_SWITCH_DDC		0x28
#define GMUX_PORT_SWITCH_EXTERNAL	0x40
#define GMUX_PORT_SWITCH_GET_EXTERNAL	0x41
#define GMUX_PORT_DISCRETE_POWER	0x50
#define GMUX_PORT_MAX_BRIGHTNESS	0x70
#define GMUX_PORT_BRIGHTNESS		0x74
#define GMUX_PORT_VALUE			0xc2
#define GMUX_PORT_READ			0xd0
#define GMUX_PORT_WRITE			0xd4

#define GMUX_MMIO_PORT_SELECT		0x0e
#define GMUX_MMIO_COMMAND_SEND		0x0f

#define GMUX_MMIO_READ			0x00
#define GMUX_MMIO_WRITE			0x40

#define GMUX_MIN_IO_LEN			(GMUX_PORT_BRIGHTNESS + 4)

enum apple_gmux_type {
	APPLE_GMUX_TYPE_PIO,
	APPLE_GMUX_TYPE_INDEXED,
	APPLE_GMUX_TYPE_MMIO,
};

#if IS_ENABLED(CONFIG_APPLE_GMUX)
static inline bool apple_gmux_is_indexed(unsigned long iostart)
{
	u16 val;

	outb(0xaa, iostart + 0xcc);
	outb(0x55, iostart + 0xcd);
	outb(0x00, iostart + 0xce);

	val = inb(iostart + 0xcc) | (inb(iostart + 0xcd) << 8);
	if (val == 0x55aa)
		return true;

	return false;
}

static inline bool apple_gmux_is_mmio(unsigned long iostart)
{
	u8 __iomem *iomem_base = ioremap(iostart, 16);
	u8 val;

	if (!iomem_base)
		return false;

	
	val = ioread8(iomem_base + GMUX_MMIO_COMMAND_SEND);
	iounmap(iomem_base);
	return (val != 0xff);
}


static inline bool apple_gmux_detect(struct pnp_dev *pnp_dev, enum apple_gmux_type *type_ret)
{
	u8 ver_major, ver_minor, ver_release;
	struct device *dev = NULL;
	struct acpi_device *adev;
	struct resource *res;
	enum apple_gmux_type type = APPLE_GMUX_TYPE_PIO;
	bool ret = false;

	if (!pnp_dev) {
		adev = acpi_dev_get_first_match_dev(GMUX_ACPI_HID, NULL, -1);
		if (!adev)
			return false;

		dev = get_device(acpi_get_first_physical_node(adev));
		acpi_dev_put(adev);
		if (!dev)
			return false;

		pnp_dev = to_pnp_dev(dev);
	}

	res = pnp_get_resource(pnp_dev, IORESOURCE_IO, 0);
	if (res && resource_size(res) >= GMUX_MIN_IO_LEN) {
		
		ver_major = inb(res->start + GMUX_PORT_VERSION_MAJOR);
		ver_minor = inb(res->start + GMUX_PORT_VERSION_MINOR);
		ver_release = inb(res->start + GMUX_PORT_VERSION_RELEASE);
		if (ver_major == 0xff && ver_minor == 0xff && ver_release == 0xff) {
			if (apple_gmux_is_indexed(res->start))
				type = APPLE_GMUX_TYPE_INDEXED;
			else
				goto out;
		}
	} else {
		res = pnp_get_resource(pnp_dev, IORESOURCE_MEM, 0);
		if (res && apple_gmux_is_mmio(res->start))
			type = APPLE_GMUX_TYPE_MMIO;
		else
			goto out;
	}

	if (type_ret)
		*type_ret = type;

	ret = true;
out:
	put_device(dev);
	return ret;
}


static inline bool apple_gmux_present(void)
{
	return acpi_dev_found(GMUX_ACPI_HID);
}

#else  

static inline bool apple_gmux_present(void)
{
	return false;
}

static inline bool apple_gmux_detect(struct pnp_dev *pnp_dev, bool *indexed_ret)
{
	return false;
}

#endif 

#endif 
