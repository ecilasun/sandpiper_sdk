#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <termios.h>
#include <linux/input.h>

#include "agnes.h"

#include "core.h"
#include "platform.h"
#include "vpu.h"
#include "vcp.h"
#include "apu.h"

static agnes_input_t s_input;
static agnes_t *s_agnes;
static bool s_alive = true;

static int havekeyboard = 1;
static struct pollfd fds[1];

static struct termios orig_termios;

static void restore_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
	agnes_destroy(s_agnes);
	printf("Emulator terminated.\n");
}

#define VIDEO_MODE		EVM_320_240
#define VIDEO_COLOR		ECM_8bit_Indexed
#define VIDEO_HEIGHT	240

// Audio configuration
// NES APU generates audio at 22050 Hz sample rate
// Buffer size for 1 frame at 60fps = 22050/60 = ~368 samples
// Use 512 samples (2048 bytes) buffer - close to frame output
#define AUDIO_SAMPLE_RATE 22050.0
#define AUDIO_BUFFER_SAMPLES 512
#define AUDIO_BUFFER_BYTES (AUDIO_BUFFER_SAMPLES * sizeof(short) * 2)  // Stereo 16-bit

// Set to 1 to generate a test tone instead of NES audio (for debugging)
#define AUDIO_TEST_TONE 0

static struct SPPlatform* s_platform = NULL;
struct SPSizeAlloc frameBufferA;
struct SPSizeAlloc frameBufferB;
struct SPSizeAlloc audioBuffer;

// Float buffer for accumulating NES audio samples
static float s_nesAudioBuffer[AUDIO_BUFFER_SAMPLES * 2];  // Extra space for accumulation

// Audio accumulation buffer for continuous playback
static float s_audioAccumBuffer[AUDIO_BUFFER_SAMPLES * 4];
static uint32_t s_audioAccumIndex = 0;

static void* read_file(const char *filename, size_t *out_len);

