[Back](sdk.md)
---
# Video Output

## VPU overview

The Video Processing Unit (VPU) handles all video output on the Sandpiper platform. It supports multiple video modes (320x240, 640x480, 320x480, 640x240) with both 8-bit indexed color and 16-bit RGB color modes. The VPU uses DMA to stream video data from shared memory to the display hardware.

The VPU supports double buffering for smooth animation, hardware palette manipulation, and includes a built-in text console system for displaying text with customizable colors.

## Data Structures

### EVideoContext

```c
struct EVideoContext
{
    struct SPPlatform *m_platform;
    uint8_t* m_characterBuffer;
    uint8_t* m_colorBuffer;
    enum EVideoMode m_vmode;
    enum EColorMode m_cmode;
    enum EVideoScanoutEnable m_scanEnable;
    enum EVideoScanlineDoubling m_scanlineDoubling;
    uint32_t m_strideInWords;
    uint32_t m_scanoutAddressCacheAligned;
    uint32_t m_cpuWriteAddressCacheAligned;
    uint32_t m_graphicsWidth, m_graphicsHeight;
    uint16_t m_consoleWidth, m_consoleHeight;
    uint16_t m_cursorX, m_cursorY;
    uint16_t m_consoleUpdated;
    uint16_t m_caretX, m_caretY;
    uint8_t m_consoleColor;
    uint8_t m_caretBlink;
    uint8_t m_caretType;
};
```

Holds the video context state including the platform reference, video mode settings, framebuffer addresses, and console state.

### EVideoSwapContext

```c
struct EVideoSwapContext
{
    uint32_t cycle;
    uint8_t *readpage;
    uint8_t *writepage;
    struct SPSizeAlloc *framebufferA;
    struct SPSizeAlloc *framebufferB;
};
```

Manages double buffering by tracking the current swap cycle and the read/write page pointers.

### EVideoMode

```c
enum EVideoMode
{
    EVM_320_240,    // 320x240 pixels
    EVM_640_480,    // 640x480 pixels
    EVM_320_480,    // 320x480 pixels
    EVM_640_240,    // 640x240 pixels
    EVM_Count
};
```

Defines the supported video resolutions.

### EColorMode

```c
enum EColorMode
{
    ECM_8bit_Indexed,   // 8-bit indexed color (256 colors from palette)
    ECM_16bit_RGB,      // 16-bit direct RGB (R5G6B5)
    ECM_Count
};
```

Defines the supported color modes.

### EVideoScanoutEnable

```c
enum EVideoScanoutEnable
{
    EVS_Disable,    // Disable video scanout
    EVS_Enable,     // Enable video scanout
    EVS_Count
};
```

Controls whether video output is enabled.

## Color Macros

---
<span style="color:#00F0D0;">MAKECOLORRGB24(_r, _g, _b)</span><br>
Creates a 24-bit RGB color value for setting palette entries. Parameters are 8-bit values (0-255).

---
<span style="color:#00F0D0;">MAKECOLORRGB16(_r, _g, _b)</span><br>
Creates a 16-bit RGB color value (R5G6B5) for direct color mode. Red and blue are 5-bit (0-31), green is 6-bit (0-63).

## API Documentation

### Initialization / Shutdown

---
<span style="color:#00F0D0;">void VPUInitVideo(struct EVideoContext* _context, struct SPPlatform* _platform);</span>

Initializes the video context. Allocates memory for the character and color buffers used by the text console, and sets the default VGA color palette. This must be called before using any other VPU functions.

---
<span style="color:#00F0D0;">void VPUShutdownVideo(struct EVideoContext* _context);</span>

Shuts down the video context by freeing allocated resources (character and color buffers).

### Video Mode Configuration

---
<span style="color:#00F0D0;">void VPUSetVideoMode(struct EVideoContext *_context, const enum EVideoMode _mode, const enum EColorMode _cmode, const enum EVideoScanoutEnable _scanEnable);</span>

Configures the video mode, color mode, and scanout enable settings. Updates the context's state with the new settings including stride, dimensions, and console size. If `_context` is NULL, it directly writes to hardware without preserving state (useful during shutdown).

---
<span style="color:#00F0D0;">void VPUGetDimensions(const enum EVideoMode _mode, uint32_t *_width, uint32_t *_height);</span>

Retrieves the width and height dimensions in pixels for a given video mode.

---
<span style="color:#00F0D0;">uint32_t VPUGetStride(const enum EVideoMode _mode, const enum EColorMode _cmode);</span>

