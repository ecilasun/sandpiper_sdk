/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __SOC_PXA_AUDIO_H__
#define __SOC_PXA_AUDIO_H__

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/ac97_codec.h>


typedef struct {
	int (*startup)(struct snd_pcm_substream *, void *);
	void (*shutdown)(struct snd_pcm_substream *, void *);
	void (*suspend)(void *);
	void (*resume)(void *);
	void *priv;
	int reset_gpio;
	void *codec_pdata[AC97_BUS_MAX_DEVICES];
} pxa2xx_audio_ops_t;

extern void pxa_set_ac97_info(pxa2xx_audio_ops_t *ops);
extern void pxa27x_configure_ac97reset(int reset_gpio, bool to_gpio);

#endif
