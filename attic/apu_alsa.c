#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/initval.h>
#include "apu.h" // Include the APU functions from apu.c

#define APU_BUFFER_SIZE 4096

struct apu_pcm {
    struct snd_pcm_substream *substream;
    char *cpu_buffer;
    dma_addr_t dma_addr;
};

static int apu_pcm_open(struct snd_pcm_substream *substream) {
    struct snd_pcm_runtime *runtime = substream->runtime;
    struct apu_pcm *apu_data;

    apu_data = kzalloc(sizeof(*apu_data), GFP_KERNEL);
    if (!apu_data)
        return -ENOMEM;

    apu_data->cpu_buffer = kzalloc(APU_BUFFER_SIZE, GFP_KERNEL);
    if (!apu_data->cpu_buffer) {
        kfree(apu_data);
        return -ENOMEM;
    }

    runtime->private_data = apu_data;
    runtime->hw = (struct snd_pcm_hardware) {
        .info = SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER,
        .formats = SNDRV_PCM_FMTBIT_S16_LE,
        .rates = SNDRV_PCM_RATE_48000,
        .rate_min = 48000,
        .rate_max = 48000,
        .channels_min = 2,
        .channels_max = 2,
        .buffer_bytes_max = APU_BUFFER_SIZE,
        .period_bytes_min = 1024,
        .period_bytes_max = APU_BUFFER_SIZE / 2,
        .periods_min = 2,
        .periods_max = 4,
    };

    return 0;
}

static int apu_pcm_close(struct snd_pcm_substream *substream) {
    struct apu_pcm *apu_data = substream->runtime->private_data;

    kfree(apu_data->cpu_buffer);
    kfree(apu_data);

    return 0;
}

static int apu_pcm_hw_params(struct snd_pcm_substream *substream,
                             struct snd_pcm_hw_params *hw_params) {
    struct apu_pcm *apu_data = substream->runtime->private_data;

    apu_data->dma_addr = virt_to_phys(apu_data->cpu_buffer);
    return snd_pcm_lib_malloc_pages(substream, params_buffer_bytes(hw_params));
}

static int apu_pcm_hw_free(struct snd_pcm_substream *substream) {
    return snd_pcm_lib_free_pages(substream);
}

static int apu_pcm_prepare(struct snd_pcm_substream *substream) {
    // Prepare the APU for playback
    APUInit();
    return 0;
}

static int apu_pcm_trigger(struct snd_pcm_substream *substream, int cmd) {
    switch (cmd) {
    case SNDRV_PCM_TRIGGER_START:
        // Start playback
        return 0;
    case SNDRV_PCM_TRIGGER_STOP:
        // Stop playback
        return 0;
    default:
        return -EINVAL;
    }
}

static snd_pcm_uframes_t apu_pcm_pointer(struct snd_pcm_substream *substream) {
    // Return the current playback position
    return 0;
}

static struct snd_pcm_ops apu_pcm_ops = {
    .open = apu_pcm_open,
    .close = apu_pcm_close,
    .ioctl = snd_pcm_lib_ioctl,
    .hw_params = apu_pcm_hw_params,
    .hw_free = apu_pcm_hw_free,
    .prepare = apu_pcm_prepare,
    .trigger = apu_pcm_trigger,
    .pointer = apu_pcm_pointer,
};

static int apu_pcm_new(struct snd_card *card, struct snd_pcm *pcm) {
    snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &apu_pcm_ops);
    return 0;
}

static int apu_probe(struct platform_device *pdev) {
    struct snd_card *card;
    struct snd_pcm *pcm;
    int err;

    err = snd_card_new(&pdev->dev, -1, NULL, THIS_MODULE, 0, &card);
    if (err < 0)
        return err;

    err = snd_pcm_new(card, "APU PCM", 0, 1, 0, &pcm);
    if (err < 0)
        goto free_card;

    apu_pcm_new(card, pcm);

    strcpy(card->driver, "APU");
    strcpy(card->shortname, "APU PCM");
    strcpy(card->longname, "APU PCM Audio");

    err = snd_card_register(card);
    if (err < 0)
        goto free_card;

    platform_set_drvdata(pdev, card);
    return 0;

free_card:
    snd_card_free(card);
    return err;
}

static int apu_remove(struct platform_device *pdev) {
    struct snd_card *card = platform_get_drvdata(pdev);
    snd_card_free(card);
    return 0;
}

static struct platform_driver apu_driver = {
    .driver = {
        .name = "apu",
    },
    .probe = apu_probe,
    .remove = apu_remove,
};

module_platform_driver(apu_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Engin Cilasun");
MODULE_DESCRIPTION("ALSA driver for sandpiper Audio Processing Unit");
