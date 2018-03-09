#include "core.h"
#include "audio.h"

void APUSetBufferSize(struct EAudioContext* _context, enum EAPUBufferSize _bufferSize)
{
	_context->m_bufferSize = 128 << (uint32_t)_bufferSize;

	*_context->m_platform->audioio = APUCMD_BUFFERSIZE;
	*_context->m_platform->audioio = (uint32_t)_bufferSize;
}

void APUStartDMA(struct EAudioContext* _context, uint32_t _audioBufferAddress16byteAligned)
{
	*_context->m_platform->audioio = APUCMD_START;
	*_context->m_platform->audioio = _audioBufferAddress16byteAligned;
}

void APUSetSampleRate(struct EAudioContext* _context, enum EAPUSampleRate _sampleRate)
{
	_context->m_sampleRate = _sampleRate;

	*_context->m_platform->audioio = APUCMD_SETRATE;
	*_context->m_platform->audioio = (uint32_t)_sampleRate;
}

void APUSwapChannels(struct EAudioContext* _context, uint32_t _swap)
{
	*_context->m_platform->audioio = APUCMD_SWAPCHANNELS;
	*_context->m_platform->audioio = (uint32_t)_swap;
}

void APUSync(struct EAudioContext* _context)
{
	// Dummy command
	*_context->m_platform->audioio = APUCMD_NOOP;
}

uint32_t APUFrame(struct EAudioContext* _context)
{
	return (*_context->m_platform->audioio);
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
