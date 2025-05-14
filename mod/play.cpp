#include <complex>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "core.h"
#include "platform.h"
#include "audio.h"
#include "video.h"

#include "xmp.h"

struct SPPlatform platform;

#define BUFFER_WORD_COUNT 1024		// buffer size (max: 2048 bytes i.e. 1024 words)
#define BUFFER_SIZE_IN_BYTES (BUFFER_WORD_COUNT*2*sizeof(short))

static xmp_context ctx;
static EVideoContext vx;
static EAudioContext ax;
static EVideoSwapContext sc;
static SPSizeAlloc apubuffer;
struct SPSizeAlloc bufferA;
struct SPSizeAlloc bufferB;

std::complex<float> outputL[BUFFER_WORD_COUNT*2];
std::complex<float> outputR[BUFFER_WORD_COUNT*2];
int16_t barsL[256];
int16_t barsR[256];

void shutdowncleanup()
{
	// Stop audio
	APUSetSampleRate(&ax, ASR_Halt);

	// Turn off video scan-out
//	VPUSetVideoMode(&vx, EVM_320_Wide, ECM_8bit_Indexed, EVS_Disable);

	// Yield physical memory and reset video routines
//	VPUShutdownVideo();
	APUShutdownAudio(&ax);

	// Release allocations
	SPFreeBuffer(&platform, &apubuffer);
//	SPFreeBuffer(&platform, &bufferB);
//	SPFreeBuffer(&platform, &bufferA);

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
    const size_t N = BUFFER_WORD_COUNT;
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

void draw_wave()
{
	VPUClear(&vx, 0x00000000);

	short* buf = (short*)apubuffer.cpuAddress;
	for (size_t i = 0; i < BUFFER_WORD_COUNT; ++i)
	{
		outputL[i] = std::complex<float>(buf[i*2+0]>>15, 0.0f);
		outputR[i] = std::complex<float>(buf[i*2+1]>>15, 0.0f);
	}

	fft(outputL);
	fft(outputR);		

	for (uint32_t i=0; i<BUFFER_WORD_COUNT/2; i+=4)
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
					sc.writepage[16 + logi+j + k*320] = 0x37;
			}
		}

		// Do the same for right channel
		for (int16_t j=0; j<delta; ++j)
		{
			if (logi+j >= 0 && logi+j < 320)
			{
				int16_t R = std::min<int16_t>(239, std::max<int16_t>(0, barsR[i]));
				for (int16_t k=R; k<200; ++k)
					sc.writepage[304 - logi-j + k*320] = 0x27;
			}
		}
	}

	DCACHE_FLUSH();

	//VPUWaitVSync();
	VPUSwapPages(&vx, &sc);
}

void PlayXMP(const char *fname)
{
	struct xmp_module_info mi;
	struct xmp_frame_info fi;
	int i;

	ctx = xmp_create_context();

	if (xmp_load_module(ctx, fname) < 0)
	{
		printf("Error: cannot load module '%s'\n", fname);
		return;
	}

	APUSetBufferSize(&ax, BUFFER_WORD_COUNT); // word count = sample count/2 (i.e. number of stereo sample pairs)
	APUSetSampleRate(&ax, ASR_22_050_Hz);
	uint32_t prevframe = APUFrame(&ax);

	if (xmp_start_player(ctx, 22050, 0) == 0)
	{
		xmp_get_module_info(ctx, &mi);
		printf("%s (%s)\n", mi.mod->name, mi.mod->type);

		int playing = 1;
		short* buf = (short*)apubuffer.cpuAddress;
		while (playing) // size == 2*BUFFER_WORD_COUNT, in bytes
		{
			playing = xmp_play_buffer(ctx, buf, BUFFER_SIZE_IN_BYTES, 0) == 0;

			// Make sure the writes are visible by the DMA
			DCACHE_FLUSH();

			// Fill current write buffer with new mix data
			APUStartDMA(&ax, (uint32_t)apubuffer.dmaAddress);

			// Wait for the APU to be done with current read buffer which is still playing
			uint32_t currframe;
			do
			{
				// APU will return a different 'frame' as soon as the current buffer reaches the end
				currframe = APUFrame(&ax);
			} while (currframe == prevframe);

			// Once we reach this point, the APU has switched to the other buffer we just filled, and playback resumes uninterrupted

//			draw_wave();

			// Remember this frame
			prevframe = currframe;
		}
		xmp_end_player(ctx);

		xmp_release_module(ctx);
		xmp_free_context(ctx);
	}
}

int main(int argc, char *argv[])
{
//	if (argc < 2)
//	{
//		printf("Usage: %s <modulefilename>\n", argv[0]);
//		return -1;
//	}

	SPInitPlatform(&platform);

//	VPUInitVideo(&vx, &platform);
	APUInitAudio(&ax, &platform);

	apubuffer.size = BUFFER_SIZE_IN_BYTES;
	SPAllocateBuffer(&platform, &apubuffer);
	{
		short* buf = (short*)apubuffer.cpuAddress;
		memset(buf, 0, BUFFER_SIZE_IN_BYTES);
	}
	printf("\nAPU mix buffer: 0x%08X <-0x%08X \n", (unsigned int)apubuffer.cpuAddress, (unsigned int)apubuffer.dmaAddress);

	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);

//	uint32_t stride = VPUGetStride(EVM_320_Wide, ECM_8bit_Indexed);
//	bufferB.size = bufferA.size = stride*240;
//	SPAllocateBuffer(&platform, &bufferB);
//	SPAllocateBuffer(&platform, &bufferA);

//	VPUSetVideoMode(&vx, EVM_320_Wide, ECM_8bit_Indexed, EVS_Enable);

//	sc.cycle = 0;
//	sc.framebufferA = &bufferA;
//	sc.framebufferB = &bufferB;
	//VPUSwapPages(&vx, &sc);
	//VPUClear(&vx, 0x00000000);
	//VPUSwapPages(&vx, &sc);
	//VPUClear(&vx, 0x00000000);

	memset(barsL, 0, 256*sizeof(int16_t));
	memset(barsR, 0, 256*sizeof(int16_t));

	PlayXMP("test.mod");//argv[1]);

	printf("Playback complete\n");

	return 0;
}
