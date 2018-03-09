#include <complex>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#include "core.h"
#include "platform.h"
#include "audio.h"
#include "video.h"

#include "xmp.h"

struct SPPlatform platform;

static xmp_context ctx;
static EVideoContext vx;
static EAudioContext ax;
static EVideoSwapContext sc;
static SPSizeAlloc apubuffer;
struct SPSizeAlloc bufferA;
struct SPSizeAlloc bufferB;

// Number of 16bit stereo samples (valid values are 1024, 512, 256, 128 or 64)
#define BUFFER_SAMPLE_COUNT 1024
#define BUFFER_CHANNEL_COUNT 2
#define BUFFER_SAMPLE_SIZE sizeof(short)
#define BUFFER_BYTE_COUNT (BUFFER_SAMPLE_COUNT*BUFFER_SAMPLE_SIZE*BUFFER_CHANNEL_COUNT)

std::complex<float> outputL[BUFFER_SAMPLE_COUNT];
std::complex<float> outputR[BUFFER_SAMPLE_COUNT];
int16_t barsL[256];
int16_t barsR[256];

void shutdowncleanup()
{
	// Stop audio
	APUSetSampleRate(&ax, ASR_Halt);

	// Turn off video scan-out
	VPUSetVideoMode(&vx, EVM_320_Wide, ECM_8bit_Indexed, EVS_Disable);

	// Yield physical memory and reset video routines
	VPUShutdownVideo();
	APUShutdownAudio(&ax);

	// Release allocations
	SPFreeBuffer(&platform, &apubuffer);
	SPFreeBuffer(&platform, &bufferB);
	SPFreeBuffer(&platform, &bufferA);

	// Shutdown platform
	SPShutdownPlatform(&platform);
}

void sigint_handler(int s)
{
	shutdowncleanup();
	exit(0);
}

