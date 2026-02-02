#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dlfcn.h>
#include <string.h>

/* ALSA Definitions for runtime loading */
typedef struct _snd_pcm snd_pcm_t;
typedef unsigned long snd_pcm_uframes_t;
typedef long snd_pcm_sframes_t;

#define SND_PCM_STREAM_PLAYBACK 0
#define SND_PCM_FORMAT_S16_LE 2
#define SND_PCM_ACCESS_RW_INTERLEAVED 3

/* Function pointers */
typedef int (*snd_pcm_open_func)(snd_pcm_t **pcm, const char *name, int stream, int mode);
typedef int (*snd_pcm_set_params_func)(snd_pcm_t *pcm, int format, int access, unsigned int channels, unsigned int rate, int soft_resample, unsigned int latency);
typedef snd_pcm_sframes_t (*snd_pcm_writei_func)(snd_pcm_t *pcm, const void *buffer, snd_pcm_uframes_t size);
typedef int (*snd_pcm_recover_func)(snd_pcm_t *pcm, int err, int silent);
typedef int (*snd_pcm_drain_func)(snd_pcm_t *pcm);
typedef int (*snd_pcm_close_func)(snd_pcm_t *pcm);
typedef const char *(*snd_strerror_func)(int errnum);

#define PCM_DEVICE "default"
#define RATE 44100
#define CHANNELS 2
#define DURATION_SEC 2
#define FORMAT SND_PCM_FORMAT_S16_LE

int main(int argc, char **argv) {
    void *alsa_handle;
    snd_pcm_open_func snd_pcm_open_dyn;
    snd_pcm_set_params_func snd_pcm_set_params_dyn;
    snd_pcm_writei_func snd_pcm_writei_dyn;
    snd_pcm_recover_func snd_pcm_recover_dyn;
    snd_pcm_drain_func snd_pcm_drain_dyn;
    snd_pcm_close_func snd_pcm_close_dyn;
    snd_strerror_func snd_strerror_dyn;

    int err;
    snd_pcm_t *pcm_handle; // Renamed to avoid confusion with dlopen handle
    short *buffer;
    snd_pcm_uframes_t frames;
    
    printf("ALSA Simple Beep Sample (Dynamic Loading)\n");

    // Load libasound.so.2
    alsa_handle = dlopen("libasound.so.2", RTLD_NOW);
    if (!alsa_handle) {
        // Try without version
        alsa_handle = dlopen("libasound.so", RTLD_NOW);
        if (!alsa_handle) {
            fprintf(stderr, "Error loading libasound.so: %s\n", dlerror());
            return 1;
        }
    }

    // Load symbols
    snd_pcm_open_dyn = (snd_pcm_open_func)dlsym(alsa_handle, "snd_pcm_open");
    snd_pcm_set_params_dyn = (snd_pcm_set_params_func)dlsym(alsa_handle, "snd_pcm_set_params");
    snd_pcm_writei_dyn = (snd_pcm_writei_func)dlsym(alsa_handle, "snd_pcm_writei");
    snd_pcm_recover_dyn = (snd_pcm_recover_func)dlsym(alsa_handle, "snd_pcm_recover");
    snd_pcm_drain_dyn = (snd_pcm_drain_func)dlsym(alsa_handle, "snd_pcm_drain");
    snd_pcm_close_dyn = (snd_pcm_close_func)dlsym(alsa_handle, "snd_pcm_close");
    snd_strerror_dyn = (snd_strerror_func)dlsym(alsa_handle, "snd_strerror");

    if (!snd_pcm_open_dyn || !snd_pcm_set_params_dyn || !snd_pcm_writei_dyn || 
        !snd_pcm_recover_dyn || !snd_pcm_drain_dyn || !snd_pcm_close_dyn || !snd_strerror_dyn) {
        fprintf(stderr, "Error loading ALSA symbols\n");
        return 1;
    }

    // Open PCM device for playback.
    if ((err = snd_pcm_open_dyn(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "Cannot open PCM device %s: %s\n", PCM_DEVICE, snd_strerror_dyn(err));
        return 1;
    }

    // Set parameters
    if ((err = snd_pcm_set_params_dyn(pcm_handle,
                                  FORMAT,
                                  SND_PCM_ACCESS_RW_INTERLEAVED,
                                  CHANNELS,
                                  RATE,
                                  1,          // Resample
                                  500000)) < 0) { // Latency 0.5s
        fprintf(stderr, "Playback open error: %s\n", snd_strerror_dyn(err));
        return 1;
    }

    // Generate sine wave
    frames = RATE * DURATION_SEC;
    size_t buffer_size_bytes = frames * CHANNELS * sizeof(short);
    buffer = (short *)malloc(buffer_size_bytes);
    if (!buffer) {
        fprintf(stderr, "Not enough memory\n");
        return 1;
    }

    // A simple 440Hz sine wave
    double freq = 440.0; 
    for (int i = 0; i < frames; i++) {
        short sample = (short)(sin(2.0 * M_PI * freq * i / RATE) * 30000);
        buffer[2*i] = sample;     // Left
        buffer[2*i+1] = sample;   // Right
    }

    // Playback
    printf("Playing %dHz tone for %d seconds...\n", (int)freq, DURATION_SEC);
    snd_pcm_sframes_t written_frames = snd_pcm_writei_dyn(pcm_handle, buffer, frames);
    if (written_frames < 0)
        written_frames = snd_pcm_recover_dyn(pcm_handle, written_frames, 0);
    if (written_frames < 0) {
        fprintf(stderr, "snd_pcm_writei failed: %s\n", snd_strerror_dyn((int)written_frames));
    }

    // Cleanup
    snd_pcm_drain_dyn(pcm_handle);
    snd_pcm_close_dyn(pcm_handle);
    free(buffer);
    dlclose(alsa_handle);

    printf("Done.\n");
    return 0;
}
