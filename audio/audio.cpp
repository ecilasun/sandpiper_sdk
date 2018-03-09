/** \file
 * Audio generation example.
 * \ingroup examples
 * This example demonstrates how to generate a simple audio tone and play it back using the APU.
 * The example generates a simple stereo tone, please connect headphones or speakers to the audio output of the board.
 */

#include <stdio.h>
#include <math.h>
#include <cstdlib>

#include "core.h"
#include "audio.h"

struct SPPlatform platform;
static EAudioContext ax;
static SPSizeAlloc apubuffer;

#define BUFFER_SAMPLES 512		// buffer size
#define NUM_CHANNELS 2			// Stereo
#define SAMPLE_WIDTH 2			// 16 bit samples

void shutdowncleanup()
{
	APUSetSampleRate(&ax, ASR_Halt);
	APUShutdownAudio(&ax);
	SPFreeBuffer(&platform, &apubuffer);
	SPShutdownPlatform(&platform);
}

void sigint_handler(int s)
{
	shutdowncleanup();
	exit(0);
}

// Approximation of an old school phone ring tone
int main(int argc, char**argv)
{
	SPInitPlatform(&platform);
	APUInitAudio(&ax, &platform);

	apubuffer.size = BUFFER_SAMPLES*NUM_CHANNELS*SAMPLE_WIDTH;
	SPAllocateBuffer(&platform, &apubuffer);
	printf("APU mix buffer at 0x%.4x\n", (unsigned int)apubuffer.cpuAddress);

	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);

	APUSetBufferSize(&ax, ABS_2048Bytes);
	APUSetSampleRate(&ax, ASR_44_100_Hz);
	uint32_t prevframe = APUFrame(&ax);

	float offset = 0.f;
	short *buf = (short*)apubuffer.cpuAddress;
	do{
		// Generate individual waves for each channel
		for (uint32_t i=0; i<BUFFER_SAMPLES; ++i)
		{
			buf[i*NUM_CHANNELS+0] = short(16384.f*sinf(offset+2.f*3.1415927f*float(i)/12.f));
			buf[i*NUM_CHANNELS+1] = short(16384.f*cosf(offset+2.f*3.1415927f*float(i*2)/38.f));
		}

		// Fill current write buffer with new mix data
		APUStartDMA(&ax, (uint32_t)apubuffer.dmaAddress);

		// Wait for the APU to finish playing back current read buffer
		uint32_t currframe;
		do
		{
			currframe = APUFrame(&ax);
		} while (currframe == prevframe);

		// Once we reach this point, the APU has switched to the other buffer we just filled, and playback resumes uninterrupted

		// Remember this frame for next time
		prevframe = currframe;

		offset += 0.1f;

	} while(1);

	return 0;
}
