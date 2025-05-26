/* SPDX-License-Identifier: GPL-2.0
 *
 * linux/sound/soc-dpcm.h -- ALSA SoC Dynamic PCM Support
 *
 * Author:		Liam Girdwood <lrg@ti.com>
 */

#ifndef __LINUX_SND_SOC_DPCM_H
#define __LINUX_SND_SOC_DPCM_H

#include <linux/slab.h>
#include <linux/list.h>
#include <sound/pcm.h>

struct snd_soc_pcm_runtime;


enum snd_soc_dpcm_update {
	SND_SOC_DPCM_UPDATE_NO	= 0,
	SND_SOC_DPCM_UPDATE_BE,
	SND_SOC_DPCM_UPDATE_FE,
};


enum snd_soc_dpcm_link_state {
	SND_SOC_DPCM_LINK_STATE_NEW	= 0,	
	SND_SOC_DPCM_LINK_STATE_FREE,		
};


enum snd_soc_dpcm_state {
	SND_SOC_DPCM_STATE_NEW	= 0,
	SND_SOC_DPCM_STATE_OPEN,
	SND_SOC_DPCM_STATE_HW_PARAMS,
	SND_SOC_DPCM_STATE_PREPARE,
	SND_SOC_DPCM_STATE_START,
	SND_SOC_DPCM_STATE_STOP,
	SND_SOC_DPCM_STATE_PAUSED,
	SND_SOC_DPCM_STATE_SUSPEND,
	SND_SOC_DPCM_STATE_HW_FREE,
	SND_SOC_DPCM_STATE_CLOSE,
};


enum snd_soc_dpcm_trigger {
	SND_SOC_DPCM_TRIGGER_PRE		= 0,
	SND_SOC_DPCM_TRIGGER_POST,
	SND_SOC_DPCM_TRIGGER_BESPOKE,
};


struct snd_soc_dpcm {
	
	struct snd_soc_pcm_runtime *be;
	struct snd_soc_pcm_runtime *fe;

	
	enum snd_soc_dpcm_link_state state;

	
	struct list_head list_be;
	struct list_head list_fe;

#ifdef CONFIG_DEBUG_FS
	struct dentry *debugfs_state;
#endif
};


struct snd_soc_dpcm_runtime {
	struct list_head be_clients;
	struct list_head fe_clients;

	int users;
	struct snd_pcm_hw_params hw_params;

	
	enum snd_soc_dpcm_update runtime_update;
	enum snd_soc_dpcm_state state;

	int trigger_pending; 

	int be_start; 
	int be_pause; 
	bool fe_pause; 
};

#define for_each_dpcm_fe(be, stream, _dpcm)				\
	list_for_each_entry(_dpcm, &(be)->dpcm[stream].fe_clients, list_fe)

#define for_each_dpcm_be(fe, stream, _dpcm)				\
	list_for_each_entry(_dpcm, &(fe)->dpcm[stream].be_clients, list_be)
#define for_each_dpcm_be_safe(fe, stream, _dpcm, __dpcm)			\
	list_for_each_entry_safe(_dpcm, __dpcm, &(fe)->dpcm[stream].be_clients, list_be)
#define for_each_dpcm_be_rollback(fe, stream, _dpcm)			\
	list_for_each_entry_continue_reverse(_dpcm, &(fe)->dpcm[stream].be_clients, list_be)


int snd_soc_dpcm_can_be_free_stop(struct snd_soc_pcm_runtime *fe,
		struct snd_soc_pcm_runtime *be, int stream);


int snd_soc_dpcm_can_be_params(struct snd_soc_pcm_runtime *fe,
		struct snd_soc_pcm_runtime *be, int stream);


int snd_soc_dpcm_can_be_prepared(struct snd_soc_pcm_runtime *fe,
				 struct snd_soc_pcm_runtime *be, int stream);


int snd_soc_dpcm_fe_can_update(struct snd_soc_pcm_runtime *fe, int stream);


int snd_soc_dpcm_be_can_update(struct snd_soc_pcm_runtime *fe,
		struct snd_soc_pcm_runtime *be, int stream);


struct snd_pcm_substream *
	snd_soc_dpcm_get_substream(struct snd_soc_pcm_runtime *be, int stream);


int snd_soc_dpcm_runtime_update(struct snd_soc_card *card);

#ifdef CONFIG_DEBUG_FS
void soc_dpcm_debugfs_add(struct snd_soc_pcm_runtime *rtd);
#else
static inline void soc_dpcm_debugfs_add(struct snd_soc_pcm_runtime *rtd)
{
}
#endif

int dpcm_path_get(struct snd_soc_pcm_runtime *fe,
	int stream, struct snd_soc_dapm_widget_list **list_);
void dpcm_path_put(struct snd_soc_dapm_widget_list **list);
int dpcm_process_paths(struct snd_soc_pcm_runtime *fe,
	int stream, struct snd_soc_dapm_widget_list **list, int new);
int dpcm_be_dai_startup(struct snd_soc_pcm_runtime *fe, int stream);
void dpcm_be_dai_stop(struct snd_soc_pcm_runtime *fe, int stream,
		      int do_hw_free, struct snd_soc_dpcm *last);
void dpcm_be_disconnect(struct snd_soc_pcm_runtime *fe, int stream);
void dpcm_clear_pending_state(struct snd_soc_pcm_runtime *fe, int stream);
void dpcm_be_dai_hw_free(struct snd_soc_pcm_runtime *fe, int stream);
int dpcm_be_dai_hw_params(struct snd_soc_pcm_runtime *fe, int tream);
int dpcm_be_dai_trigger(struct snd_soc_pcm_runtime *fe, int stream, int cmd);
int dpcm_be_dai_prepare(struct snd_soc_pcm_runtime *fe, int stream);
int dpcm_dapm_stream_event(struct snd_soc_pcm_runtime *fe, int dir,
	int event);
bool dpcm_end_walk_at_be(struct snd_soc_dapm_widget *widget, enum snd_soc_dapm_direction dir);
int widget_in_list(struct snd_soc_dapm_widget_list *list,
		   struct snd_soc_dapm_widget *widget);

#define dpcm_be_dai_startup_rollback(fe, stream, last)	\
						dpcm_be_dai_stop(fe, stream, 0, last)
#define dpcm_be_dai_startup_unwind(fe, stream)	dpcm_be_dai_stop(fe, stream, 0, NULL)
#define dpcm_be_dai_shutdown(fe, stream)	dpcm_be_dai_stop(fe, stream, 1, NULL)

#endif
