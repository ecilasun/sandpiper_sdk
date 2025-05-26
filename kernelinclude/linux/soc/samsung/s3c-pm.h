/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_SOC_SAMSUNG_S3C_PM_H
#define __LINUX_SOC_SAMSUNG_S3C_PM_H __FILE__

#include <linux/types.h>



#define S3C_PMDBG(fmt...) pr_debug(fmt)

static inline void s3c_pm_save_uarts(bool is_s3c24xx) { }
static inline void s3c_pm_restore_uarts(bool is_s3c24xx) { }



#ifdef CONFIG_SAMSUNG_PM_CHECK
extern void s3c_pm_check_prepare(void);
extern void s3c_pm_check_restore(void);
extern void s3c_pm_check_cleanup(void);
extern void s3c_pm_check_store(void);
#else
#define s3c_pm_check_prepare() do { } while (0)
#define s3c_pm_check_restore() do { } while (0)
#define s3c_pm_check_cleanup() do { } while (0)
#define s3c_pm_check_store()   do { } while (0)
#endif

#endif
