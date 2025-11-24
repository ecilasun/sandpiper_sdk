#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "agnes.h"

#include "core.h"
#include "platform.h"
#include "vpu.h"
#include "vcp.h"

static agnes_input_t s_input;
static agnes_t *s_agnes;
static bool s_alive = true;

#define VIDEO_MODE		EVM_320_Wide
#define VIDEO_COLOR		ECM_8bit_Indexed
#define VIDEO_HEIGHT	240

static struct SPPlatform* s_platform = NULL;
struct SPSizeAlloc frameBufferA;
struct SPSizeAlloc frameBufferB;

static void* read_file(const char *filename, size_t *out_len);

uint32_t videoCallback(uint32_t interval, void* param)
{
	/*uint32_t* pixels = (uint32_t*)s_surface->pixels;
	uint32_t W = s_surface->w;
	uint32_t H = s_surface->h-8;

	agnes_color_t *palette = agnes_get_palette(s_agnes);

    uint8_t *source = agnes_get_raw_screen_buffer(s_agnes);
    for (uint32_t y = 0; y<AGNES_SCREEN_HEIGHT; ++y)
	{
        for (uint32_t x = 0; x<AGNES_SCREEN_WIDTH; ++x)
        {
            uint32_t color = source[x+AGNES_SCREEN_WIDTH*y];
            uint32_t col = SDL_MapRGBA(s_surface->format, palette[color].r, palette[color].g, palette[color].b, 255);
            pixels[0+x*2+W*y*2] = col;
            pixels[1+x*2+W*y*2] = col;
            pixels[0+x*2+W*(y*2+1)] = col;
            pixels[1+x*2+W*(y*2+1)] = col;
        }
	}

	if (SDL_MUSTLOCK(s_surface))
		SDL_UnlockSurface(s_surface);
	SDL_UpdateWindowSurface(s_window);

    s_frame++;*/

	return interval;
}

/*void get_input(SDL_Event *ev, agnes_input_t *out_input)
{
	if (ev==NULL)
	{
		// Read joystick input and convert to agnes_input_t
		if (s_numjoysticks > 0 && s_joystick != NULL)
		{
			out_input->a = SDL_GameControllerGetButton(s_controller, SDL_CONTROLLER_BUTTON_A);
			out_input->b = SDL_GameControllerGetButton(s_controller, SDL_CONTROLLER_BUTTON_B);
			out_input->start = SDL_GameControllerGetButton(s_controller, SDL_CONTROLLER_BUTTON_START);
			out_input->select = SDL_GameControllerGetButton(s_controller, SDL_CONTROLLER_BUTTON_BACK);
			out_input->up = SDL_GameControllerGetButton(s_controller, SDL_CONTROLLER_BUTTON_DPAD_UP);
			out_input->down = SDL_GameControllerGetButton(s_controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
			out_input->left = SDL_GameControllerGetButton(s_controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
			out_input->right = SDL_GameControllerGetButton(s_controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
		}
	}
	else
	{
		// Convert SDL2 events to agnes_input_t
		if (ev->type == SDL_KEYDOWN || ev->type == SDL_KEYUP)
		{
			SDL_Keycode key = ev->key.keysym.sym;
			switch(key)
			{
				case SDLK_RETURN:   { out_input->start = ev->type == SDL_KEYDOWN; break; }
				case SDLK_TAB:      { out_input->select = ev->type == SDL_KEYDOWN; break; }
				case SDLK_a:        { out_input->left = ev->type == SDL_KEYDOWN; break; }
				case SDLK_d:        { out_input->right = ev->type == SDL_KEYDOWN; break; }
				case SDLK_w:        { out_input->up = ev->type == SDL_KEYDOWN; break; }
				case SDLK_s:        { out_input->down = ev->type == SDL_KEYDOWN; break; }
				case SDLK_COMMA:    { out_input->a = ev->type == SDL_KEYDOWN; break; }
				case SDLK_PERIOD:   { out_input->b = ev->type == SDL_KEYDOWN; break; }
				default:            break;
			}
		}
	}
}*/

