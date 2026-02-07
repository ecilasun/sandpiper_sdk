/**
 * \file jpg.cpp
 * \brief Interactive JPEG image viewer with zoom and pan
 *
 * \ingroup examples
 * This example demonstrates an interactive JPEG viewer with:
 * - Mouse click-and-drag to pan
 * - Mouse wheel to zoom in/out
 * - Arrow keys to pan, +/- to zoom
 * - ESC key to open file browser
 */

#include "platform.h"
#include "vpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <limits.h>
#include <linux/input.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <string>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif
#include "nanojpeg.h"
#ifdef __cplusplus
}
#endif

#define VIDEO_MODE      EVM_640_480
#define VIDEO_COLOR     ECM_16bit_RGB
#define VIDEO_WIDTH     640
#define VIDEO_HEIGHT    480

static struct SPPlatform* s_platform = NULL;
struct SPSizeAlloc frameBufferA;
struct SPSizeAlloc frameBufferB;

static int havekeyboard = 1;
static int havemouse = 1;
static struct pollfd fds[2];

uint8_t *fullImage = NULL;  // Full resolution RGB image data
int imageWidth = 0;
int imageHeight = 0;
bool imageIsColor = true;

// View state
float panX = 0.0f;  // Pan offset in image coordinates
float panY = 0.0f;
float zoom = 1.0f;  // Zoom level (1.0 = fit to screen)

// Mouse state
bool mouseDown = false;
int lastMouseX = 0;
int lastMouseY = 0;
int mouseX = VIDEO_WIDTH / 2;  // Start at center
int mouseY = VIDEO_HEIGHT / 2;

#define min(_x_,_y_) ((_x_) < (_y_) ? (_x_) : (_y_))
#define max(_x_,_y_) ((_x_) > (_y_) ? (_x_) : (_y_))
#define clamp(_v_,_lo_,_hi_) ((_v_) < (_lo_) ? (_lo_) : ((_v_) > (_hi_) ? (_hi_) : (_v_)))

// Load full resolution JPEG into RAM
bool LoadJPEG(const char *fname)
{
	// Free previous image if any
	if (fullImage)
	{
		free(fullImage);
		fullImage = NULL;
	}

	njInit();
	bool success = false;

	FILE *fp = fopen(fname, "rb");
	if (fp)
	{
		// Get file size
		fpos_t pos, endpos;
		fgetpos(fp, &pos);
		fseek(fp, 0, SEEK_END);
		fgetpos(fp, &endpos);
		fsetpos(fp, &pos);
		uint32_t fsize = (uint32_t)endpos.__pos;

		printf("Loading %s (%ld bytes)\n", fname, (long int)fsize);
		uint8_t *rawjpeg = (uint8_t *)malloc(fsize);
		size_t readsize = fread(rawjpeg, 1, fsize, fp);
		fclose(fp);

		if (readsize == fsize)
		{
			nj_result_t jres = njDecode(rawjpeg, fsize);
			
			if (jres == NJ_OK)
			{
				imageWidth = njGetWidth();
				imageHeight = njGetHeight();
				imageIsColor = njIsColor();
				
				printf("Decoded: %dx%d %s\n", imageWidth, imageHeight, 
				       imageIsColor ? "color" : "grayscale");
				
				// Allocate and copy full image data
				int channels = imageIsColor ? 3 : 1;
				fullImage = (uint8_t*)malloc(imageWidth * imageHeight * channels);
				memcpy(fullImage, njGetImage(), imageWidth * imageHeight * channels);
				
				// Reset view to fit image
				float wScale = (float)VIDEO_WIDTH / imageWidth;
				float hScale = (float)VIDEO_HEIGHT / imageHeight;
				zoom = min(wScale, hScale);
				panX = 0.0f;
				panY = 0.0f;
				
				success = true;
			}
			else
			{
				printf("Failed to decode JPEG: error %d\n", jres);
			}
		}
		free(rawjpeg);
	}
	else
	{
		printf("Could not open file %s\n", fname);
	}

	njDone();
	return success;
}