Calculates the stride (number of bytes per row) for a given video mode and color mode. For some modes, the stride is wider than visible screen area, for example the 320x240x8bpp mode has a stride of 384 bytes. This allows the hardware to bring in an extra 64 pixels of image by utilizing the scroll functions listed below.

### Framebuffer Control

---
<span style="color:#00F0D0;">void VPUSetScanoutAddress(struct EVideoContext *_context, const uint32_t _scanOutAddress64ByteAligned);</span>

Sets the primary scanout address for the VPU. The address must be 64-byte aligned and should be a physical address obtained from `SPAllocateBuffer()`.

---
<span style="color:#00F0D0;">void VPUSetScanoutAddress2(struct EVideoContext *_context, const uint32_t _scanOutAddress64ByteAligned);</span>

Sets the secondary scanout address for sync-swap double buffering. The address must be 64-byte aligned.

---
<span style="color:#00F0D0;">void VPUSetWriteAddress(struct EVideoContext *_context, const uint32_t _cpuWriteAddress64ByteAligned);</span>

Sets the CPU write address for the framebuffer. This is the address where the CPU will write pixel data. Must be 64-byte aligned.

---
<span style="color:#00F0D0;">void VPUClear(struct EVideoContext *_context, const uint32_t _colorWord);</span>

Clears the current CPU write page with the specified color. The `_colorWord` is a 32-bit value containing a 4-pixel wide color pattern.

---
<span style="color:#00F0D0;">void VPUSwapPages(struct EVideoContext* _context, struct EVideoSwapContext *_sc);</span>

Swaps the read and write pages for double buffering on the CPU side. Updates the swap context with new read/write pointers and increments the cycle counter.

### Synchronization

---
<span style="color:#00F0D0;">void VPUSyncSwap(struct EVideoContext *_context, uint8_t _donotwaitforvsync);</span>

Initiates a swap between the two video pages set up by `VPUSetScanoutAddress` and `VPUSetScanoutAddress2`. If `_donotwaitforvsync` is non-zero, the swap will not wait for vertical sync. The swap is not immediate and is queued to be executed on the video hardware on first chance it gets. CPU side can wait for this event using the wait functions listed below (please see the vsync sample for an example)

---
<span style="color:#00F0D0;">void VPUWaitVSync(struct EVideoContext *_context);</span>

Blocks until the VPU's VBlank counter flips, indicating a new frame has started. Note: This is not a precise way to time for vsync due to polling frequency and system load. The recommended approach is to use `VPUSyncSwap`/`VPUNoop` pair and wait for the noop (i.e. command barrier) on the CPU instead.

---
<span style="color:#00F0D0;">uint32_t VPUReadVBlankCounter(struct EVideoContext *_context);</span>

Reads the current value of the vertical blanking counter. The counter alternates between 0 and 1 for each vertical blanking event.

---
<span style="color:#00F0D0;">uint32_t VPUGetScanline(struct EVideoContext *_context);</span>

Returns the current scanline being drawn by the VPU. Valid scanline values range from 0 to 524. Note that timing may not be perfectly synchronized with the VPU due to memory bus delays, and is only for diagnosis or simple first-time sync use.

---
<span style="color:#00F0D0;">void VPUNoop(struct EVideoContext *_context);</span>

Sends a no-operation command to the VPU. Can be used in combination with `VPUGetFIFONotEmpty()` to implement barriers/sync points.

---
<span style="color:#00F0D0;">uint32_t VPUGetFIFONotEmpty(struct EVideoContext *_context);</span>

Checks if the VPU's command FIFO is not empty. Returns 1 if there are commands pending, 0 otherwise. Can be used to wait for a noop at the end of a command stream as a sync point.

### Display Effects

---
<span style="color:#00F0D0;">void VPUShiftCache(struct EVideoContext *_context, uint8_t _offset);</span>

Shifts the scanline cache write address by the specified offset in bytes. Can be used to adjust the write position within the cache for effects like scrolling. The cache writes happen at the end of each scanline, in bursts of 128 bytes, and are written onto a scanline cache large enough to hold 2048 bytes of information.

---
<span style="color:#00F0D0;">void VPUShiftScanout(struct EVideoContext *_context, uint8_t _offset);</span>

Shifts the scanline cache read address by the specified offset in bytes. Can be used to implement panning or other display effects, especially in video modes where buffer stride is wider than displayed screen area.

---
<span style="color:#00F0D0;">void VPUShiftPixel(struct EVideoContext *_context, uint8_t _offset);</span>

