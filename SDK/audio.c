#include "core.h"
#include "audio.h"

void APUSetBufferSize(struct EAudioContext* _context, enum EAPUBufferSize _bufferSize)
{
	_context->m_bufferSize = 128 << (uint32_t)_bufferSize;

	uint32_t cmd = APUCMD_BUFFERSIZE;
	audiowrite32(&cmd);
	audiowrite32(&_context->m_bufferSize);
}

void APUStartDMA(struct EAudioContext* _context, uint32_t _audioBufferAddress16byteAligned)
{
	uint32_t cmd = APUCMD_START;
	audiowrite32(&cmd);
	uint32_t dat = _audioBufferAddress16byteAligned;
	audiowrite32(&dat);
}

void APUSetSampleRate(struct EAudioContext* _context, enum EAPUSampleRate _sampleRate)
{
	_context->m_sampleRate = _sampleRate;

	uint32_t cmd = APUCMD_SETRATE;
	audiowrite32(&cmd);
	uint32_t dat = _sampleRate;
	audiowrite32(&dat);
}

void APUSwapChannels(struct EAudioContext* _context, uint32_t _swap)
{
	uint32_t cmd = APUCMD_SWAPCHANNELS;
	audiowrite32(&cmd);
	uint32_t dat = _swap;
	audiowrite32(&dat);
}

void APUSync(struct EAudioContext* _context)
{
	// Dummy command
	uint32_t cmd = APUCMD_NOOP;
	audiowrite32(&cmd);
}

uint32_t APUFrame(struct EAudioContext* _context)
{
	uint32_t status = audioread32();
	return status;
}

int APUInitAudio(struct EAudioContext* _context, struct SPPlatform* _platform)
{
	_context->m_platform = _platform;
	_context->m_sampleRate = ASR_44_100_Hz;
	_context->m_bufferSize = 0;

	return 0;
}

void APUShutdownAudio(struct EAudioContext* _context)
{
	if (_context->m_sampleRate != ASR_Halt)
	{
		// In case the user forgot to stop audio
		APUSetSampleRate(_context, ASR_Halt);
	}
}