// Render the current view to the display buffer
void RenderView(uint16_t* displayBuffer, uint32_t stride)
{
	if (!fullImage)
		return;
	
	// Don't clear buffer - draw every pixel to avoid flicker
	// memset would cause black flashes as the display scans out
	
	for (int screenY = 0; screenY < VIDEO_HEIGHT; screenY++)
	{
		for (int screenX = 0; screenX < VIDEO_WIDTH; screenX++)
		{
			// Map screen coordinate to image coordinate
			float imgX = panX + (float)screenX / zoom;
			float imgY = panY + (float)screenY / zoom;
			
			int ix = (int)imgX;
			int iy = (int)imgY;
			
			// Check bounds
			if (ix >= 0 && ix < imageWidth && iy >= 0 && iy < imageHeight)
			{
				if (imageIsColor)
				{
					int idx = (ix + iy * imageWidth) * 3;
					uint32_t red = (uint32_t)(31.0f * fullImage[idx + 0] / 255.0f);
					uint32_t green = (uint32_t)(63.0f * fullImage[idx + 1] / 255.0f);
					uint32_t blue = (uint32_t)(31.0f * fullImage[idx + 2] / 255.0f);
					displayBuffer[screenX + screenY * stride] = MAKECOLORRGB16(red, green, blue);
				}
				else
				{
					uint8_t gray = fullImage[ix + iy * imageWidth];
					uint32_t gray5 = (uint32_t)(31.0f * gray / 255.0f);
					uint32_t gray6 = (uint32_t)(63.0f * gray / 255.0f);
					displayBuffer[screenX + screenY * stride] = MAKECOLORRGB16(gray5, gray6, gray5);
				}
			}
			else
			{
				// Draw black for areas outside the image
				displayBuffer[screenX + screenY * stride] = 0;
			}
		}
	}
}

// Draw mouse cursor as an arrow
void DrawCursor(uint16_t* displayBuffer, uint32_t stride, int cx, int cy)
{
	if (cx < 0 || cy < 0 || cx >= VIDEO_WIDTH || cy >= VIDEO_HEIGHT)
		return;
	
	// White cursor (RGB565: 5 bits red, 6 bits green, 5 bits blue)
	const uint16_t white = MAKECOLORRGB16(31, 63, 31);
	const uint16_t black = MAKECOLORRGB16(0, 0, 0);
	
	// Arrow cursor pattern - simple arrow shape
	// 0 = transparent (don't draw), 1 = black outline, 2 = white fill
	const char arrow[13][8] = {
		{2, 0, 0, 0, 0, 0, 0, 0},  // *
		{2, 2, 0, 0, 0, 0, 0, 0},  // **
		{2, 2, 2, 0, 0, 0, 0, 0},  // ***
		{2, 2, 2, 2, 0, 0, 0, 0},  // ****
		{2, 2, 2, 2, 2, 0, 0, 0},  // *****
		{2, 2, 2, 2, 2, 2, 0, 0},  // ******
		{2, 2, 2, 2, 2, 2, 2, 0},  // *******
		{2, 2, 2, 2, 2, 0, 0, 0},  // *****
		{2, 2, 0, 2, 2, 2, 0, 0},  // **  ***
		{2, 0, 0, 2, 2, 2, 0, 0},  // *   ***
		{0, 0, 0, 0, 2, 2, 2, 0},  //     ***
		{0, 0, 0, 0, 2, 2, 0, 0},  //     **
		{0, 0, 0, 0, 0, 0, 0, 0}
	};
	
	// First pass: draw black outline
	for (int row = 0; row < 13; row++)
	{
		int y = cy + row;
		if (y < 0 || y >= VIDEO_HEIGHT) continue;
		
		for (int col = 0; col < 8; col++)
		{
			if (arrow[row][col] == 0) continue;
			
			int x = cx + col;
			if (x < 0 || x >= VIDEO_WIDTH) continue;
			
			// Draw black outline around filled pixels
			if (x > 0 && arrow[row][col] == 2)
			{
				if (col == 0 || arrow[row][col-1] == 0)
					displayBuffer[(x-1) + y * stride] = black;
			}
			if (x < VIDEO_WIDTH - 1 && arrow[row][col] == 2)
			{
				if (col == 7 || arrow[row][col+1] == 0)
					displayBuffer[(x+1) + y * stride] = black;
			}
			if (y > 0 && arrow[row][col] == 2)
			{
				if (row == 0 || arrow[row-1][col] == 0)
					displayBuffer[x + (y-1) * stride] = black;
			}
			if (y < VIDEO_HEIGHT - 1 && arrow[row][col] == 2)
			{
				if (row == 12 || arrow[row+1][col] == 0)
					displayBuffer[x + (y+1) * stride] = black;
			}
		}
	}
	
	// Second pass: draw white fill
	for (int row = 0; row < 13; row++)
	{
		int y = cy + row;
		if (y < 0 || y >= VIDEO_HEIGHT) continue;
		
		for (int col = 0; col < 8; col++)
		{
			if (arrow[row][col] == 2)  // White fill
			{
				int x = cx + col;
				if (x >= 0 && x < VIDEO_WIDTH)
					displayBuffer[x + y * stride] = white;
			}
		}
	}
}