Shifts the pixel cache address by the specified offset in pixels. Range is 0 to 7 pixels. Used for fine-grained control over video output offset.

### Control Register

---
<span style="color:#00F0D0;">void VPUWriteControlRegister(struct EVideoContext *_context, uint8_t _setFlag, uint8_t _value);</span>

Writes a value to the VPU's control register. `_setFlag` determines whether to set (1) or clear (0) the control register bits. `_value` is the value to write or clear. (P.S. The control registers are not utilized by hardware at this point in time, and will be used to switch control of scanline address control or other aspects to the VCP unit)

---
<span style="color:#00F0D0;">uint8_t VPUReadControlRegister(struct EVideoContext *_context);</span>

Reads the current value of the VPU's control register. (Please see the note above)

### Palette Control

---
<span style="color:#00F0D0;">void VPUSetDefaultPalette(struct EVideoContext *_context);</span>

Sets the default VGA color palette by writing all 256 color entries to the hardware.

---
<span style="color:#00F0D0;">void VPUSetPal(struct EVideoContext *_context, const uint8_t _paletteIndex, const uint32_t _red, const uint32_t _green, const uint32_t _blue);</span>

Sets a single palette entry. `_paletteIndex` specifies the palette slot (0-255). `_red`, `_green`, and `_blue` specify the color components (0-255).

### Text Rendering

---
<span style="color:#00F0D0;">void VPUPrintString(struct EVideoContext *_context, const uint8_t _foregroundIndex, const uint8_t _backgroundIndex, const uint16_t _x, const uint16_t _y, const char *_message, int _length);</span>

Renders a string of text directly onto the framebuffer at the specified pixel position with the given foreground and background palette colors. The X position is aligned to 4 pixels. Useful for debug purposes.

### Console Functions

The VPU includes a built-in text console system with character and color buffers. The console uses an 8x8 pixel built-in font.

---
<span style="color:#00F0D0;">void VPUConsoleClear(struct EVideoContext *_context);</span>

Clears the console screen by filling it with spaces and the current background color. Resets the cursor position to the top-left corner.

---
<span style="color:#00F0D0;">void VPUConsolePrint(struct EVideoContext *_context, const char *_message, int _length);</span>

Prints a string of text onto the console at the current cursor position, advancing the cursor as characters are rendered. Respects newline (`\n`), tab (`\t`), and carriage return (`\r`) characters. Automatically scrolls when reaching the bottom of the screen.

---
<span style="color:#00F0D0;">void VPUConsolePrintInPlace(struct EVideoContext *_context, const char *_message, int _length);</span>

Prints a string of text at the current cursor position without advancing the cursor. Does not scroll when reaching the bottom of the screen.

---
<span style="color:#00F0D0;">void VPUConsoleResolve(struct EVideoContext *_context);</span>

Resolves the console's character and color buffers into the current CPU write page, rendering all visible characters with their respective colors. Also renders the blinking caret if visible.

---
<span style="color:#00F0D0;">void VPUConsoleScrollUp(struct EVideoContext *_context);</span>

Scrolls the console content up by one row. Discards the top row and clears the bottom row with spaces and the default background color.

---
<span style="color:#00F0D0;">void VPUConsoleScrollDown(struct EVideoContext *_context);</span>

Scrolls the console content down by one row. Discards the bottom row and clears the top row with spaces and the default background color.

---
<span style="color:#00F0D0;">void VPUConsoleSetColors(struct EVideoContext *_context, const uint8_t _foregroundIndex, const uint8_t _backgroundIndex);</span>

Sets both the foreground and background colors for subsequent console text output. Values are palette indices (0-15 for standard console colors).

---
<span style="color:#00F0D0;">void VPUConsoleSetForeground(struct EVideoContext *_context, const uint8_t _foregroundIndex);</span>

Sets only the foreground color for subsequent console text output.

---
<span style="color:#00F0D0;">void VPUConsoleSetBackground(struct EVideoContext *_context, const uint8_t _backgroundIndex);</span>

Sets only the background color for subsequent console text output.

---
<span style="color:#00F0D0;">void VPUConsoleMoveCursor(struct EVideoContext *_context, int dx, int dy);</span>

Moves the console cursor by the specified horizontal and vertical offsets. The cursor position is clamped within the console boundaries. Scrolls if necessary.

---
<span style="color:#00F0D0;">void VPUConsoleHomeCursor(struct EVideoContext *_context);</span>

