/** \file
 * JPEG image viewer example
 *
 * \ingroup examples
 * This example demonstrates how to decode a JPEG image and display it on the screen.
 * It uses the NanoJPEG library to decode the JPEG image.
 */

#include "platform.h"
#include "video.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "nanojpeg.h"

#define VIDEO_MODE      EVM_640_Wide
#define VIDEO_COLOR     ECM_16bit_RGB
#define VIDEO_WIDTH     640
#define VIDEO_HEIGHT    480

static struct EVideoContext s_vctx;
static struct EVideoSwapContext s_sctx;
struct SPSizeAlloc frameBuffer;
static struct SPPlatform s_platform;

void shutdowncleanup()
{
	// Turn off video scan-out
	VPUSetVideoMode(&s_vctx, VIDEO_MODE, VIDEO_COLOR, EVS_Disable);

	// Yield physical memory and reset video routines
	VPUShutdownVideo();

	// Release allocations
	SPFreeBuffer(&s_platform, &frameBuffer);

	// Shutdown platform
	SPShutdownPlatform(&s_platform);
}

void sigint_handler(int s)
{
	shutdowncleanup();
	exit(0);
}

uint16_t *image;

#define min(_x_,_y_) (_x_) < (_y_) ? (_x_) : (_y_)
#define max(_x_,_y_) (_x_) > (_y_) ? (_x_) : (_y_)

void DecodeJPEG(uint32_t stride, const char *fname)
{
	njInit();

	FILE *fp = fopen(fname, "rb");
	if (fp)
	{
		// Grab file size
		fpos_t pos, endpos;
		fgetpos(fp, &pos);
		fseek(fp, 0, SEEK_END);
		fgetpos(fp, &endpos);
		fsetpos(fp, &pos);
		uint32_t fsize = (uint32_t)endpos.__pos;

		printf("Reading %ld bytes\n", fsize);
		uint8_t *rawjpeg = (uint8_t *)malloc(fsize);
		fread(rawjpeg, fsize, 1, fp);
		fclose(fp);

		printf("Decoding image\n");
		nj_result_t jres = njDecode(rawjpeg, fsize);

		if (jres == NJ_OK)
		{
			int W = njGetWidth();
			int H = njGetHeight();

			int iW = W>=VIDEO_WIDTH ? VIDEO_WIDTH : W;
			int iH = H>=VIDEO_HEIGHT ? VIDEO_HEIGHT : H;

			uint8_t *img = njGetImage();
			if (njIsColor())
			{
				// Copy, dither and convert to indexed color
				for (int y=0;y<iH;++y)
				{
					for (int x=0;x<iW;++x)
					{
						uint32_t red = uint32_t(15.f*float(img[(x+y*W)*3+0])/255.f);
						uint32_t green = uint32_t(15.f*float(img[(x+y*W)*3+1])/255.f);
						uint32_t blue = uint32_t(15.f*float(img[(x+y*W)*3+2])/255.f);
						image[x+y*stride] = MAKECOLORRGB12(red, green, blue);
					}
				}
			}
			else
			{
				// Grayscale
				for (int j=0;j<iH;++j)
					for (int i=0;i<iW;++i)
					{
						uint8_t V = img[i+j*W]>>4;
						image[i+j*stride] = MAKECOLORRGB12(V,V,V);
					}
			}
		}
		free(rawjpeg);
	}
	else
		printf("Could not open file %s\n", fname);

	njDone();
}

int main(int argc, char** argv )
{
	SPInitPlatform(&s_platform);
	VPUInitVideo(&s_vctx, &s_platform);
	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	frameBuffer.size = stride*VIDEO_HEIGHT;
	SPAllocateBuffer(&s_platform, &frameBuffer);

	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);
	signal(SIGTERM, &sigint_handler);

	// Set aside space for the decompressed image
	// NOTE: Video scanout buffer has to be aligned at 64 byte boundary
	image = (uint16_t*)frameBuffer.cpuAddress;

	VPUSetWriteAddress(&s_vctx, (uint32_t)frameBuffer.cpuAddress);
	VPUSetScanoutAddress(&s_vctx, (uint32_t)frameBuffer.dmaAddress);
	VPUSetDefaultPalette(&s_vctx);
	VPUSetVideoMode(&s_vctx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	if (argc<=1)
	{
		printf("Usage: %s <image.jpg>\n", argv[0]);
		return 1;
	}	
	else
	{
		DecodeJPEG(stride/sizeof(uint16_t), argv[1]);
	}

	// Hold image while we view it
	while(1){}

	return 0;
}
