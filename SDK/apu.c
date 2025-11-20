#include "core.h"
#include "apu.h"

// Command codes for APU control
#define APUCMD_BUFFERSIZE   0x00000000
#define APUCMD_START        0x00000001
#define APUCMD_NOOP         0x00000002
#define APUCMD_SWAPCHANNELS 0x00000003
#define APUCMD_SETRATE      0x00000004

// Audio Processing Unit (APU) functions

/*
 * Set the audio buffer size.
 * Parameters:
 *   _context - Pointer to the audio context.
 *   _bufferSize - Desired buffer size from EAPUBufferSize enum.
 */
void APUSetBufferSize(struct EAudioContext* _context, enum EAPUBufferSize _bufferSize)
{
	_context->m_bufferSize = 128 << (uint32_t)_bufferSize;

	audiowrite32(_context->m_platform, 0, APUCMD_BUFFERSIZE);
	audiowrite32(_context->m_platform, 0, (uint32_t)_bufferSize);
}

/*
 * Start audio DMA with the specified buffer address.
 * Parameters:
 *   _context - Pointer to the audio context.
 *   _audioBufferAddress16byteAligned - 16-byte aligned address of the audio buffer.
 */
void APUStartDMA(struct EAudioContext* _context, uint32_t _audioBufferAddress16byteAligned)
{
	audiowrite32(_context->m_platform, 0, APUCMD_START);
	audiowrite32(_context->m_platform, 0, _audioBufferAddress16byteAligned);
}

/*
 * Set the audio sample rate.
 * Parameters:
 *   _context - Pointer to the audio context.
 *   _sampleRate - Desired sample rate from EAPUSampleRate enum.
 */
void APUSetSampleRate(struct EAudioContext* _context, enum EAPUSampleRate _sampleRate)
{
	_context->m_sampleRate = _sampleRate;

	audiowrite32(_context->m_platform, 0, APUCMD_SETRATE);
	audiowrite32(_context->m_platform, 0, (uint32_t)_sampleRate);
}

/*
 * Swap audio channels.
 * Parameters:
 *   _context - Pointer to the audio context.
 *   _swap - Non-zero to swap channels, zero to keep original order.
 */
void APUSwapChannels(struct EAudioContext* _context, uint32_t _swap)
{
	audiowrite32(_context->m_platform, 0, APUCMD_SWAPCHANNELS);
	audiowrite32(_context->m_platform, 0, _swap);
}

/*
 * Synchronize the APU.
 * Parameters:
 *   _context - Pointer to the audio context.
 */
void APUSync(struct EAudioContext* _context)
{
	// Dummy command
	audiowrite32(_context->m_platform, 0, APUCMD_NOOP);
}

/*
 * Get the current APU frame status.
 * Parameters:
 *   _context - Pointer to the audio context.
 * Returns:
 *   Current frame status (0 or 1).
 */
uint32_t APUFrame(struct EAudioContext* _context)
{
	return audioread32(_context->m_platform, 0) & 0x1;
}

/*
 * Get the current word count in the APU buffer.
 * Parameters:
 *   _context - Pointer to the audio context.
 * Returns:
 *   Number of words currently in the buffer.
 */
uint32_t APUGetWordCount(struct EAudioContext* _context)
{
	uint32_t status = audioread32(_context->m_platform, 0);
	return (status>>1)&0x3FF;
}

/*
 * Initialize the audio context.
 * Parameters:
 *   _context - Pointer to the audio context to initialize.
 *   _platform - Pointer to the platform structure.
 * Returns:
 *   0 on success.
 */
int APUInitAudio(struct EAudioContext* _context, struct SPPlatform* _platform)
{
	_context->m_platform = _platform;
	_context->m_sampleRate = ASR_Halt;
	_context->m_bufferSize = 0;

	// NOTE: No failure conditions yet
	return 0;
}

/*
 * Shutdown the audio context.
 * Parameters:
 *   _context - Pointer to the audio context to shutdown.
 */
void APUShutdownAudio(struct EAudioContext* _context)
{
	if (_context->m_sampleRate != ASR_Halt)
	{
		// In case the user forgot to stop audio
		APUSetSampleRate(_context, ASR_Halt);
	}
}

/*
 * Wait for the next APU frame.
 * Parameters:
 *   _context - Pointer to the audio context.
 * Each time APU swaps buffers, the frame toggles between 0 and 1.
 * This function blocks until the next frame is reached.
 */
void APUWaitSync(struct EAudioContext *_context)
{
	volatile uint32_t prevsync = APUFrame(_context);
	volatile uint32_t currentsync;
	do {
		currentsync = APUFrame(_context);
	} while (currentsync == prevsync);
}