int main(int argc, char** argv)
{
	if (argc != 2) {
        fprintf(stderr, "Usage: %s filename.nes\n", argv[0]);
		fprintf(stderr, "Keyboard controls:\n");
		fprintf(stderr, "  A:      P\n");
		fprintf(stderr, "  B:      O\n");
		fprintf(stderr, "  Select: Esc\n");
		fprintf(stderr, "  Start:  1\n");
		fprintf(stderr, "  Up:     W\n");
		fprintf(stderr, "  Down:   S\n");
		fprintf(stderr, "  Left:   A\n");
		fprintf(stderr, "  Right:  D\n");
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

	// Open keyboard device
	fds[0].fd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
	fds[0].events = POLLIN;

	if (fds[0].fd < 0)
	{
		perror("/dev/input/event0: make sure a keyboard is connected");
		havekeyboard = 0;
	}
	else
		printf("attached to /dev/input/event for keyboard access\n");

	fprintf(stderr, "Starting emulator\n");

	// Save terminal settings and set raw mode
	struct termios raw_termios;
	tcgetattr(STDIN_FILENO, &orig_termios); // Save current settings
	raw_termios = orig_termios;
	raw_termios.c_lflag &= ~(ECHO | ICANON); // Disable echo and canonical mode
	tcsetattr(STDIN_FILENO, TCSANOW, &raw_termios);
	atexit(restore_terminal);

	s_platform = SPInitPlatform();
	VPUSetVideoMode(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	frameBufferB.size = frameBufferA.size = stride*VIDEO_HEIGHT;
	SPAllocateBuffer(s_platform, &frameBufferA);
	SPAllocateBuffer(s_platform, &frameBufferB);

	// Initialize audio
	audioBuffer.size = AUDIO_BUFFER_BYTES;
	SPAllocateBuffer(s_platform, &audioBuffer);
	memset(audioBuffer.cpuAddress, 0, AUDIO_BUFFER_BYTES);

	// Configure the platform APU
	APUSetBufferSize(s_platform->ac, ABS_2048Bytes);  // 512 stereo samples
	APUSetSampleRate(s_platform->ac, ASR_22_050_Hz);

	// Configure NES APU audio output - use larger buffer for frame samples
	agnes_set_audio_sample_rate(s_agnes, AUDIO_SAMPLE_RATE);
	agnes_set_audio_buffer(s_agnes, s_nesAudioBuffer, AUDIO_BUFFER_SAMPLES * 2);

	fprintf(stderr, "Audio initialized at %.0f Hz\n", AUDIO_SAMPLE_RATE);

	for (int y = 0; y < VIDEO_HEIGHT; y++)
	{
		for (int x = 0; x < stride/4; x++)
		{
			uint32_t* pixelA = (uint32_t*)frameBufferA.cpuAddress + (y * stride/4) + x;
			*pixelA = 0x0D0D0D0D;
			uint32_t* pixelB = (uint32_t*)frameBufferB.cpuAddress + (y * stride/4) + x;
			*pixelB = 0x0D0D0D0D;
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
		if (havekeyboard)
		{
			int ret = poll(fds, 1, 0);
			if (ret > 0)
			{
				struct input_event ev;
				int n = read(fds[0].fd, &ev, sizeof(struct input_event));
				if (n > 0 && ev.type == EV_KEY)
				{
					switch (ev.code)
					{
						// Take action based on key code
						// Down(1) and autorepeat(2) both set the button, up event(0) clears it
						case KEY_W:		{ s_input.up = ev.value == 0 ? 0 : 1; break; }
						case KEY_A:		{ s_input.left = ev.value == 0 ? 0 : 1; break; }
						case KEY_S:		{ s_input.down = ev.value == 0 ? 0 : 1; break; }
						case KEY_D:		{ s_input.right = ev.value == 0 ? 0 : 1; break; }
						case KEY_ESC:	{ s_input.select = ev.value == 0 ? 0 : 1; break; }
						case KEY_1:		{ s_input.start = ev.value == 0 ? 0 : 1; break; }
						case KEY_P:		{ s_input.a = ev.value == 0 ? 0 : 1; break; }
						case KEY_O:   	{ s_input.b = ev.value == 0 ? 0 : 1; break; }
						default:    break;
					}
				}
			}
		}

		agnes_set_input(s_agnes, &s_input, NULL);
		s_alive = agnes_next_frame(s_agnes);

		// Process audio: accumulate samples and send when buffer is full
		short* audioDest = (short*)audioBuffer.cpuAddress;
		
#if AUDIO_TEST_TONE
		// Generate a simple 440 Hz test tone
		static uint32_t testPhase = 0;
		for (uint32_t i = 0; i < AUDIO_BUFFER_SAMPLES; i++)
		{
			// 440 Hz square wave at 22050 Hz sample rate
			// Period = 22050/440 = ~50 samples
			int16_t sample = (testPhase % 50 < 25) ? 8000 : -8000;
			audioDest[i * 2 + 0] = sample;
			audioDest[i * 2 + 1] = sample;
			testPhase++;
		}
		
		// Send audio to APU DMA
		APUStartDMA(s_platform->ac, (uint32_t)audioBuffer.dmaAddress);
		APUWaitSync(s_platform->ac);
#else
		// Get samples generated this frame
		uint32_t samples = agnes_get_audio_samples(s_agnes);
		
		// Accumulate samples from NES APU
		for (uint32_t i = 0; i < samples && s_audioAccumIndex < AUDIO_BUFFER_SAMPLES * 4; i++)
		{
			s_audioAccumBuffer[s_audioAccumIndex++] = s_nesAudioBuffer[i];
		}
		
		// When we have enough samples, send a buffer
		if (s_audioAccumIndex >= AUDIO_BUFFER_SAMPLES)
		{
			// Convert float mono to 16-bit stereo
			// APU already applies high-pass filter for click reduction
			for (uint32_t i = 0; i < AUDIO_BUFFER_SAMPLES; i++)
			{
				int16_t sample = (int16_t)(s_audioAccumBuffer[i] * 28000.0f);
				audioDest[i * 2 + 0] = sample;  // Left channel
				audioDest[i * 2 + 1] = sample;  // Right channel
			}

			// Send audio to APU DMA
			APUStartDMA(s_platform->ac, (uint32_t)audioBuffer.dmaAddress);
			APUWaitSync(s_platform->ac);
			
			// Shift remaining samples to front of accumulator using memmove
			uint32_t remaining = s_audioAccumIndex - AUDIO_BUFFER_SAMPLES;
			if (remaining > 0)
			{
				memmove(s_audioAccumBuffer, &s_audioAccumBuffer[AUDIO_BUFFER_SAMPLES], remaining * sizeof(float));
			}
			s_audioAccumIndex = remaining;
		}
		
		// Reset the NES audio buffer for next frame
		agnes_reset_audio_buffer(s_agnes);
#endif

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
