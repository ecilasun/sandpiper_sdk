#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include <pty.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include "core.h"
#include "platform.h"
#include "vpu.h"

#define VIDEO_MODE      EVM_640_Wide
#define VIDEO_COLOR     ECM_8bit_Indexed
#define VIDEO_HEIGHT    480

typedef enum {
    CSI_STATE_NORMAL,
    CSI_STATE_ESCAPE,
    CSI_STATE_CSI,
    CSI_STATE_OSC
} CSIState;

static struct EVideoContext s_vctx;
static struct EVideoSwapContext s_sctx;
struct SPSizeAlloc frameBuffer;
static struct SPPlatform s_platform;

static int32_t masterfd = 0;
static char buf[1024];
static int32_t buflen = 0;
static fd_set fdset;

static struct termios orig_termios;
static int stdin_flags;

static CSIState csi_state = CSI_STATE_NORMAL;
static char csi_buffer[256];
static int csi_buffer_len = 0;

void setupKeyboardInput()
{
	// Save original terminal settings
	tcgetattr(STDIN_FILENO, &orig_termios);
	
	// Set terminal to raw mode for immediate character input
	struct termios raw = orig_termios;
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_cflag &= ~(CSIZE | PARENB);
	raw.c_cflag |= CS8;
	raw.c_oflag &= ~(OPOST);
	raw.c_cc[VMIN] = 0;  // Non-blocking read
	raw.c_cc[VTIME] = 0;
	
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	
	// Set stdin to non-blocking
	stdin_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, stdin_flags | O_NONBLOCK);
}

void restoreKeyboardInput()
{
	// Restore original terminal settings
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
	fcntl(STDIN_FILENO, F_SETFL, stdin_flags);
}

void shutdowncleanup()
{
	// Reset shifts
	VPUShiftCache(&s_vctx, 0);
	VPUShiftScanout(&s_vctx, 0);
	VPUShiftPixel(&s_vctx, 0);

	// Switch to fbcon buffer
	VPUSetScanoutAddress(&s_vctx, 0x18000000);
	VPUSetVideoMode(&s_vctx, EVM_640_Wide, ECM_16bit_RGB, EVS_Enable);

	// Yield physical memory and reset video routines
	VPUShutdownVideo();

	// Shutdown platform
	SPShutdownPlatform(&s_platform);
}

int getKeyboardInput(char *input_char)
{
	int result = read(STDIN_FILENO, input_char, 1);
	return (result == 1);
}

void sigint_handler(int /*s*/)
{
	shutdowncleanup();
	restoreKeyboardInput();
	exit(0);
}

