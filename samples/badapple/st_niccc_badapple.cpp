/*
* Reading the ST-NICCC megademo data stored in
* the SDCard and streaming it to polygons,
* rendered in the framebuffer.
* 
* The polygon stream is a 640K file 
* (C_EXAMPLES/DATA/scene1.dat), that needs to 
* be stored on the SD card. 
*
* More details and links in C_EXAMPLES/DATA/notes.txt
*/

// Please see https://github.com/BrunoLevy/Vectorizer for the original code

#include "core.h"
#include "vpu.h"
#include "io.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

int cur_byte_address = 0;

FILE *s_fp;
uint8_t *filedata;

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define VIDEO_MODE      EVM_320_Wide
#define VIDEO_COLOR     ECM_8bit_Indexed
#define VIDEO_HEIGHT    240

struct EVideoContext s_vctx;
struct EVideoSwapContext s_sctx;
struct SPSizeAlloc frameBufferA;
struct SPSizeAlloc frameBufferB;
static struct SPPlatform s_platform;

void shutdowncleanup()
{
	// Switch to fbcon buffer
	VPUSetScanoutAddress(&s_vctx, 0x18000000);
	VPUSetVideoMode(&s_vctx, EVM_640_Wide, ECM_16bit_RGB, EVS_Enable);

	// Yield physical memory and reset video routines
	VPUShutdownVideo();

	// Release allocations
	SPFreeBuffer(&s_platform, &frameBufferB);
	SPFreeBuffer(&s_platform, &frameBufferA);

	// Shutdown platform
	SPShutdownPlatform(&s_platform);
}

void sigint_handler(int s)
{
	shutdowncleanup();
	exit(0);
}

void gfx_fillpoly(uint8_t* buffer, uint32_t stride, int nb_pts, int* points, uint8_t color)
{
    int x_left[256];
    int x_right[256];

    /* Determine clockwise, miny, maxy */
    int clockwise = 0;
    int miny =  1024;
    int maxy = -1024;
    
    for(int i1=0; i1<nb_pts; ++i1)
	{
		int i2=(i1==nb_pts-1) ? 0 : i1+1;
		int i3=(i2==nb_pts-1) ? 0 : i2+1;
		int x1 = points[2*i1];
		int y1 = points[2*i1+1];
		int dx1 = points[2*i2]   - x1;
		int dy1 = points[2*i2+1] - y1;
		int dx2 = points[2*i3]   - x1;
		int dy2 = points[2*i3+1] - y1;
		clockwise += dx1 * dy2 - dx2 * dy1;
		miny = MIN(miny,y1);
		maxy = MAX(maxy,y1);
    }

    /* Determine x_left and x_right for each scaline */
    for(int i1=0; i1<nb_pts; ++i1)
	{
		int i2=(i1==nb_pts-1) ? 0 : i1+1;

		int x1 = points[2*i1];
		int y1 = points[2*i1+1];
		int x2 = points[2*i2];
		int y2 = points[2*i2+1];

		int* x_buffer = ((clockwise > 0) ^ (y2 > y1)) ? x_left : x_right;

        // ensure consistent rasterization of neighboring edges in
        // a triangulation, avoid small gaps
        if(y2 < y1)
		{
            int tmp = y1;
            y1 = y2;
            y2 = tmp;
            tmp = x1;
            x1 = x2;
            x2 = tmp;
        }
        
		int dx = x2 - x1;
		int sx = 1;
		int dy = y2 - y1;
		int sy = 1;
		int x = x1;
		int y = y1;
		int ex;
		
		if(dx < 0)
		{
			sx = -1;
			dx = -dx;
		}
	
		if(dy < 0)
		{
			sy = -1;
			dy = -dy;
		}

		if(y1 == y2)
		{
			x_left[y1]  = MIN(x1,x2);
			x_right[y1] = MAX(x1,x2);
			continue;
		}

		ex = (dx << 1) - dy;

		for(int u=0; u <= dy; ++u)
		{
			x_buffer[y] = x; 
			y += sy;
			while(ex >= 0) {
				x += sx;
				ex -= dy << 1;
			}
			ex += dx << 1;
		}
	}

	for(int y = miny; y <= maxy; ++y)
		for(int x = x_left[y]; x <= x_right[y]; ++x)
			buffer[y * stride + x] = color;
}

int main(int argc, char** argv)
{
	char scene_file[256];
	if (argc>=2)
		strcpy(scene_file, argv[1]);
	else
		strcpy(scene_file, "scene1.bin");

	ST_NICCC_IO io;
	ST_NICCC_FRAME frame;
	ST_NICCC_POLYGON polygon;

	if(!st_niccc_open(&io,scene_file,ST_NICCC_READ))
	{
        	fprintf(stderr,"could not open data file\n");
	        exit(-1);
    	}

	SPInitPlatform(&s_platform);
	VPUInitVideo(&s_vctx, &s_platform);
	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	frameBufferB.size = frameBufferA.size = stride*VIDEO_HEIGHT;
	SPAllocateBuffer(&s_platform, &frameBufferA);
	SPAllocateBuffer(&s_platform, &frameBufferB);

	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);
	signal(SIGTERM, &sigint_handler);

	VPUSetVideoMode(&s_vctx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	s_sctx.cycle = 0;
	s_sctx.framebufferA = &frameBufferA;
	s_sctx.framebufferB = &frameBufferB;
	VPUSwapPages(&s_vctx, &s_sctx);
	VPUClear(&s_vctx, 0x00000000);
	VPUSwapPages(&s_vctx, &s_sctx);
	VPUClear(&s_vctx, 0x00000000);

	// More than one parameter on command line triggers no-vsync mode
	int haveVsync = argc <= 2 ? 1 : 0;

	for(;;)
	{
		st_niccc_rewind(&io);
		while(st_niccc_read_frame(&s_vctx, &io, &frame))
		{
			if(frame.flags & CLEAR_BIT)
				VPUClear(&s_vctx, 0x07070707);

			while(st_niccc_read_polygon(&io, &frame, &polygon))
				gfx_fillpoly(s_sctx.writepage, stride, polygon.nb_vertices, polygon.XY, polygon.color);

			if (haveVsync)
				VPUWaitVSync(&s_vctx);
			VPUSwapPages(&s_vctx, &s_sctx);
		}
	}
}
