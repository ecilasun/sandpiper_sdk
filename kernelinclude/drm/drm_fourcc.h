
#ifndef __DRM_FOURCC_H__
#define __DRM_FOURCC_H__

#include <linux/types.h>
#include <uapi/drm/drm_fourcc.h>


#define DRM_FORMAT_MAX_PLANES	4u


#ifdef __BIG_ENDIAN
# define DRM_FORMAT_HOST_XRGB1555     (DRM_FORMAT_XRGB1555         |	\
				       DRM_FORMAT_BIG_ENDIAN)
# define DRM_FORMAT_HOST_RGB565       (DRM_FORMAT_RGB565           |	\
				       DRM_FORMAT_BIG_ENDIAN)
# define DRM_FORMAT_HOST_XRGB8888     DRM_FORMAT_BGRX8888
# define DRM_FORMAT_HOST_ARGB8888     DRM_FORMAT_BGRA8888
#else
# define DRM_FORMAT_HOST_XRGB1555     DRM_FORMAT_XRGB1555
# define DRM_FORMAT_HOST_RGB565       DRM_FORMAT_RGB565
# define DRM_FORMAT_HOST_XRGB8888     DRM_FORMAT_XRGB8888
# define DRM_FORMAT_HOST_ARGB8888     DRM_FORMAT_ARGB8888
#endif

struct drm_device;
struct drm_mode_fb_cmd2;


struct drm_format_info {
	
	u32 format;

	
	u8 depth;

	
	u8 num_planes;

	union {
		
		u8 cpp[DRM_FORMAT_MAX_PLANES];

		
		u8 char_per_block[DRM_FORMAT_MAX_PLANES];
	};

	
	u8 block_w[DRM_FORMAT_MAX_PLANES];

	
	u8 block_h[DRM_FORMAT_MAX_PLANES];

	
	u8 pixels_per_macropixel[3];

	
	u8 bytes_per_macropixel[3];

	
	u8 hsub;
	
	u8 vsub;

	
	bool has_alpha;

	
	bool is_yuv;

	
	bool is_color_indexed;
};


static inline bool
drm_format_info_is_yuv_packed(const struct drm_format_info *info)
{
	return info->is_yuv && info->num_planes == 1;
}


static inline bool
drm_format_info_is_yuv_semiplanar(const struct drm_format_info *info)
{
	return info->is_yuv && info->num_planes == 2;
}


static inline bool
drm_format_info_is_yuv_planar(const struct drm_format_info *info)
{
	return info->is_yuv && info->num_planes == 3;
}


static inline bool
drm_format_info_is_yuv_sampling_410(const struct drm_format_info *info)
{
	return info->is_yuv && info->hsub == 4 && info->vsub == 4;
}


static inline bool
drm_format_info_is_yuv_sampling_411(const struct drm_format_info *info)
{
	return info->is_yuv && info->hsub == 4 && info->vsub == 1;
}


static inline bool
drm_format_info_is_yuv_sampling_420(const struct drm_format_info *info)
{
	return info->is_yuv && info->hsub == 2 && info->vsub == 2;
}


static inline bool
drm_format_info_is_yuv_sampling_422(const struct drm_format_info *info)
{
	return info->is_yuv && info->hsub == 2 && info->vsub == 1;
}


static inline bool
drm_format_info_is_yuv_sampling_444(const struct drm_format_info *info)
{
	return info->is_yuv && info->hsub == 1 && info->vsub == 1;
}


static inline
int drm_format_info_plane_width(const struct drm_format_info *info, int width,
				int plane)
{
	if (!info || plane >= info->num_planes)
		return 0;

	if (plane == 0)
		return width;

	return width / info->hsub;
}


static inline
int drm_format_info_plane_height(const struct drm_format_info *info, int height,
				 int plane)
{
	if (!info || plane >= info->num_planes)
		return 0;

	if (plane == 0)
		return height;

	return height / info->vsub;
}

const struct drm_format_info *__drm_format_info(u32 format);
const struct drm_format_info *drm_format_info(u32 format);
const struct drm_format_info *
drm_get_format_info(struct drm_device *dev,
		    const struct drm_mode_fb_cmd2 *mode_cmd);
uint32_t drm_mode_legacy_fb_format(uint32_t bpp, uint32_t depth);
uint32_t drm_driver_legacy_fb_format(struct drm_device *dev,
				     uint32_t bpp, uint32_t depth);
unsigned int drm_format_info_block_width(const struct drm_format_info *info,
					 int plane);
unsigned int drm_format_info_block_height(const struct drm_format_info *info,
					  int plane);
unsigned int drm_format_info_bpp(const struct drm_format_info *info, int plane);
uint64_t drm_format_info_min_pitch(const struct drm_format_info *info,
				   int plane, unsigned int buffer_width);
uint64_t drm_format_plane_width_bytes(const struct drm_format_info *info,
				 int plane, unsigned int width);

#endif 