void handleCSISequence(const char* seq, int len)
{
	if (len < 1) return;

	char final_char = seq[len - 1];

	// Parse parameters (numbers separated by semicolons)
	int params[16];
	int param_count = 0;
	int current_param = 0;
	int has_param = 0;

	for (int i = 0; i < len - 1; i++) {
		char c = seq[i];
		if (isdigit(c)) {
			current_param = current_param * 10 + (c - '0');
			has_param = 1;
		} else if (c == ';') {
			if (param_count < 16) {
				params[param_count++] = has_param ? current_param : 0;
			}
			current_param = 0;
			has_param = 0;
		}
		// Skip other intermediate characters
	}

	// Add the last parameter
	if (param_count < 16) {
		params[param_count++] = has_param ? current_param : 0;
	}

	// Default parameters to 1 if not specified for certain commands
	if (param_count == 0) {
		params[0] = 1;
		param_count = 1;
	}

	switch (final_char) {
		case 'A': // Cursor Up
			{
				int lines = (param_count > 0 && params[0] > 0) ? params[0] : 1;
				for (int i = 0; i < lines; i++) {
					VPUConsoleMoveCursor(&s_vctx, 0, -1);
				}
			}
			break;

		case 'B': // Cursor Down
			{
				int lines = (param_count > 0 && params[0] > 0) ? params[0] : 1;
				for (int i = 0; i < lines; i++) {
					VPUConsoleMoveCursor(&s_vctx, 0, 1);
				}
			}
			break;

		case 'C': // Cursor Forward (Right)
			{
				int cols = (param_count > 0 && params[0] > 0) ? params[0] : 1;
				for (int i = 0; i < cols; i++) {
					VPUConsoleMoveCursor(&s_vctx, 1, 0);
				}
			}
			break;

		case 'D': // Cursor Backward (Left)
			{
				int cols = (param_count > 0 && params[0] > 0) ? params[0] : 1;
				for (int i = 0; i < cols; i++) {
					VPUConsoleMoveCursor(&s_vctx, -1, 0);
				}
			}
			break;

		case 'H': // Cursor Position
		case 'f': // Horizontal and Vertical Position
			{
				int row = (param_count > 0 && params[0] > 0) ? params[0] - 1 : 0;
				int col = (param_count > 1 && params[1] > 0) ? params[1] - 1 : 0;
				VPUConsoleSetCursor(&s_vctx, col, row);
			}
			break;

		case 'J': // Erase in Display
			{
				int mode = (param_count > 0) ? params[0] : 0;
				switch (mode) {
					case 0: // Clear from cursor to end of screen
						//VPUConsoleClearToEndOfScreen(&s_vctx);
						break;
					case 1: // Clear from cursor to beginning of screen
						//VPUConsoleClearToBeginningOfScreen(&s_vctx);
						break;
					case 2: // Clear entire screen
					case 3: // Clear entire screen and scrollback (treat same as 2)
						VPUConsoleClear(&s_vctx);
						break;
				}
			}
			break;

		case 'K': // Erase in Line
			{
				int mode = (param_count > 0) ? params[0] : 0;
				switch (mode) {
					case 0: // Clear from cursor to end of line
						//VPUConsoleClearToEndOfLine(&s_vctx);
						break;
					case 1: // Clear from cursor to beginning of line
						//VPUConsoleClearToBeginningOfLine(&s_vctx);
						break;
					case 2: // Clear entire line
						//VPUConsoleClearLine(&s_vctx);
						break;
				}
			}
			break;

		case 'm': // Select Graphic Rendition (SGR) - Colors and styles
			{
				for (int i = 0; i < param_count; i++) {
					int param = params[i];
					if (param == 0) { // Reset
						VPUConsoleSetColors(&s_vctx, CONSOLEDEFAULTFG, CONSOLEDEFAULTBG);
					} else if (param >= 30 && param <= 37) { // Foreground colors
						uint8_t color = param - 30;
						VPUConsoleSetForeground(&s_vctx, color);
					} else if (param >= 40 && param <= 47) { // Background colors
						uint8_t color = param - 40;
						VPUConsoleSetBackground(&s_vctx, color);
					} else if (param >= 90 && param <= 97) { // Bright foreground colors
						uint8_t color = (param - 90) + 8;
						VPUConsoleSetForeground(&s_vctx, color);
					} else if (param >= 100 && param <= 107) { // Bright background colors
						uint8_t color = (param - 100) + 8;
						VPUConsoleSetBackground(&s_vctx, color);
					}
					// Add more SGR parameters as needed (bold, italic, etc.)
				}
			}
			break;

		case 'S': // Scroll Up
			{
				int lines = (param_count > 0 && params[0] > 0) ? params[0] : 1;
				for (int i = 0; i < lines; i++) {
					VPUConsoleScrollUp(&s_vctx);
				}
			}
			break;

		case 'T': // Scroll Down
			{
				int lines = (param_count > 0 && params[0] > 0) ? params[0] : 1;
				for (int i = 0; i < lines; i++) {
					VPUConsoleScrollDown(&s_vctx);
				}
			}
			break;

		case 'L': // Insert Lines
			{
				int lines = (param_count > 0 && params[0] > 0) ? params[0] : 1;
				for (int i = 0; i < lines; i++) {
					//VPUConsoleInsertLine(&s_vctx);
				}
			}
			break;

		case 'M': // Delete Lines
			{
				int lines = (param_count > 0 && params[0] > 0) ? params[0] : 1;
				for (int i = 0; i < lines; i++) {
					//VPUConsoleDeleteLine(&s_vctx);
				}
			}
			break;

		case '@': // Insert Characters
			{
				int chars = (param_count > 0 && params[0] > 0) ? params[0] : 1;
				for (int i = 0; i < chars; i++) {
					//VPUConsoleInsertChar(&s_vctx);
				}
			}
			break;

		case 'P': // Delete Characters
			{
				int chars = (param_count > 0 && params[0] > 0) ? params[0] : 1;
				for (int i = 0; i < chars; i++) {
					//VPUConsoleDeleteChar(&s_vctx);
				}
			}
			break;

		default:
			// Unknown CSI sequence - ignore
			printf("Unknown CSI sequence: ESC[%.*s\n", len, seq);
			break;
	}
}

