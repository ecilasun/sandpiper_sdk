/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LINUX_SYSFB_H
#define _LINUX_SYSFB_H



#include <linux/kernel.h>
#include <linux/platform_data/simplefb.h>

struct screen_info;

enum {
	M_I17,		
	M_I20,		
	M_I20_SR,	
	M_I24,		
	M_I24_8_1,	
	M_I24_10_1,	
	M_I27_11_1,	
	M_MINI,		
	M_MINI_3_1,	
	M_MINI_4_1,	
	M_MB,		
	M_MB_2,		
	M_MB_3,		
	M_MB_5_1,	
	M_MB_6_1,	
	M_MB_7_1,	
	M_MB_SR,	
	M_MBA,		
	M_MBA_3,	
	M_MBP,		
	M_MBP_2,	
	M_MBP_2_2,	
	M_MBP_SR,	
	M_MBP_4,	
	M_MBP_5_1,	
	M_MBP_5_2,	
	M_MBP_5_3,	
	M_MBP_6_1,	
	M_MBP_6_2,	
	M_MBP_7_1,	
	M_MBP_8_2,	
	M_UNKNOWN	
};

struct efifb_dmi_info {
	char *optname;
	unsigned long base;
	int stride;
	int width;
	int height;
	int flags;
};

#ifdef CONFIG_SYSFB

void sysfb_disable(void);

#else 

static inline void sysfb_disable(void)
{
}

#endif 

#ifdef CONFIG_EFI

extern struct efifb_dmi_info efifb_dmi_list[];
void sysfb_apply_efi_quirks(void);
void sysfb_set_efifb_fwnode(struct platform_device *pd);

#else 

static inline void sysfb_apply_efi_quirks(void)
{
}

static inline void sysfb_set_efifb_fwnode(struct platform_device *pd)
{
}

#endif 

#ifdef CONFIG_SYSFB_SIMPLEFB

bool sysfb_parse_mode(const struct screen_info *si,
		      struct simplefb_platform_data *mode);
struct platform_device *sysfb_create_simplefb(const struct screen_info *si,
					      const struct simplefb_platform_data *mode);

#else 

static inline bool sysfb_parse_mode(const struct screen_info *si,
				    struct simplefb_platform_data *mode)
{
	return false;
}

static inline struct platform_device *sysfb_create_simplefb(const struct screen_info *si,
							    const struct simplefb_platform_data *mode)
{
	return ERR_PTR(-EINVAL);
}

#endif 

#endif 
