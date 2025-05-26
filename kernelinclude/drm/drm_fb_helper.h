
#ifndef DRM_FB_HELPER_H
#define DRM_FB_HELPER_H

struct drm_clip_rect;
struct drm_fb_helper;

#include <linux/fb.h>

#include <drm/drm_client.h>


struct drm_fb_helper_surface_size {
	u32 fb_width;
	u32 fb_height;
	u32 surface_width;
	u32 surface_height;
	u32 surface_bpp;
	u32 surface_depth;
};


struct drm_fb_helper_funcs {
	
	int (*fb_probe)(struct drm_fb_helper *helper,
			struct drm_fb_helper_surface_size *sizes);

	
	int (*fb_dirty)(struct drm_fb_helper *helper, struct drm_clip_rect *clip);
};


struct drm_fb_helper {
	
	struct drm_client_dev client;

	
	struct drm_client_buffer *buffer;

	struct drm_framebuffer *fb;
	struct drm_device *dev;
	const struct drm_fb_helper_funcs *funcs;
	struct fb_info *info;
	u32 pseudo_palette[17];
	struct drm_clip_rect damage_clip;
	spinlock_t damage_lock;
	struct work_struct damage_work;
	struct work_struct resume_work;

	
	struct mutex lock;

	
	struct list_head kernel_fb_list;

	
	bool delayed_hotplug;

	
	bool deferred_setup;

	
	int preferred_bpp;

#ifdef CONFIG_FB_DEFERRED_IO
	
	struct fb_deferred_io fbdefio;
#endif
};

static inline struct drm_fb_helper *
drm_fb_helper_from_client(struct drm_client_dev *client)
{
	return container_of(client, struct drm_fb_helper, client);
}


#define DRM_FB_HELPER_DEFAULT_OPS \
	.fb_check_var	= drm_fb_helper_check_var, \
	.fb_set_par	= drm_fb_helper_set_par, \
	.fb_setcmap	= drm_fb_helper_setcmap, \
	.fb_blank	= drm_fb_helper_blank, \
	.fb_pan_display	= drm_fb_helper_pan_display, \
	.fb_debug_enter = drm_fb_helper_debug_enter, \
	.fb_debug_leave = drm_fb_helper_debug_leave, \
	.fb_ioctl	= drm_fb_helper_ioctl

#ifdef CONFIG_DRM_FBDEV_EMULATION
void drm_fb_helper_prepare(struct drm_device *dev, struct drm_fb_helper *helper,
			   unsigned int preferred_bpp,
			   const struct drm_fb_helper_funcs *funcs);
void drm_fb_helper_unprepare(struct drm_fb_helper *fb_helper);
int drm_fb_helper_init(struct drm_device *dev, struct drm_fb_helper *helper);
void drm_fb_helper_fini(struct drm_fb_helper *helper);
int drm_fb_helper_blank(int blank, struct fb_info *info);
int drm_fb_helper_pan_display(struct fb_var_screeninfo *var,
			      struct fb_info *info);
int drm_fb_helper_set_par(struct fb_info *info);
int drm_fb_helper_check_var(struct fb_var_screeninfo *var,
			    struct fb_info *info);

int drm_fb_helper_restore_fbdev_mode_unlocked(struct drm_fb_helper *fb_helper);

struct fb_info *drm_fb_helper_alloc_info(struct drm_fb_helper *fb_helper);
void drm_fb_helper_release_info(struct drm_fb_helper *fb_helper);
void drm_fb_helper_unregister_info(struct drm_fb_helper *fb_helper);
void drm_fb_helper_fill_info(struct fb_info *info,
			     struct drm_fb_helper *fb_helper,
			     struct drm_fb_helper_surface_size *sizes);

void drm_fb_helper_damage_range(struct fb_info *info, off_t off, size_t len);
void drm_fb_helper_damage_area(struct fb_info *info, u32 x, u32 y, u32 width, u32 height);

void drm_fb_helper_deferred_io(struct fb_info *info, struct list_head *pagereflist);