void processCharacterWithCSI(uint32_t codepoint)
{
	char c = (char)codepoint;

	switch (csi_state) {
		case CSI_STATE_NORMAL:
			if (c == '\033') { // ESC character
				csi_state = CSI_STATE_ESCAPE;
			} else {
				// Normal character - print it
				VPUConsolePrint(&s_vctx, &c, 1);
			}
			break;

		case CSI_STATE_ESCAPE:
			if (c == '[') {
				csi_state = CSI_STATE_CSI;
				csi_buffer_len = 0;
			} else if (c == ']') {
				csi_state = CSI_STATE_OSC;
				csi_buffer_len = 0;
			} else {
				// Not a CSI sequence - handle single ESC commands
				switch (c) {
					case 'c': // Reset terminal
						VPUConsoleClear(&s_vctx);
						VPUConsoleSetColors(&s_vctx, CONSOLEDEFAULTFG, CONSOLEDEFAULTBG);
						VPUConsoleSetCursor(&s_vctx, 0, 0);
						break;
					case 'D': // Line feed
						VPUConsolePrint(&s_vctx, "\n", 1);
						break;
					case 'M': // Reverse line feed
						//VPUConsoleMoveUp(&s_vctx);
						break;
					default:
						// Unknown escape sequence
						break;
				}
				csi_state = CSI_STATE_NORMAL;
			}
			break;

		case CSI_STATE_CSI:
			if (csi_buffer_len < sizeof(csi_buffer) - 1) {
				csi_buffer[csi_buffer_len++] = c;
			}

			// Check if this is the final character of the sequence
			if (c >= 0x40 && c <= 0x7E) {
				csi_buffer[csi_buffer_len] = '\0';
				handleCSISequence(csi_buffer, csi_buffer_len);
				csi_state = CSI_STATE_NORMAL;
			}
			break;

		case CSI_STATE_OSC:
			// Operating System Command - usually terminated by BEL (0x07) or ESC
			if (c == 0x07 || (csi_buffer_len > 0 && csi_buffer[csi_buffer_len-1] == '\033' && c == '\\')) {
				// OSC sequences are typically for setting window title, etc.
				// Most can be ignored for a simple terminal
				csi_state = CSI_STATE_NORMAL;
			} else if (csi_buffer_len < sizeof(csi_buffer) - 1) {
				csi_buffer[csi_buffer_len++] = c;
			}
			break;
	}
}

int32_t utf8decode(const char *s, uint32_t *out_cp) {
	unsigned char c = s[0];
	if (c < 0x80) {
		*out_cp = c;
		return 1;
	} else if ((c>>5) == 0x6) {
		*out_cp=((c & 0x1F) << 6)|(s[1]& 0x3F);
		return 2;
	} else if ((c >> 4) == 0xE) {
		*out_cp=((c &0x0F) << 12)|((s[1]&0x3F) << 6)|(s[2]&0x3F);
		return 3;
	} else if ((c>>3) == 0x1E) {
		*out_cp=((c &0x07) << 18)|((s[1]&0x3F) << 12)|((s[2]&0x3F) << 6)|(s[3]&0x3F);
		return 4;
	}
	return -1; // invalid UTF-8
}