int main(int argc, char** argv)
{
	if (argc != 2) {
        fprintf(stderr, "Usage: %s filename.nes\n", argv[0]);
        return 1;
    }

    const char *ines_name = argv[1];

    size_t ines_data_size = 0;
    void* ines_data = read_file(ines_name, &ines_data_size);
    if (ines_data == NULL) {
        fprintf(stderr, "Reading %s failed.\n", ines_name);
        return 1;
    }
    
    s_agnes = agnes_make();
    if (s_agnes == NULL) {
        fprintf(stderr, "agnes startup failed.\n");
        return 1;
    }

    bool ok = agnes_load_ines_data(s_agnes, ines_data, ines_data_size);
    if (!ok) {
        fprintf(stderr, "Loading '%s' failed.\n", ines_name);
        return 1;
    }

	fprintf(stderr, "Starting emulator\n");

	s_platform = SPInitPlatform();
	VPUSetVideoMode(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	frameBufferB.size = frameBufferA.size = stride*VIDEO_HEIGHT;
	SPAllocateBuffer(s_platform, &frameBufferA);
	SPAllocateBuffer(s_platform, &frameBufferB);

	for (int y = 0; y < VIDEO_HEIGHT; y++)
	{
		for (int x = 0; x < stride/4; x++)
		{
			uint32_t* pixelA = (uint32_t*)frameBufferA.cpuAddress + (y * stride/4) + x;
			*pixelA = 0x00000000;
			uint32_t* pixelB = (uint32_t*)frameBufferB.cpuAddress + (y * stride/4) + x;
			*pixelB = 0x00000000;
		}
	}

	// Using VPU assisted vsync
	VPUSetScanoutAddress(s_platform->vx, (uint32_t)frameBufferA.dmaAddress);
	VPUSetScanoutAddress2(s_platform->vx, (uint32_t)frameBufferB.dmaAddress);
	s_platform->sc->cycle = 0;
	s_platform->sc->framebufferA = &frameBufferA;
	s_platform->sc->framebufferB = &frameBufferB;

    memset(&s_input, 0, sizeof(agnes_input_t));
	do
	{
		//get_input(&ev, &s_input);
		agnes_set_input(s_agnes, &s_input, NULL);
		s_alive = agnes_next_frame(s_agnes);

		// Vsync barrier
		// Wait for previous frame (if any) to consume swap command + barrier, then swap buffers
		while(VPUGetFIFONotEmpty(s_platform->vx)) { }
		VPUSwapPages(s_platform->vx, s_platform->sc);

		agnes_color_t *palette = agnes_get_palette(s_agnes);
		for (uint32_t i = 0; i < 256; ++i)
			VPUSetPal(s_platform->vx, i, palette[i].r, palette[i].g, palette[i].b);

		uint8_t *source = agnes_get_raw_screen_buffer(s_agnes);
		uint8_t *dest = (uint8_t*)s_platform->sc->writepage;
		for (uint32_t y = 0; y<AGNES_SCREEN_HEIGHT; ++y)
		{
			for (uint32_t x = 0; x<AGNES_SCREEN_WIDTH; ++x)
			{
				uint32_t color = source[x+AGNES_SCREEN_WIDTH*y];
				dest[32 + x+y*stride] = color;
			}
		}

		VPUSyncSwap(s_platform->vx, 0);
		VPUNoop(s_platform->vx);

	} while(s_alive);

    agnes_destroy(s_agnes);

    return 0;
}

static void* read_file(const char *filename, size_t *out_len)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
	{
        return NULL;
    }
    fseek(fp, 0L, SEEK_END);
    long pos = ftell(fp);
    if (pos < 0)
	{
        fclose(fp);
        return NULL;
    }
    size_t file_size = pos;
    rewind(fp);
    unsigned char *file_contents = (unsigned char *)malloc(file_size);
    if (!file_contents)
	{
        fclose(fp);
        return NULL;
    }
    if (fread(file_contents, file_size, 1, fp) < 1)
	{
        if (ferror(fp))
		{
            fclose(fp);
            free(file_contents);
            return NULL;
        }
    }
    fclose(fp);
    *out_len = file_size;
    return file_contents;
}
