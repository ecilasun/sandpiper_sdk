

#ifndef __DRM_FRAMEBUFFER_H__
#define __DRM_FRAMEBUFFER_H__

#include <linux/ctype.h>
#include <linux/list.h>
#include <linux/sched.h>

#include <drm/drm_fourcc.h>
#include <drm/drm_mode_object.h>

struct drm_clip_rect;
struct drm_device;
struct drm_file;
struct drm_framebuffer;
struct drm_gem_object;


struct drm_framebuffer_funcs {
	
	void (*destroy)(struct drm_framebuffer *framebuffer);

	
	int (*create_handle)(struct drm_framebuffer *fb,
			     struct drm_file *file_priv,
			     unsigned int *handle);
	
	int (*dirty)(struct drm_framebuffer *framebuffer,
		     struct drm_file *file_priv, unsigned flags,
		     unsigned color, struct drm_clip_rect *clips,
		     unsigned num_clips);
};


struct drm_framebuffer {
	
	struct drm_device *dev;
	
	struct list_head head;

	
	struct drm_mode_object base;

	
	char comm[TASK_COMM_LEN];

	
	const struct drm_format_info *format;
	
	const struct drm_framebuffer_funcs *funcs;
	
	unsigned int pitches[DRM_FORMAT_MAX_PLANES];
	
	unsigned int offsets[DRM_FORMAT_MAX_PLANES];
	
	uint64_t modifier;
	
	unsigned int width;
	
	unsigned int height;
	
	int flags;
	
	int hot_x;
	
	int hot_y;
	
	struct list_head filp_head;
	
	struct drm_gem_object *obj[DRM_FORMAT_MAX_PLANES];
};

#define obj_to_fb(x) container_of(x, struct drm_framebuffer, base)

int drm_framebuffer_init(struct drm_device *dev,
			 struct drm_framebuffer *fb,
			 const struct drm_framebuffer_funcs *funcs);
struct drm_framebuffer *drm_framebuffer_lookup(struct drm_device *dev,
					       struct drm_file *file_priv,
					       uint32_t id);
void drm_framebuffer_remove(struct drm_framebuffer *fb);
void drm_framebuffer_cleanup(struct drm_framebuffer *fb);
void drm_framebuffer_unregister_private(struct drm_framebuffer *fb);


static inline void drm_framebuffer_get(struct drm_framebuffer *fb)
{
	drm_mode_object_get(&fb->base);
}


static inline void drm_framebuffer_put(struct drm_framebuffer *fb)
{
	drm_mode_object_put(&fb->base);
}


static inline uint32_t drm_framebuffer_read_refcount(const struct drm_framebuffer *fb)
{
	return kref_read(&fb->base.refcount);
}


static inline void drm_framebuffer_assign(struct drm_framebuffer **p,
					  struct drm_framebuffer *fb)
{
	if (fb)
		drm_framebuffer_get(fb);
	if (*p)
		drm_framebuffer_put(*p);
	*p = fb;
}


#define drm_for_each_fb(fb, dev) \
	for (WARN_ON(!mutex_is_locked(&(dev)->mode_config.fb_lock)),		\
	     fb = list_first_entry(&(dev)->mode_config.fb_list,	\
					  struct drm_framebuffer, head);	\
	     &fb->head != (&(dev)->mode_config.fb_list);			\
	     fb = list_next_entry(fb, head))

int drm_framebuffer_plane_width(int width,
				const struct drm_framebuffer *fb, int plane);
int drm_framebuffer_plane_height(int height,
				 const struct drm_framebuffer *fb, int plane);


struct drm_afbc_framebuffer {
	
	struct drm_framebuffer base;
	
	u32 block_width;
	
	u32 block_height;
	
	u32 aligned_width;
	
	u32 aligned_height;
	
	u32 offset;
	
	u32 afbc_size;
};

#define fb_to_afbc_fb(x) container_of(x, struct drm_afbc_framebuffer, base)

#endif