Moves the console cursor to the beginning of the current line.

---
<span style="color:#00F0D0;">void VPUConsoleEndCursor(struct EVideoContext *_context);</span>

Moves the console cursor to the end of the current line, just after the last non-space character.

---
<span style="color:#00F0D0;">void VPUConsoleSetCursor(struct EVideoContext *_context, uint16_t _x, uint16_t _y);</span>

Sets the console cursor to the specified position. The position is clamped within the console boundaries. Also updates the caret position.

---
<span style="color:#00F0D0;">void VPUConsoleCopyLine(struct EVideoContext *_context, uint16_t _line, uint16_t _xStart, uint16_t _xEnd, char *_buffer);</span>

Copies a line of text from the console's character buffer into a provided buffer. If `_line` is `VPU_AUTO`, the current cursor line is used. The copied text is null-terminated.

---
<span style="color:#00F0D0;">int VPUConsoleFillLine(struct EVideoContext *_context, const char _character);</span>

Fills the current line from the cursor position to the end with the specified character. Advances the cursor to the beginning of the next line, scrolling if necessary. Returns the number of characters filled.

---
<span style="color:#00F0D0;">void VPUInsertCharacter(struct EVideoContext *_context, uint16_t _line, uint16_t _column, uint8_t _character);</span>

Inserts a character at the specified position, shifting existing characters to the right.

---
<span style="color:#00F0D0;">void VPURemoveCharacter(struct EVideoContext *_context, uint16_t _line, uint16_t _column);</span>

Removes the character at the specified position, shifting subsequent characters to the left.

## Console Color Constants

```c
// Dim colors (0x00-0x07)
CONSOLEDIMGRAY, CONSOLEDIMBLUE, CONSOLEDIMGREEN, CONSOLEDIMCYAN,
CONSOLEDIMRED, CONSOLEDIMMAGENTA, CONSOLEDIMYELLOW, CONSOLEDIMWHITE

// Bright colors (0x08-0x0F)
CONSOLEGRAY, CONSOLEBLUE, CONSOLEGREEN, CONSOLECYAN,
CONSOLERED, CONSOLEMAGENTA, CONSOLEYELLOW, CONSOLEWHITE

// Defaults
CONSOLEDEFAULTFG = CONSOLEWHITE
CONSOLEDEFAULTBG = CONSOLEDIMGRAY
```

## Example Usage

```c
// Initialize platform and video
struct SPPlatform* platform = SPInitPlatform();
struct EVideoContext videoCtx;
VPUInitVideo(&videoCtx, platform);

// Allocate framebuffers (must be 64-byte aligned)
struct SPSizeAlloc framebufferA, framebufferB;
framebufferA.size = 640 * 480;  // For 8-bit indexed mode
framebufferB.size = 640 * 480;
SPAllocateBuffer(platform, &framebufferA);
SPAllocateBuffer(platform, &framebufferB);

// Set up swap context for double buffering
struct EVideoSwapContext swapCtx;
swapCtx.cycle = 0;
swapCtx.framebufferA = &framebufferA;
swapCtx.framebufferB = &framebufferB;

// Configure video mode
VPUSetVideoMode(&videoCtx, EVM_640_480, ECM_8bit_Indexed, EVS_Enable);
VPUSetScanoutAddress(&videoCtx, (uint32_t)framebufferA.dmaAddress);
VPUSetScanoutAddress2(&videoCtx, (uint32_t)framebufferB.dmaAddress);
VPUSetWriteAddress(&videoCtx, (uint32_t)framebufferA.cpuAddress);

// Clear screen with black
VPUClear(&videoCtx, 0x00000000);

// Set a custom palette color
VPUSetPal(&videoCtx, 1, 255, 0, 0);  // Palette index 1 = red

// Use the console
VPUConsoleSetColors(&videoCtx, CONSOLEWHITE, CONSOLEDIMBLUE);
VPUConsoleClear(&videoCtx);
VPUConsolePrint(&videoCtx, "Hello, Sandpiper!\n", 18);
VPUConsoleResolve(&videoCtx);

// Main loop with double buffering
while (running) {
    // Draw to write page
    // ...
    
    // Swap buffers with vsync
    VPUSyncSwap(&videoCtx, 0);
    VPUSwapPages(&videoCtx, &swapCtx);
}

// Cleanup
VPUShutdownVideo(&videoCtx);
SPFreeBuffer(platform, &framebufferA);
SPFreeBuffer(platform, &framebufferB);
SPShutdownPlatform(platform);
```
