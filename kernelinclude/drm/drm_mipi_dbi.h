/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __LINUX_MIPI_DBI_H
#define __LINUX_MIPI_DBI_H

#include <linux/mutex.h>
#include <drm/drm_device.h>
#include <drm/drm_simple_kms_helper.h>

struct drm_rect;
struct gpio_desc;
struct iosys_map;
struct regulator;
struct spi_device;


struct mipi_dbi {
	
	struct mutex cmdlock;

	
	int (*command)(struct mipi_dbi *dbi, u8 *cmd, u8 *param, size_t num);

	
	const u8 *read_commands;

	
	bool swap_bytes;

	
	struct gpio_desc *reset;

	

	
	struct spi_device *spi;

	
	struct gpio_desc *dc;

	
	void *tx_buf9;

	
	size_t tx_buf9_len;
};


struct mipi_dbi_dev {
	
	struct drm_device drm;

	
	struct drm_simple_display_pipe pipe;

	
	struct drm_connector connector;

	
	struct drm_display_mode mode;

	
	u16 *tx_buf;

	
	unsigned int rotation;

	
	unsigned int left_offset;

	
	unsigned int top_offset;

	
	struct backlight_device *backlight;

	
	struct regulator *regulator;

	
	struct regulator *io_regulator;

	
	struct mipi_dbi dbi;

	
	void *driver_private;
};

static inline struct mipi_dbi_dev *drm_to_mipi_dbi_dev(struct drm_device *drm)
{
	return container_of(drm, struct mipi_dbi_dev, drm);
}

int mipi_dbi_spi_init(struct spi_device *spi, struct mipi_dbi *dbi,
		      struct gpio_desc *dc);
int mipi_dbi_dev_init_with_formats(struct mipi_dbi_dev *dbidev,
				   const struct drm_simple_display_pipe_funcs *funcs,
				   const uint32_t *formats, unsigned int format_count,
				   const struct drm_display_mode *mode,
				   unsigned int rotation, size_t tx_buf_size);
int mipi_dbi_dev_init(struct mipi_dbi_dev *dbidev,
		      const struct drm_simple_display_pipe_funcs *funcs,
		      const struct drm_display_mode *mode, unsigned int rotation);
enum drm_mode_status mipi_dbi_pipe_mode_valid(struct drm_simple_display_pipe *pipe,
					      const struct drm_display_mode *mode);
void mipi_dbi_pipe_update(struct drm_simple_display_pipe *pipe,
			  struct drm_plane_state *old_state);
void mipi_dbi_enable_flush(struct mipi_dbi_dev *dbidev,
			   struct drm_crtc_state *crtc_state,
			   struct drm_plane_state *plan_state);
void mipi_dbi_pipe_disable(struct drm_simple_display_pipe *pipe);
int mipi_dbi_pipe_begin_fb_access(struct drm_simple_display_pipe *pipe,
				  struct drm_plane_state *plane_state);
void mipi_dbi_pipe_end_fb_access(struct drm_simple_display_pipe *pipe,
				 struct drm_plane_state *plane_state);
void mipi_dbi_pipe_reset_plane(struct drm_simple_display_pipe *pipe);
struct drm_plane_state *mipi_dbi_pipe_duplicate_plane_state(struct drm_simple_display_pipe *pipe);
void mipi_dbi_pipe_destroy_plane_state(struct drm_simple_display_pipe *pipe,
				       struct drm_plane_state *plane_state);

void mipi_dbi_hw_reset(struct mipi_dbi *dbi);
bool mipi_dbi_display_is_on(struct mipi_dbi *dbi);
int mipi_dbi_poweron_reset(struct mipi_dbi_dev *dbidev);
int mipi_dbi_poweron_conditional_reset(struct mipi_dbi_dev *dbidev);

u32 mipi_dbi_spi_cmd_max_speed(struct spi_device *spi, size_t len);
int mipi_dbi_spi_transfer(struct spi_device *spi, u32 speed_hz,
			  u8 bpw, const void *buf, size_t len);

int mipi_dbi_command_read(struct mipi_dbi *dbi, u8 cmd, u8 *val);
int mipi_dbi_command_buf(struct mipi_dbi *dbi, u8 cmd, u8 *data, size_t len);
int mipi_dbi_command_stackbuf(struct mipi_dbi *dbi, u8 cmd, const u8 *data,
			      size_t len);
int mipi_dbi_buf_copy(void *dst, struct iosys_map *src, struct drm_framebuffer *fb,
		      struct drm_rect *clip, bool swap);


#define mipi_dbi_command(dbi, cmd, seq...) \
({ \
	const u8 d[] = { seq }; \
	struct device *dev = &(dbi)->spi->dev;	\
	int ret; \
	ret = mipi_dbi_command_stackbuf(dbi, cmd, d, ARRAY_SIZE(d)); \
	if (ret) \
		dev_err_ratelimited(dev, "error %d when sending command %#02x\n", ret, cmd); \
	ret; \
})

#ifdef CONFIG_DEBUG_FS
void mipi_dbi_debugfs_init(struct drm_minor *minor);
#else
static inline void mipi_dbi_debugfs_init(struct drm_minor *minor) {}
#endif


#define DRM_MIPI_DBI_SIMPLE_DISPLAY_PIPE_FUNCS(enable_) \
	.mode_valid = mipi_dbi_pipe_mode_valid, \
	.enable = (enable_), \
	.disable = mipi_dbi_pipe_disable, \
	.update = mipi_dbi_pipe_update, \
	.begin_fb_access = mipi_dbi_pipe_begin_fb_access, \
	.end_fb_access = mipi_dbi_pipe_end_fb_access, \
	.reset_plane = mipi_dbi_pipe_reset_plane, \
	.duplicate_plane_state = mipi_dbi_pipe_duplicate_plane_state, \
	.destroy_plane_state = mipi_dbi_pipe_destroy_plane_state

#endif 