// Clamp pan values to keep image visible
void ClampPan()
{
	float visibleWidth = VIDEO_WIDTH / zoom;
	float visibleHeight = VIDEO_HEIGHT / zoom;
	
	panX = clamp(panX, -visibleWidth * 0.9f, imageWidth - visibleWidth * 0.1f);
	panY = clamp(panY, -visibleHeight * 0.9f, imageHeight - visibleHeight * 0.1f);
}

int main(int argc, char** argv)
{
	s_platform = SPInitPlatform();
	if (!s_platform)
	{
		fprintf(stderr, "Failed to init platform\n");
		return -1;
	}

	// Set up double buffering
	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	frameBufferA.size = stride * VIDEO_HEIGHT;
	frameBufferB.size = stride * VIDEO_HEIGHT;
	SPAllocateBuffer(s_platform, &frameBufferA);
	SPAllocateBuffer(s_platform, &frameBufferB);

	// Clear both buffers
	memset(frameBufferA.cpuAddress, 0, stride * VIDEO_HEIGHT);
	memset(frameBufferB.cpuAddress, 0, stride * VIDEO_HEIGHT);

	// Set up video mode
	VPUSetVideoMode(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);
	
	// Set up swap context for double buffering
	s_platform->sc->cycle = 0;
	s_platform->sc->framebufferA = &frameBufferA;
	s_platform->sc->framebufferB = &frameBufferB;
	VPUSwapPages(s_platform->vx, s_platform->sc);

	std::string currentFile;
	
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <jpeg_file>\n", argv[0]);
		return -1;
	}
	
	currentFile = argv[1];

	// Set up keyboard device
	fds[0].fd = SPFindKeyboardDevice();
	fds[0].events = POLLIN;
	
	if (fds[0].fd < 0)
	{
		printf("Could not find keyboard device. Make sure a keyboard is connected.\n");
		havekeyboard = 0;
	}
	
	// Set up mouse device
	fds[1].fd = SPFindMouseDevice();
	fds[1].events = POLLIN;
	
	if (fds[1].fd < 0)
	{
		printf("Could not find mouse device. Mouse controls disabled.\n");
		havemouse = 0;
	}

	// Load initial image if provided
	if (!currentFile.empty())
	{
		LoadJPEG(currentFile.c_str());
	}
	
	// Main loop
	bool running = true;
	int framesToRedraw = 2;  // Render 2 frames initially to fill both buffers
	
	while (running)
	{
		// Process mouse events
		if (havemouse)
		{
			int ret = poll(&fds[1], 1, 0);
			if (ret > 0)
			{
				struct input_event ev;
				while (read(fds[1].fd, &ev, sizeof(struct input_event)) == sizeof(struct input_event))
				{
					if (ev.type == EV_REL)
					{
						if (ev.code == REL_X)
						{
							mouseX += ev.value;
							mouseX = clamp(mouseX, 0, VIDEO_WIDTH - 1);
							if (mouseDown)
							{
								panX -= ev.value / zoom;
								ClampPan();
							}
							framesToRedraw = 2;  // Always redraw to update cursor
						}
						if (ev.code == REL_Y)
						{
							mouseY += ev.value;
							mouseY = clamp(mouseY, 0, VIDEO_HEIGHT - 1);
							if (mouseDown)
							{
								panY -= ev.value / zoom;
								ClampPan();
							}
							framesToRedraw = 2;  // Always redraw to update cursor
						}
						if (ev.code == REL_WHEEL)
						{
							// Zoom in/out
							float oldZoom = zoom;
							if (ev.value > 0)
								zoom *= 1.1f;  // Zoom in
							else
								zoom *= 0.9f;  // Zoom out
							
							zoom = clamp(zoom, 0.1f, 10.0f);
							
							// Adjust pan to zoom towards mouse position
							float zoomRatio = zoom / oldZoom;
							panX = mouseX / zoom + (panX - mouseX / oldZoom) * (oldZoom / zoom);
							panY = mouseY / zoom + (panY - mouseY / oldZoom) * (oldZoom / zoom);
							
							ClampPan();
							framesToRedraw = 2;
						}
					}
					else if (ev.type == EV_KEY)
					{
						if (ev.code == BTN_LEFT)
						{
							mouseDown = (ev.value != 0);
							if (mouseDown)
							{
								lastMouseX = mouseX;
								lastMouseY = mouseY;
							}
						}
					}
				}
			}
		}
		
		// Process keyboard events
		if (havekeyboard)
		{
			int ret = poll(&fds[0], 1, 0);
			if (ret > 0)
			{
				struct input_event ev;
				while (read(fds[0].fd, &ev, sizeof(struct input_event)) == sizeof(struct input_event))
				{
					if (ev.type == EV_KEY && ev.value == 1)  // Key press
					{
						if (ev.code == KEY_Q)
						{
							running = false;
						}
						// Arrow keys for panning
						else if (ev.code == KEY_LEFT)
						{
							panX -= 20.0f / zoom;
							ClampPan();
							framesToRedraw = 2;
						}
						else if (ev.code == KEY_RIGHT)
						{
							panX += 20.0f / zoom;
							ClampPan();
							framesToRedraw = 2;
						}
						else if (ev.code == KEY_UP)
						{
							panY -= 20.0f / zoom;
							ClampPan();
							framesToRedraw = 2;
						}
						else if (ev.code == KEY_DOWN)
						{
							panY += 20.0f / zoom;
							ClampPan();
							framesToRedraw = 2;
						}
						// +/- keys for zoom
						else if (ev.code == KEY_EQUAL || ev.code == KEY_KPPLUS)
						{
							zoom *= 1.2f;
							zoom = clamp(zoom, 0.1f, 10.0f);
							ClampPan();
							framesToRedraw = 2;
						}
						else if (ev.code == KEY_MINUS || ev.code == KEY_KPMINUS)
						{
							zoom *= 0.8f;
							zoom = clamp(zoom, 0.1f, 10.0f);
							ClampPan();
							framesToRedraw = 2;
						}
						// R to reset view
						else if (ev.code == KEY_R)
						{
							float wScale = (float)VIDEO_WIDTH / imageWidth;
							float hScale = (float)VIDEO_HEIGHT / imageHeight;
							zoom = min(wScale, hScale);
							panX = 0.0f;
							panY = 0.0f;
							framesToRedraw = 2;
						}
					}
				}
			}
		}
		
		// Get current write buffer
		uint16_t* displayBuffer = (uint16_t*)s_platform->sc->writepage;
		
		// Render if needed (render for 2 frames to update both buffers)
		if (framesToRedraw > 0 && fullImage)
		{
			RenderView(displayBuffer, stride / sizeof(uint16_t));
			framesToRedraw--;
		}
		
		// Always draw cursor if we have mouse
		if (havemouse && fullImage)
		{
			DrawCursor(displayBuffer, stride / sizeof(uint16_t), mouseX, mouseY);
		}
		
		// Wait for vsync and swap buffers
		VPUWaitVSync(s_platform->vx);
		VPUSwapPages(s_platform->vx, s_platform->sc);
	}
	
	// Cleanup
	if (fullImage)
		free(fullImage);
	
	if (havekeyboard)
		close(fds[0].fd);
	if (havemouse)
		close(fds[1].fd);

	return 0;
}