void fft(std::complex<float>* data)
{
    const size_t N = BUFFER_SAMPLE_COUNT;
    const float PI = 3.14159265358979323846f;

    // Bit-reversal permutation
    size_t n = N;
    size_t j = 0;
    for (size_t i = 0; i < n; ++i) {
        if (i < j) {
            std::swap(data[i], data[j]);
        }
        size_t m = n >> 1;
        while (j >= m && m >= 2) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }

    // Cooley-Tukey FFT
    for (size_t len = 2; len <= n; len <<= 1) {
        float angle = -2.0f * PI / len;
        std::complex<float> wlen(cos(angle), sin(angle));
        for (size_t i = 0; i < n; i += len) {
            std::complex<float> w(1);
            for (size_t j = 0; j < len / 2; ++j) {
                std::complex<float> u = data[i + j];
                std::complex<float> v = data[i + j + len / 2] * w;
                data[i + j] = u + v;
                data[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }
}

void *draw_wave(void *data)
{
	uint32_t stride = VPUGetStride(EVM_320_Wide, ECM_8bit_Indexed);

	while(1)
	{
		VPUClear(&vx, 0x00000000);

		short* buf = (short*)apubuffer.cpuAddress;
		for (size_t i = 0; i < BUFFER_SAMPLE_COUNT; ++i)
		{
			outputL[i] = std::complex<float>(buf[i*2+0]>>15, 0.0f);
			outputR[i] = std::complex<float>(buf[i*2+1]>>15, 0.0f);
		}

		fft(outputL);
		fft(outputR);		

		for (uint32_t i=0; i<256; i+=4)
		{
			int16_t L0 = 200 - (int16_t)std::abs(outputL[i+0]);
			int16_t L1 = 200 - (int16_t)std::abs(outputL[i+1]);
			int16_t L2 = 200 - (int16_t)std::abs(outputL[i+2]);
			int16_t L3 = 200 - (int16_t)std::abs(outputL[i+3]);
			int16_t R0 = 200 - (int16_t)std::abs(outputR[i+0]);
			int16_t R1 = 200 - (int16_t)std::abs(outputR[i+1]);
			int16_t R2 = 200 - (int16_t)std::abs(outputR[i+2]);
			int16_t R3 = 200 - (int16_t)std::abs(outputR[i+3]);
			barsL[i>>2] = (barsL[i>>2] + L0 + L1 + L2 + L3)/5;
			barsR[i>>2] = (barsR[i>>2] + R0 + R1 + R2 + R3)/5;
		}

		// Draw first 128 samples
		for (uint32_t i=0; i<128; ++i)
		{
			// Convert i to a logarithmic coordinate
			int16_t logi = (int16_t)(128.0f * log10f((float)i+1.0f) / 2.0f);
			// Next bar's logarithmic coordinate
			int16_t nextlogi = (int16_t)(128.0f * log10f((float)(i+1)+1.0f) / 2.0f);
			// Distance between the two
			int16_t delta = nextlogi - logi;
	
			// Draw bars for left channel
			for (int16_t j=0; j<delta; ++j)
			{
				if (logi+j >= 0 && logi+j < 320)
				{
					int16_t L = std::min<int16_t>(239, std::max<int16_t>(0, barsL[i]));
					for (int16_t k=L; k<200; ++k)
						sc.writepage[16 + logi+j + k*stride] = 0x37;
				}
			}

			// Do the same for right channel
			for (int16_t j=0; j<delta; ++j)
			{
				if (logi+j >= 0 && logi+j < 320)
				{
					int16_t R = std::min<int16_t>(239, std::max<int16_t>(0, barsR[i]));
					for (int16_t k=R; k<200; ++k)
						sc.writepage[304 - logi-j + k*stride] = 0x27;
				}
			}
		}

		VPUWaitVSync(&vx);
		VPUSwapPages(&vx, &sc);
		sched_yield();
	}
	return NULL;
}

void *PlayXMP(void *data)
{
	char *fname = (char*)data;

	struct xmp_module_info mi;
	struct xmp_frame_info fi;
	int i;

	ctx = xmp_create_context();

	if (xmp_load_module(ctx, fname) < 0)
	{
		printf("Error: cannot load module '%s'\n", fname);
		return NULL;
	}

	APUSetBufferSize(&ax, ABS_4096Bytes);
	APUSetSampleRate(&ax, ASR_22_050_Hz);

	if (xmp_start_player(ctx, 22050, 0) == 0)
	{
		xmp_get_module_info(ctx, &mi);
		printf("%s (%s)\n", mi.mod->name, mi.mod->type);

		int playing = 1;
		short* buf = (short*)apubuffer.cpuAddress;
		volatile uint32_t prevframe = APUFrame(&ax);
		while (playing)
		{
			playing = xmp_play_buffer(ctx, buf, BUFFER_BYTE_COUNT, 0) == 0;

			// Fill current write buffer with new mix data
			APUStartDMA(&ax, (uint32_t)apubuffer.dmaAddress);

			// Wait for the APU to be done with current read buffer which is still playing
			volatile uint32_t currframe;
			do
			{
				// APU will return a different 'frame' as soon as the current buffer reaches the end
				currframe = APUFrame(&ax);
			} while (currframe == prevframe);

			// Once we reach this point, the APU has switched to the other buffer we just filled, and playback resumes uninterrupted

			// Remember this frame
			prevframe = currframe;
		}
		xmp_end_player(ctx);

		xmp_release_module(ctx);
		xmp_free_context(ctx);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Usage: %s <modulefilename>\n", argv[0]);
		return -1;
	}

	SPInitPlatform(&platform);

	VPUInitVideo(&vx, &platform);
	APUInitAudio(&ax, &platform);

	// 4Kbytes of space for the APU
	// Total APU memory is 8Kbytes and it alternates between two halves each time we call APUStartDMA
	apubuffer.size = BUFFER_BYTE_COUNT;
	SPAllocateBuffer(&platform, &apubuffer);
	{
		short* buf = (short*)apubuffer.cpuAddress;
		memset(buf, 0, BUFFER_BYTE_COUNT);
	}
	printf("APU mix buffer: 0x%08X <-0x%08X - %dbytes \n", (unsigned int)apubuffer.cpuAddress, (unsigned int)apubuffer.dmaAddress, apubuffer.size);

	uint32_t stride = VPUGetStride(EVM_320_Wide, ECM_8bit_Indexed);
	bufferB.size = bufferA.size = stride*240;
	SPAllocateBuffer(&platform, &bufferA);
	printf("VPU buffer: 0x%08X <-0x%08X - %dbytes \n", (unsigned int)bufferA.cpuAddress, (unsigned int)bufferA.dmaAddress, bufferB.size);
	SPAllocateBuffer(&platform, &bufferB);
	printf("VPU buffer: 0x%08X <-0x%08X - %dbytes \n", (unsigned int)bufferB.cpuAddress, (unsigned int)bufferB.dmaAddress, bufferB.size);

	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);

	VPUSetVideoMode(&vx, EVM_320_Wide, ECM_8bit_Indexed, EVS_Enable);
	VPUSetDefaultPalette(&vx);

	sc.cycle = 0;
	sc.framebufferA = &bufferA;
	sc.framebufferB = &bufferB;
	VPUSwapPages(&vx, &sc);
	VPUClear(&vx, 0x00000000);
	VPUSwapPages(&vx, &sc);
	VPUClear(&vx, 0x00000000);

	memset(barsL, 0, 256*sizeof(int16_t));
	memset(barsR, 0, 256*sizeof(int16_t));

	pthread_t thread1, thread2;
	int success = pthread_create(&thread1, NULL, draw_wave, NULL);
	success = pthread_create(&thread2, NULL, PlayXMP, argv[1]);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	printf("Playback complete\n");

	return 0;
}
