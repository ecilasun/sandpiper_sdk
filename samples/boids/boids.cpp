/**
 * \file boids.cpp
 * \brief Dual-core boids flocking demo
 *
 * \ingroup examples
 * This example demonstrates a simple boids simulation where the update step
 * is split across two CPU cores. Rendering uses a double-buffered VPU setup.
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#include "core.h"
#include "platform.h"
#include "vpu.h"

#define VIDEO_MODE      EVM_320_240
#define VIDEO_COLOR     ECM_16bit_RGB
#define VIDEO_WIDTH     320
#define VIDEO_HEIGHT    240

static const int kBoidCount = 240;
static const int kWorkerCount = 2;
static const int kObstacleCount = 3;

static const float kNeighborRadius = 24.0f;
static const float kSeparationRadius = 12.0f;
static const float kMaxSpeed = 2.1f;
static const float kMinSpeed = 0.6f;
static const float kAlignWeight = 0.050f;
static const float kCohesionWeight = 0.0030f;
static const float kSeparationWeight = 0.140f;
static const float kObstacleWeight = 0.180f;
static const float kWaterDrag = 0.985f;
static const float kPostBurstDrag = 0.920f;

struct Boid
{
	float x;
	float y;
	float vx;
	float vy;
	float speedBias;
	int burstTimer;
	int burstCooldown;
	float wigglePhase;
	float wiggleSpeed;
	float wiggleAmp;
};

struct Obstacle
{
	float x;
	float y;
	float r;
};

struct WorkerArgs
{
	int startIndex;
	int endIndex;
};

static pthread_mutex_t g_jobMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_jobCond = PTHREAD_COND_INITIALIZER;
static pthread_cond_t g_doneCond = PTHREAD_COND_INITIALIZER;
static int g_frameCounter = 0;
static int g_jobsRemaining = 0;
static bool g_shutdown = false;

static Boid* g_readBoids = NULL;
static Boid* g_writeBoids = NULL;
static float g_dt = 1.0f;
static Obstacle g_obstacles[kObstacleCount];

static inline float frand_range(float lo, float hi)
{
	return lo + (float)rand() / (float)RAND_MAX * (hi - lo);
}

static inline void limit_speed(float* vx, float* vy, float minSpeed, float maxSpeed)
{
	float s2 = (*vx) * (*vx) + (*vy) * (*vy);
	float max2 = maxSpeed * maxSpeed;
	if (s2 > max2)
	{
		float inv = maxSpeed / sqrtf(s2);
		*vx *= inv;
		*vy *= inv;
		return;
	}

	float min2 = minSpeed * minSpeed;
	if (s2 < min2)
	{
		float inv = (s2 > 0.0001f) ? (minSpeed / sqrtf(s2)) : 1.0f;
		*vx *= inv;
		*vy *= inv;
	}
}

static inline void set_pixel16(uint16_t* fb, uint32_t stride, int x, int y, uint16_t color)
{
	if (x < 0 || y < 0 || x >= VIDEO_WIDTH || y >= VIDEO_HEIGHT)
		return;
	fb[y * (stride >> 1) + x] = color;
}

static void draw_filled_circle16(uint16_t* fb, uint32_t stride, int cx, int cy, int radius, uint16_t color)
{
	int r2 = radius * radius;
	for (int y = -radius; y <= radius; ++y)
	{
		int yy = y * y;
		int dx = (int)sqrtf((float)(r2 - yy));
		int x0 = cx - dx;
		int x1 = cx + dx;
		int py = cy + y;
		if (py < 0 || py >= VIDEO_HEIGHT)
			continue;
		if (x0 < 0) x0 = 0;
		if (x1 >= VIDEO_WIDTH) x1 = VIDEO_WIDTH - 1;
		for (int x = x0; x <= x1; ++x)
			set_pixel16(fb, stride, x, py, color);
	}
}

static inline int edge_fn(int x0, int y0, int x1, int y1, int x, int y)
{
	return (x - x0) * (y1 - y0) - (y - y0) * (x1 - x0);
}

static void fill_triangle16(uint16_t* fb, uint32_t stride, int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color)
{
	int minx = x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2);
	int maxx = x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2);
	int miny = y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2);
	int maxy = y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2);

	if (maxx < 0 || maxy < 0 || minx >= VIDEO_WIDTH || miny >= VIDEO_HEIGHT)
		return;

	if (minx < 0) minx = 0;
	if (miny < 0) miny = 0;
	if (maxx >= VIDEO_WIDTH) maxx = VIDEO_WIDTH - 1;
	if (maxy >= VIDEO_HEIGHT) maxy = VIDEO_HEIGHT - 1;

	int area = edge_fn(x0, y0, x1, y1, x2, y2);
	if (area == 0)
		return;

	// 2x2 MSAA - antialiased rasterization
	for (int y = miny; y <= maxy; ++y)
	{
		for (int x = minx; x <= maxx; ++x)
		{
			int inside_count = 0;
			int scaled_area = area * 4;
			
			// Test at 4 sub-pixel positions (2x2 grid)
			for (int sy = y * 2; sy < y * 2 + 2; ++sy)
			{
				for (int sx = x * 2; sx < x * 2 + 2; ++sx)
				{
					int w0 = edge_fn(x1 * 2, y1 * 2, x2 * 2, y2 * 2, sx, sy);
					int w1 = edge_fn(x2 * 2, y2 * 2, x0 * 2, y0 * 2, sx, sy);
					int w2 = edge_fn(x0 * 2, y0 * 2, x1 * 2, y1 * 2, sx, sy);
					
					if ((w0 >= 0 && w1 >= 0 && w2 >= 0 && scaled_area > 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0 && scaled_area < 0))
						inside_count++;
				}
			}
			
			// Draw pixel if any sample is inside the triangle
			if (inside_count > 0)
				set_pixel16(fb, stride, x, y, color);
		}
	}
}

static inline uint16_t color_from_speed(float speed)
{
	float t = speed / kMaxSpeed;
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;
	int r = (int)(8 + t * 23);    // 8..31
	int g = (int)(12 + t * 43);   // 12..55
	int b = (int)(31 - t * 20);   // 11..31
	if (r > 31) r = 31;
	if (g > 63) g = 63;
	if (b > 31) b = 31;
	return MAKECOLORRGB16(r, g, b);
}

static void draw_boid(uint16_t* fb, uint32_t stride, const Boid* b, uint16_t color)
{
	float mag2 = b->vx * b->vx + b->vy * b->vy;
	float inv = (mag2 > 0.0001f) ? (1.0f / sqrtf(mag2)) : 1.0f;
	float dx = b->vx * inv;
	float dy = b->vy * inv;
	float px = -dy;
	float py = dx;

	int x0 = (int)(b->x + dx * 5.0f);
	int y0 = (int)(b->y + dy * 5.0f);
	int x1 = (int)(b->x - dx * 3.0f + px * 2.5f);
	int y1 = (int)(b->y - dy * 3.0f + py * 2.5f);
	int x2 = (int)(b->x - dx * 3.0f - px * 2.5f);
	int y2 = (int)(b->y - dy * 3.0f - py * 2.5f);

	fill_triangle16(fb, stride, x0, y0, x1, y1, x2, y2, color);
}

static inline void wrap_position(float* x, float* y)
{
	if (*x < 0.0f) *x += (float)VIDEO_WIDTH;
	if (*x >= (float)VIDEO_WIDTH) *x -= (float)VIDEO_WIDTH;
	if (*y < 0.0f) *y += (float)VIDEO_HEIGHT;
	if (*y >= (float)VIDEO_HEIGHT) *y -= (float)VIDEO_HEIGHT;
}

static void update_range(int startIndex, int endIndex)
{
	const float neighbor2 = kNeighborRadius * kNeighborRadius;
	const float separation2 = kSeparationRadius * kSeparationRadius;

	for (int i = startIndex; i < endIndex; ++i)
	{
		const Boid* b = &g_readBoids[i];
		float alignX = 0.0f, alignY = 0.0f;
		float cohX = 0.0f, cohY = 0.0f;
		float sepX = 0.0f, sepY = 0.0f;
		int neighborCount = 0;
		int sepCount = 0;

		for (int j = 0; j < kBoidCount; ++j)
		{
			if (i == j)
				continue;

			float dx = g_readBoids[j].x - b->x;
			float dy = g_readBoids[j].y - b->y;

			if (dx > VIDEO_WIDTH * 0.5f) dx -= VIDEO_WIDTH;
			if (dx < -VIDEO_WIDTH * 0.5f) dx += VIDEO_WIDTH;
			if (dy > VIDEO_HEIGHT * 0.5f) dy -= VIDEO_HEIGHT;
			if (dy < -VIDEO_HEIGHT * 0.5f) dy += VIDEO_HEIGHT;

			float dist2 = dx * dx + dy * dy;
			if (dist2 < neighbor2)
			{
				alignX += g_readBoids[j].vx;
				alignY += g_readBoids[j].vy;
				cohX += b->x + dx;
				cohY += b->y + dy;
				neighborCount++;
			}
			if (dist2 < separation2 && dist2 > 0.0001f)
			{
				sepX -= dx / dist2;
				sepY -= dy / dist2;
				sepCount++;
			}
		}

		float steerX = 0.0f;
		float steerY = 0.0f;
		float centerDx = 0.0f;
		float centerDy = 0.0f;

		if (neighborCount > 0)
		{
			alignX /= (float)neighborCount;
			alignY /= (float)neighborCount;
			cohX /= (float)neighborCount;
			cohY /= (float)neighborCount;
			centerDx = cohX - b->x;
			centerDy = cohY - b->y;

			steerX += (alignX - b->vx) * kAlignWeight;
			steerY += (alignY - b->vy) * kAlignWeight;
			steerX += (cohX - b->x) * kCohesionWeight;
			steerY += (cohY - b->y) * kCohesionWeight;
		}
		if (sepCount > 0)
		{
			steerX += sepX * kSeparationWeight;
			steerY += sepY * kSeparationWeight;
		}

		float v2 = b->vx * b->vx + b->vy * b->vy;
		if (v2 > 0.0001f)
		{
			float inv = 1.0f / sqrtf(v2);
			float hx = b->vx * inv;
			float hy = b->vy * inv;
			float px = -hy;
			float py = hx;
			float wiggle = sinf(b->wigglePhase) * b->wiggleAmp;
			steerX += px * wiggle;
			steerY += py * wiggle;
		}

		for (int o = 0; o < kObstacleCount; ++o)
		{
			float dx = g_obstacles[o].x - b->x;
			float dy = g_obstacles[o].y - b->y;

			if (dx > VIDEO_WIDTH * 0.5f) dx -= VIDEO_WIDTH;
			if (dx < -VIDEO_WIDTH * 0.5f) dx += VIDEO_WIDTH;
			if (dy > VIDEO_HEIGHT * 0.5f) dy -= VIDEO_HEIGHT;
			if (dy < -VIDEO_HEIGHT * 0.5f) dy += VIDEO_HEIGHT;

			float avoidDist = g_obstacles[o].r + kSeparationRadius + 4.0f;
			float avoid2 = avoidDist * avoidDist;
			float dist2 = dx * dx + dy * dy;
			if (dist2 < avoid2 && dist2 > 0.0001f)
			{
				float dist = sqrtf(dist2);
				float push = (avoidDist - dist) / avoidDist;
				float inv = 1.0f / dist;
				steerX -= dx * inv * push * kObstacleWeight;
				steerY -= dy * inv * push * kObstacleWeight;
			}
		}

		Boid* out = &g_writeBoids[i];
		out->vx = b->vx + steerX * g_dt;
		out->vy = b->vy + steerY * g_dt;
		float minSpeed = kMinSpeed * b->speedBias;
		float maxSpeed = kMaxSpeed * b->speedBias;

		int burstTimer = b->burstTimer;
		int burstCooldown = b->burstCooldown;
		if (burstCooldown > 0)
			burstCooldown--;

		float speed2 = out->vx * out->vx + out->vy * out->vy;
		float burstDist2 = (kNeighborRadius * 0.6f) * (kNeighborRadius * 0.6f);
		if (burstTimer == 0 && burstCooldown == 0 && neighborCount > 0 && speed2 < (maxSpeed * 0.8f) * (maxSpeed * 0.8f))
		{
			float centerDist2 = centerDx * centerDx + centerDy * centerDy;
			if (centerDist2 > burstDist2)
				burstTimer = 6;
		}

		if (burstTimer > 0)
		{
			float speed = sqrtf(speed2);
			float boost = (maxSpeed - speed) * 0.8f;
			float inv = (speed > 0.001f) ? (1.0f / speed) : 0.0f;
			float hx = out->vx * inv;
			float hy = out->vy * inv;
			if (inv == 0.0f && neighborCount > 0)
			{
				float cdist = sqrtf(centerDx * centerDx + centerDy * centerDy);
				if (cdist > 0.001f)
				{
					hx = centerDx / cdist;
					hy = centerDy / cdist;
				}
			}
			out->vx += hx * boost;
			out->vy += hy * boost;
			burstTimer--;
			if (burstTimer == 0)
				burstCooldown = 20;
		}

		float drag = kWaterDrag;
		if (burstTimer == 0 && burstCooldown > 0)
			drag = kPostBurstDrag;
		out->vx *= drag;
		out->vy *= drag;

		limit_speed(&out->vx, &out->vy, minSpeed, maxSpeed);
		out->speedBias = b->speedBias;
		out->burstTimer = burstTimer;
		out->burstCooldown = burstCooldown;
		out->wigglePhase = b->wigglePhase + b->wiggleSpeed * g_dt;
		if (out->wigglePhase > 6.28318f)
			out->wigglePhase -= 6.28318f;
		out->wiggleSpeed = b->wiggleSpeed;
		out->wiggleAmp = b->wiggleAmp;

		out->x = b->x + out->vx * g_dt;
		out->y = b->y + out->vy * g_dt;
		wrap_position(&out->x, &out->y);
	}
}

static void* worker_main(void* arg)
{
	WorkerArgs* args = (WorkerArgs*)arg;
	int localFrame = 0;

	while (1)
	{
		pthread_mutex_lock(&g_jobMutex);
		while (!g_shutdown && g_frameCounter == localFrame)
			pthread_cond_wait(&g_jobCond, &g_jobMutex);

		if (g_shutdown)
		{
			pthread_mutex_unlock(&g_jobMutex);
			break;
		}

		localFrame = g_frameCounter;
		pthread_mutex_unlock(&g_jobMutex);

		update_range(args->startIndex, args->endIndex);

		pthread_mutex_lock(&g_jobMutex);
		g_jobsRemaining--;
		if (g_jobsRemaining == 0)
			pthread_cond_signal(&g_doneCond);
		pthread_mutex_unlock(&g_jobMutex);
	}

	return NULL;
}

int main(int argc, char** argv)
{
	(void)argc; (void)argv;

	srand((unsigned int)time(NULL));

	struct SPPlatform* platform = SPInitPlatform();
	if (!platform)
	{
		fprintf(stderr, "Failed to init platform\n");
		return -1;
	}

	VPUSetVideoMode(platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	struct SPSizeAlloc frameBufferA;
	struct SPSizeAlloc frameBufferB;
	frameBufferA.size = stride * VIDEO_HEIGHT;
	frameBufferB.size = stride * VIDEO_HEIGHT;
	SPAllocateBuffer(platform, &frameBufferA);
	SPAllocateBuffer(platform, &frameBufferB);

	memset(frameBufferA.cpuAddress, 0, frameBufferA.size);
	memset(frameBufferB.cpuAddress, 0, frameBufferB.size);

	platform->sc->cycle = 1;
	platform->sc->framebufferA = &frameBufferA;
	platform->sc->framebufferB = &frameBufferB;
	VPUSwapPages(platform->vx, platform->sc);

	Boid boidsA[kBoidCount];
	Boid boidsB[kBoidCount];
	g_obstacles[0] = { VIDEO_WIDTH * 0.50f, VIDEO_HEIGHT * 0.50f, 22.0f };
	g_obstacles[1] = { VIDEO_WIDTH * 0.25f, VIDEO_HEIGHT * 0.30f, 16.0f };
	g_obstacles[2] = { VIDEO_WIDTH * 0.75f, VIDEO_HEIGHT * 0.70f, 18.0f };
	for (int i = 0; i < kBoidCount; ++i)
	{
		boidsA[i].x = frand_range(0.0f, (float)VIDEO_WIDTH);
		boidsA[i].y = frand_range(0.0f, (float)VIDEO_HEIGHT);
		boidsA[i].vx = frand_range(-1.0f, 1.0f);
		boidsA[i].vy = frand_range(-1.0f, 1.0f);
		boidsA[i].speedBias = frand_range(0.85f, 1.15f);
		boidsA[i].burstTimer = 0;
		boidsA[i].burstCooldown = (int)frand_range(0.0f, 20.0f);
		boidsA[i].wigglePhase = frand_range(0.0f, 6.28318f);
		boidsA[i].wiggleSpeed = frand_range(0.12f, 0.22f);
		boidsA[i].wiggleAmp = frand_range(0.010f, 0.020f);
		limit_speed(&boidsA[i].vx, &boidsA[i].vy, kMinSpeed * boidsA[i].speedBias, kMaxSpeed * boidsA[i].speedBias);
		boidsB[i] = boidsA[i];
	}

	pthread_t workers[kWorkerCount];
	WorkerArgs workerArgs[kWorkerCount];
	int mid = kBoidCount / 2;
	workerArgs[0].startIndex = 0;
	workerArgs[0].endIndex = mid;
	workerArgs[1].startIndex = mid;
	workerArgs[1].endIndex = kBoidCount;

	for (int i = 0; i < kWorkerCount; ++i)
		pthread_create(&workers[i], NULL, worker_main, &workerArgs[i]);

	Boid* boidsRead = boidsA;
	Boid* boidsWrite = boidsB;

	while (1)
	{
		pthread_mutex_lock(&g_jobMutex);
		g_readBoids = boidsRead;
		g_writeBoids = boidsWrite;
		g_dt = 1.0f;
		g_jobsRemaining = kWorkerCount;
		g_frameCounter++;
		pthread_cond_broadcast(&g_jobCond);
		while (g_jobsRemaining > 0)
			pthread_cond_wait(&g_doneCond, &g_jobMutex);
		pthread_mutex_unlock(&g_jobMutex);

		Boid* tmp = boidsRead;
		boidsRead = boidsWrite;
		boidsWrite = tmp;

		uint16_t* fb = (uint16_t*)platform->sc->writepage;
		memset(fb, 0, stride * VIDEO_HEIGHT);

		uint16_t obstacleColor = MAKECOLORRGB16(6, 10, 14);
		for (int o = 0; o < kObstacleCount; ++o)
			draw_filled_circle16(fb, stride, (int)g_obstacles[o].x, (int)g_obstacles[o].y, (int)g_obstacles[o].r, obstacleColor);

		for (int i = 0; i < kBoidCount; ++i)
		{
			float speed = sqrtf(boidsRead[i].vx * boidsRead[i].vx + boidsRead[i].vy * boidsRead[i].vy);
			uint16_t color = color_from_speed(speed);
			draw_boid(fb, stride, &boidsRead[i], color);
		}

		VPUWaitVSync(platform->vx);
		VPUSwapPages(platform->vx, platform->sc);
	}

	return 0;
}