size_t readfrompty()
{
	int32_t nbytes = read(masterfd, buf + buflen, sizeof(buf)-buflen);
	buflen += nbytes;

	int32_t iter = 0;
	while(iter < buflen)
	{
		uint32_t codepoint = 0;
		int32_t len = utf8decode(&buf[iter], &codepoint);
		if (len == -1 || len > buflen)
			break;
        processCharacterWithCSI(codepoint);
		iter += len;
	}

	if (iter < buflen)
	{
		memmove(buf, buf + iter, buflen - iter);
	}

	buflen -= iter;

	return nbytes;
}

void handleKeyboardInput()
{
	// This is pseudocode - you'll need to implement based on your platform
	char input_char;
	if (getKeyboardInput(&input_char)) // You need to implement this
	{
		write(masterfd, &input_char, 1); // Send to the PTY
	}
}

int main(int /*argc*/, char** /*argv*/)
{
	SPInitPlatform(&s_platform);

	VPUInitVideo(&s_vctx, &s_platform);

	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);

	// Grab memory address reserved for console framebuffer
	frameBuffer.size = stride*VIDEO_HEIGHT;
	SPGetConsoleFramebuffer(&s_platform, &frameBuffer);

	// To handle key input
	setupKeyboardInput();

	// In case something happens to us
	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);
	signal(SIGTERM, &sigint_handler);

	// Point at shared console memory
	VPUSetWriteAddress(&s_vctx, (uint32_t)frameBuffer.cpuAddress);
	VPUSetScanoutAddress(&s_vctx, (uint32_t)frameBuffer.dmaAddress);
	VPUSetDefaultPalette(&s_vctx);
	VPUSetVideoMode(&s_vctx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	// Reset and start console
	VPUConsoleSetColors(&s_vctx, CONSOLEDEFAULTFG, CONSOLEDEFAULTBG);
	VPUConsoleClear(&s_vctx);

	s_sctx.cycle = 0;
	s_sctx.framebufferA = &frameBuffer;
	s_sctx.framebufferB = &frameBuffer; // Single buffered

	s_vctx.m_caretX = s_vctx.m_cursorX;
	s_vctx.m_caretY = s_vctx.m_cursorY;
	s_vctx.m_caretBlink = 0;
	s_vctx.m_caretType = 0;

	// Here we fork a terminal
	if (forkpty(&masterfd, NULL, NULL, NULL) == 0)
	{
		// Child process
		setenv("TERM", "linux", 1);  // Set terminal type (xterm for color escape codes etc)
		setenv("COLUMNS", "80", 1);  // Set terminal width (640/8)
		setenv("LINES", "60", 1);    // Set terminal height (480/8)
		execlp("/bin/bash", "bash", NULL);
		perror("execlp");
		exit(1);
	}

	int needUpdate = 0;

	do
	{
		FD_ZERO(&fdset);
		FD_SET(masterfd, &fdset);

		// NOTE: Without the timeout select() hangs
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 10000;  // 10ms timeout
		select(masterfd+1, &fdset, NULL, NULL, &timeout);

		if (FD_ISSET(masterfd, &fdset))
		{
			// Request update if number of bytes read is nonzero
			needUpdate = readfrompty();
		}

		// Send key presses over to masterfd
		handleKeyboardInput();

		// Cursor blinks every quarter of a second
		if (s_sctx.cycle % 15 == 0)
		{
			s_vctx.m_caretBlink ^= 1;
			needUpdate = 1;
		}

		// Resolve and display console contents
		if (needUpdate)
			VPUConsoleResolve(&s_vctx);

		// Vsync is really not needed but nice to have to limit our pacing
		// We could alternatively increase the timeout to 250ms and remove
		// the vsync
		VPUWaitVSync(&s_vctx);
		VPUSwapPages(&s_vctx, &s_sctx);
	} while(1);

	return 0;
}
