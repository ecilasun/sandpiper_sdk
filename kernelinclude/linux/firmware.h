/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_FIRMWARE_H
#define _LINUX_FIRMWARE_H

#include <linux/types.h>
#include <linux/compiler.h>
#include <linux/gfp.h>

#define FW_ACTION_NOUEVENT 0
#define FW_ACTION_UEVENT 1

struct firmware {
	size_t size;
	const u8 *data;

	
	void *priv;
};


enum fw_upload_err {
	FW_UPLOAD_ERR_NONE,
	FW_UPLOAD_ERR_HW_ERROR,
	FW_UPLOAD_ERR_TIMEOUT,
	FW_UPLOAD_ERR_CANCELED,
	FW_UPLOAD_ERR_BUSY,
	FW_UPLOAD_ERR_INVALID_SIZE,
	FW_UPLOAD_ERR_RW_ERROR,
	FW_UPLOAD_ERR_WEAROUT,
	FW_UPLOAD_ERR_MAX
};

struct fw_upload {
	void *dd_handle; 
	void *priv;	 
};


struct fw_upload_ops {
	enum fw_upload_err (*prepare)(struct fw_upload *fw_upload,
				      const u8 *data, u32 size);
	enum fw_upload_err (*write)(struct fw_upload *fw_upload,
				    const u8 *data, u32 offset,
				    u32 size, u32 *written);
	enum fw_upload_err (*poll_complete)(struct fw_upload *fw_upload);
	void (*cancel)(struct fw_upload *fw_upload);
	void (*cleanup)(struct fw_upload *fw_upload);
};

struct module;
struct device;


#ifdef CONFIG_FW_LOADER
bool firmware_request_builtin(struct firmware *fw, const char *name);
#else
static inline bool firmware_request_builtin(struct firmware *fw,
					    const char *name)
{
	return false;
}
#endif

#if IS_REACHABLE(CONFIG_FW_LOADER)
int request_firmware(const struct firmware **fw, const char *name,
		     struct device *device);
int firmware_request_nowarn(const struct firmware **fw, const char *name,
			    struct device *device);
int firmware_request_platform(const struct firmware **fw, const char *name,
			      struct device *device);
int request_firmware_nowait(
	struct module *module, bool uevent,
	const char *name, struct device *device, gfp_t gfp, void *context,
	void (*cont)(const struct firmware *fw, void *context));
int request_firmware_direct(const struct firmware **fw, const char *name,
			    struct device *device);
int request_firmware_into_buf(const struct firmware **firmware_p,
	const char *name, struct device *device, void *buf, size_t size);
int request_partial_firmware_into_buf(const struct firmware **firmware_p,
				      const char *name, struct device *device,
				      void *buf, size_t size, size_t offset);

void release_firmware(const struct firmware *fw);
#else
static inline int request_firmware(const struct firmware **fw,
				   const char *name,
				   struct device *device)
{
	return -EINVAL;
}

static inline int firmware_request_nowarn(const struct firmware **fw,
					  const char *name,
					  struct device *device)
{
	return -EINVAL;
}

static inline int firmware_request_platform(const struct firmware **fw,
					    const char *name,
					    struct device *device)
{
	return -EINVAL;
}

static inline int request_firmware_nowait(
	struct module *module, bool uevent,
	const char *name, struct device *device, gfp_t gfp, void *context,
	void (*cont)(const struct firmware *fw, void *context))
{
	return -EINVAL;
}

static inline void release_firmware(const struct firmware *fw)
{
}

static inline int request_firmware_direct(const struct firmware **fw,
					  const char *name,
					  struct device *device)
{
	return -EINVAL;
}

static inline int request_firmware_into_buf(const struct firmware **firmware_p,
	const char *name, struct device *device, void *buf, size_t size)
{
	return -EINVAL;
}

static inline int request_partial_firmware_into_buf
					(const struct firmware **firmware_p,
					 const char *name,
					 struct device *device,
					 void *buf, size_t size, size_t offset)
{
	return -EINVAL;
}

#endif

#ifdef CONFIG_FW_UPLOAD

struct fw_upload *
firmware_upload_register(struct module *module, struct device *parent,
			 const char *name, const struct fw_upload_ops *ops,
			 void *dd_handle);
void firmware_upload_unregister(struct fw_upload *fw_upload);

#else

static inline struct fw_upload *
firmware_upload_register(struct module *module, struct device *parent,
			 const char *name, const struct fw_upload_ops *ops,
			 void *dd_handle)
{
		return ERR_PTR(-EINVAL);
}

static inline void firmware_upload_unregister(struct fw_upload *fw_upload)
{
}

#endif

int firmware_request_cache(struct device *device, const char *name);

#endif
