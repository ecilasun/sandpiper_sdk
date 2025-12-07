[Back](sdk.md)
---
# Audio Output

## APU overview

The Audio Processing Unit (APU) handles audio playback on the Sandpiper platform. It supports stereo 16-bit PCM audio at various sample rates (44.1 KHz, 22.05 KHz, 11.025 KHz) and uses DMA to stream audio data from shared memory to the audio hardware.

The APU works with a double-buffered system where the hardware toggles between two buffer halves. Applications can use `APUFrame()` to determine which buffer is currently being played and `APUWaitSync()` to synchronize audio updates with buffer swaps.

## Data Structures

### EAudioContext

```c
struct EAudioContext
{
    struct SPPlatform *m_platform;
    enum EAPUSampleRate m_sampleRate;
    uint32_t m_bufferSize;
};
```

Holds the audio context state including the platform reference, current sample rate, and buffer size.

### EAPUSampleRate

```c
enum EAPUSampleRate
{
    ASR_44_100_Hz = 0,  // 44.1000 KHz
    ASR_22_050_Hz = 1,  // 22.0500 KHz
    ASR_11_025_Hz = 2,  // 11.0250 KHz
    ASR_Halt = 3,       // Halt audio playback
};
```

Defines the supported audio sample rates.

### EAPUBufferSize

```c
enum EAPUBufferSize
{
    ABS_128Bytes  = 0,  //   32 16bit stereo samples
    ABS_256Bytes  = 1,  //   64 16bit stereo samples
    ABS_512Bytes  = 2,  //  128 16bit stereo samples
    ABS_1024Bytes = 3,  //  256 16bit stereo samples
    ABS_2048Bytes = 4,  //  512 16bit stereo samples
    ABS_4096Bytes = 5,  // 1024 16bit stereo samples
};
```

Defines the supported audio buffer sizes. Each sample consists of two 16-bit values (left and right channels).

## API Documentation

### Initialization / Shutdown

---
<span style="color:#00F0D0;">int APUInitAudio(struct EAudioContext* _context, struct SPPlatform* _platform);</span>

Initializes the audio context. This must be called before using any other APU functions. The sample rate is initially set to `ASR_Halt` (audio stopped).

Returns 0 on success.

---
<span style="color:#00F0D0;">void APUShutdownAudio(struct EAudioContext* _context);</span>

Shuts down the audio context. If audio is still playing, it will be halted automatically.

### Configuration

---
<span style="color:#00F0D0;">void APUSetBufferSize(struct EAudioContext* _context, enum EAPUBufferSize _bufferSize);</span>

Sets the audio buffer size. The buffer size determines how many samples are played before the APU switches to the next buffer half. Larger buffers provide more time to fill the next buffer but introduce more latency.

---
<span style="color:#00F0D0;">void APUSetSampleRate(struct EAudioContext* _context, enum EAPUSampleRate _sampleRate);</span>

Sets the audio sample rate. Use `ASR_Halt` to stop audio playback.

---
<span style="color:#00F0D0;">void APUSwapChannels(struct EAudioContext* _context, uint32_t _swap);</span>

Swaps the left and right audio channels. Pass a non-zero value to swap channels, or zero to keep the original order.

### Playback Control

---
<span style="color:#00F0D0;">void APUStartDMA(struct EAudioContext* _context, uint32_t _audioBufferAddress16byteAligned);</span>

Queues the next set of audio samples to be transferred to the APU's internal buffer. The buffer address must be 16-byte aligned and should be a physical address obtained from `SPAllocateBuffer()`.

### Synchronization

---
<span style="color:#00F0D0;">void APUSync(struct EAudioContext* _context);</span>

Sends a no-operation command to the APU. This can be used to ensure previous commands have been processed, by waiting on the command fifo to become empty (i.e. that this command has been processed as our wait marker)

---
<span style="color:#00F0D0;">uint32_t APUFrame(struct EAudioContext* _context);</span>

Returns the current frame status (0 or 1). This indicates which half of the double buffer is currently being played. Applications should fill the inactive buffer half while the other is being played.

---
<span style="color:#00F0D0;">uint32_t APUGetWordCount(struct EAudioContext* _context);</span>

Returns the number of words currently in the APU buffer. This can be used to monitor buffer fill level.

---
<span style="color:#00F0D0;">void APUWaitSync(struct EAudioContext *_context);</span>

Blocks until the APU switches to the next buffer. This is useful for synchronizing audio data updates with buffer swaps. The function monitors the frame status and returns when it changes from its initial value.

## Example Usage

```c
// Initialize platform and audio
struct SPPlatform* platform = SPInitPlatform();
struct EAudioContext audioCtx;
APUInitAudio(&audioCtx, platform);

// Allocate audio buffer (must be 16-byte aligned)
struct SPSizeAlloc audioBuffer;
audioBuffer.size = 4096;  // Double buffer: 2x 2048 bytes
SPAllocateBuffer(platform, &audioBuffer);

// Configure audio
APUSetBufferSize(&audioCtx, ABS_2048Bytes);
APUSetSampleRate(&audioCtx, ASR_44_100_Hz);

// Start playback
APUStartDMA(&audioCtx, (uint32_t)audioBuffer.dmaAddress);

// Audio loop
while (playing) {
    uint32_t frame = APUFrame(&audioCtx);
    // Fill the inactive buffer half (frame == 0 ? second half : first half)
    int16_t* samples = (int16_t*)(audioBuffer.cpuAddress + (frame ? 0 : 2048));
    // ... fill samples ...
    
    APUWaitSync(&audioCtx);  // Wait for buffer swap
}

// Cleanup
APUShutdownAudio(&audioCtx);
SPFreeBuffer(platform, &audioBuffer);
SPShutdownPlatform(platform);
```
