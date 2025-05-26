/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __HDMI_CODEC_H__
#define __HDMI_CODEC_H__

#include <linux/of_graph.h>
#include <linux/hdmi.h>
#include <drm/drm_edid.h>
#include <sound/asoundef.h>
#include <sound/soc.h>
#include <uapi/sound/asound.h>


struct hdmi_codec_daifmt {
	enum {
		HDMI_I2S,
		HDMI_RIGHT_J,
		HDMI_LEFT_J,
		HDMI_DSP_A,
		HDMI_DSP_B,
		HDMI_AC97,
		HDMI_SPDIF,
	} fmt;
	unsigned int bit_clk_inv:1;
	unsigned int frame_clk_inv:1;
	unsigned int bit_clk_provider:1;
	unsigned int frame_clk_provider:1;
	
	snd_pcm_format_t bit_fmt;
};


struct hdmi_codec_params {
	struct hdmi_audio_infoframe cea;
	struct snd_aes_iec958 iec;
	int sample_rate;
	int sample_width;
	int channels;
};

typedef void (*hdmi_codec_plugged_cb)(struct device *dev,
				      bool plugged);

struct hdmi_codec_pdata;
struct hdmi_codec_ops {
	
	int (*audio_startup)(struct device *dev, void *data);

	
	int (*hw_params)(struct device *dev, void *data,
			 struct hdmi_codec_daifmt *fmt,
			 struct hdmi_codec_params *hparms);

	
	int (*prepare)(struct device *dev, void *data,
		       struct hdmi_codec_daifmt *fmt,
		       struct hdmi_codec_params *hparms);

	
	void (*audio_shutdown)(struct device *dev, void *data);

	
	int (*mute_stream)(struct device *dev, void *data,
			   bool enable, int direction);

	
	int (*get_eld)(struct device *dev, void *data,
		       uint8_t *buf, size_t len);

	
	int (*get_dai_id)(struct snd_soc_component *comment,
			  struct device_node *endpoint);

	
	int (*hook_plugged_cb)(struct device *dev, void *data,
			       hdmi_codec_plugged_cb fn,
			       struct device *codec_dev);

	
	unsigned int no_capture_mute:1;
};


struct hdmi_codec_pdata {
	const struct hdmi_codec_ops *ops;
	uint i2s:1;
	uint no_i2s_playback:1;
	uint no_i2s_capture:1;
	uint spdif:1;
	uint no_spdif_playback:1;
	uint no_spdif_capture:1;
	int max_i2s_channels;
	void *data;
};

struct snd_soc_component;
struct snd_soc_jack;

#define HDMI_CODEC_DRV_NAME "hdmi-audio-codec"

#endif 