void drm_fb_helper_set_suspend(struct drm_fb_helper *fb_helper, bool suspend);
void drm_fb_helper_set_suspend_unlocked(struct drm_fb_helper *fb_helper,
					bool suspend);

int drm_fb_helper_setcmap(struct fb_cmap *cmap, struct fb_info *info);

int drm_fb_helper_ioctl(struct fb_info *info, unsigned int cmd,
			unsigned long arg);

int drm_fb_helper_hotplug_event(struct drm_fb_helper *fb_helper);
int drm_fb_helper_initial_config(struct drm_fb_helper *fb_helper);
int drm_fb_helper_debug_enter(struct fb_info *info);
int drm_fb_helper_debug_leave(struct fb_info *info);

void drm_fb_helper_lastclose(struct drm_device *dev);
void drm_fb_helper_output_poll_changed(struct drm_device *dev);
#else
static inline void drm_fb_helper_prepare(struct drm_device *dev,
					 struct drm_fb_helper *helper,
					 unsigned int preferred_bpp,
					 const struct drm_fb_helper_funcs *funcs)
{
}

static inline void drm_fb_helper_unprepare(struct drm_fb_helper *fb_helper)
{
}

static inline int drm_fb_helper_init(struct drm_device *dev,
		       struct drm_fb_helper *helper)
{
	
	helper->dev = dev;
	dev->fb_helper = helper;

	return 0;
}

static inline void drm_fb_helper_fini(struct drm_fb_helper *helper)
{
	if (helper && helper->dev)
		helper->dev->fb_helper = NULL;
}

static inline int drm_fb_helper_blank(int blank, struct fb_info *info)
{
	return 0;
}

static inline int drm_fb_helper_pan_display(struct fb_var_screeninfo *var,
					    struct fb_info *info)
{
	return 0;
}

static inline int drm_fb_helper_set_par(struct fb_info *info)
{
	return 0;
}

static inline int drm_fb_helper_check_var(struct fb_var_screeninfo *var,
					  struct fb_info *info)
{
	return 0;
}

static inline int
drm_fb_helper_restore_fbdev_mode_unlocked(struct drm_fb_helper *fb_helper)
{
	return 0;
}

static inline struct fb_info *
drm_fb_helper_alloc_info(struct drm_fb_helper *fb_helper)
{
	return NULL;
}

static inline void drm_fb_helper_release_info(struct drm_fb_helper *fb_helper)
{
}

static inline void drm_fb_helper_unregister_info(struct drm_fb_helper *fb_helper)
{
}

static inline void
drm_fb_helper_fill_info(struct fb_info *info,
			struct drm_fb_helper *fb_helper,
			struct drm_fb_helper_surface_size *sizes)
{
}

static inline int drm_fb_helper_setcmap(struct fb_cmap *cmap,
					struct fb_info *info)
{
	return 0;
}

static inline int drm_fb_helper_ioctl(struct fb_info *info, unsigned int cmd,
				      unsigned long arg)
{
	return 0;
}

static inline void drm_fb_helper_deferred_io(struct fb_info *info,
					     struct list_head *pagelist)
{
}

static inline void drm_fb_helper_set_suspend(struct drm_fb_helper *fb_helper,
					     bool suspend)
{
}

static inline void
drm_fb_helper_set_suspend_unlocked(struct drm_fb_helper *fb_helper, bool suspend)
{
}

static inline int drm_fb_helper_hotplug_event(struct drm_fb_helper *fb_helper)
{
	return 0;
}

static inline int drm_fb_helper_initial_config(struct drm_fb_helper *fb_helper)
{
	return 0;
}

static inline int drm_fb_helper_debug_enter(struct fb_info *info)
{
	return 0;
}

static inline int drm_fb_helper_debug_leave(struct fb_info *info)
{
	return 0;
}

static inline void drm_fb_helper_lastclose(struct drm_device *dev)
{
}

static inline void drm_fb_helper_output_poll_changed(struct drm_device *dev)
{
}
#endif

#endif
