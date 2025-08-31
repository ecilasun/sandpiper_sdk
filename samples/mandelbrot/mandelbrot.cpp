/** \file
 * Multithreaded Mandelbrot set example.
 *
 * \ingroup examples
 * This example demonstrates the use of the VPU frame buffers to render a Mandelbrot set.
 * The render work is split across the two CPUs, where upon task completion, each CPU
 * pulls the next available tile to render.
 * It's a very simple task system, but should suffice to demontstrate basic thread usage.
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <cmath>
#include <signal.h>
#include <pthread.h>
#include <atomic>

#include "core.h"
#include "platform.h"
#include "vpu.h"

const float X = -0.235125f;
const float Y = 0.827215f;

struct SThreadData
{
	int tid;
	int tilex;
	int tiley;
	float R;
	std::atomic<bool> go;
	struct SPPlatform* platform = nullptr;
	struct SPSizeAlloc* framebuffer = nullptr;
};

void InitThreadData(SThreadData* data, struct SPPlatform* platform, struct SPSizeAlloc* framebuffer, int tid, float R, int tilex, int tiley)
{
	data->platform = platform;
	data->framebuffer = framebuffer;
	data->tid = tid;
	data->tilex = tilex;
	data->tiley = tiley;
	data->R = R;
	data->go.store(false);
}

float evalMandel(const int maxiter, int col, int row, float ox, float oy, float sx)
{
	float iteration = 0.f;

	float c_re = (float(col) - 160.f) / 240.f * sx + ox; // Divide by shortest side of display for correct aspect ratio
	float c_im = (float(row) - 120.f) / 240.f * sx + oy;
	float x = 0.f, y = 0.f;
	float x2 = 0.f, y2 = 0.f;
	while (x2+y2 < 4.f && iteration < maxiter)
	{
		y = c_im + 2.f*x*y;
		x = c_re + x2 - y2;
		x2 = x*x;
		y2 = y*y;
		iteration += 1.f;
	}

	return iteration;
}

void mandelbrotFloat(uint32_t tid, uint32_t stride, uint16_t* frame, float ox, float oy, float sx, int tilex, int tiley)
{
	// http://blog.recursiveprocess.com/2014/04/05/mandelbrot-fractal-v2/
	float ratio = 27.71f-5.156f*logf(sx);

	for (int y = 0; y < 16; ++y)
	{
		int row = y + tiley*16;
		for (int x = 0; x < 16; ++x)
		{
			int col = x + tilex*16;

			float M = evalMandel(ratio, col, row, ox, oy, sx);
			float local_ratio = M / ratio;
			int c = int(local_ratio*31.f);
			//c = tid == 0 ? c : c | 2; // DEBUG: watermark threads
			frame[col + (row*stride>>1)] = MAKECOLORRGB16(c, c, c);
		}
	}

	// distance	(via iq's shadertoy sample https://www.shadertoy.com/view/lsX3W4)
	// d(c) = |Z|·log|Z|/|Z'|
	// float d = 0.5*sqrt(dot(z,z)/dot(dz,dz))*log(dot(z,z));
	// if( di>0.5 ) d=0.0;
}

void* mandelbrot(void* arg)
{
	SThreadData* data = (SThreadData*)arg;

	uint32_t stride = VPUGetStride(EVM_320_Wide, ECM_16bit_RGB);

	while(1)
	{
		if (data->go.load() == true)
		{
			//printf(">%d\n", data->tid);
			int tilex = data->tilex;
			int tiley = data->tiley;
			float R = data->R;
			mandelbrotFloat(data->tid, stride, (uint16_t*)data->platform->sc->writepage, X, Y, R, tilex, tiley);
			data->go.store(false);
		}

		sched_yield();
	}

	pthread_exit(NULL);
}

void PickNextTile(int* tilex, int* tiley, float* R)
{
	(*tilex)++;
	if (*tilex == 20)
	{
		*tilex = 0;
		(*tiley)++;
	}
	if (*tiley == 15)
	{
		*tiley = 0;
		// Zoom at last tile
		*R += 0.0002f;
	}
}

int main()
{
	// Initialize platform and video system
	SPPlatform* platform = new SPPlatform();
	platform = SPInitPlatform();
	if (!platform)
	{
		printf("Failed to initialize platform\n");
		return -1;
	}

	// Grab video buffer
	uint32_t stride = VPUGetStride(EVM_320_Wide, ECM_16bit_RGB);
	SPSizeAlloc* framebuffer = new SPSizeAlloc();
	framebuffer->size = stride*240;
	SPAllocateBuffer(platform, framebuffer);

	// Set up the video mode and frame pointers
	VPUSetVideoMode(platform->vx, EVM_320_Wide, ECM_16bit_RGB, EVS_Enable);
	platform->sc->cycle = 0;
	platform->sc->framebufferA = framebuffer; // Not double-buffering
	platform->sc->framebufferB = framebuffer;
	VPUSwapPages(platform->vx, platform->sc);
	VPUClear(platform->vx, 0x00000000);

	float R = 4.0E-6f + 0.01f;

	SThreadData *threadData1;
	SThreadData *threadData2;
	pthread_t thread1;
	pthread_t thread2;
	pthread_attr_t attr1;
	pthread_attr_t attr2;
	cpu_set_t cpuset1;
	cpu_set_t cpuset2;

	threadData1 = new SThreadData();
	threadData2 = new SThreadData();
	InitThreadData(threadData1, platform, framebuffer, 0, R, 0, 0);
	InitThreadData(threadData2, platform, framebuffer, 1, R, 0, 0);

	CPU_ZERO(&cpuset1);
	CPU_ZERO(&cpuset2);
	CPU_SET(0, &cpuset1); // Only on CPU#0
	CPU_SET(1, &cpuset2); // Only on CPU#1

	pthread_attr_init(&attr1);
	pthread_attr_setaffinity_np(&attr1, sizeof(cpu_set_t), &cpuset1);
	pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_DETACHED);

	pthread_attr_init(&attr2);
	pthread_attr_setaffinity_np(&attr2, sizeof(cpu_set_t), &cpuset2);
	pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_DETACHED);

	// Create threads
	int success = pthread_create(&thread1, &attr1, mandelbrot, (void*)threadData1);
	if (success != 0) {
		printf("Failed to create thread 1\n");
		return -1;
	}

	success = pthread_create(&thread2, &attr2, mandelbrot, (void*)threadData2);
	if (success != 0) {
		printf("Failed to create thread 2\n");
		return -1;
	}

	int tilex = 0;
	int tiley = 0;
	while(1)
	{
		if (threadData1->go.load() == false)
		{
			PickNextTile(&tilex, &tiley, &R);
			threadData1->tilex = tilex;
			threadData1->tiley = tiley;
			threadData1->R = R;
			threadData1->go.store(true);
		}

		if (threadData2->go.load() == false)
		{
			PickNextTile(&tilex, &tiley, &R);
			threadData2->tilex = tilex;
			threadData2->tiley = tiley;
			threadData2->R = R;
			threadData2->go.store(true);
		}

		sched_yield();
	}

	//pthread_attr_destroy(&attr1);
	//pthread_attr_destroy(&attr2);

	return 0;
}
