#pragma once

#include "platform.h"

#define APUCMD_BUFFERSIZE  0x00000000
#define APUCMD_START       0x00000001
#define APUCMD_NOOP1       0x00000002
#define APUCMD_NOOP2       0x00000003
#define APUCMD_SETRATE     0x00000004

enum EAPUSampleRate
{
	ASR_44_100_Hz = 0,	// 44.1000 KHz
	ASR_22_050_Hz = 1,	// 22.0500 KHz
	ASR_11_025_Hz = 2,	// 11.0250 KHz
	ASR_Halt = 3,		// Halt
};

enum EAPUBufferSize
{
	ABS_256Bytes  = 0,	//  64 16bit stereo samples
	ABS_512Bytes  = 1,	// 128 16bit stereo samples
	ABS_1024Bytes = 2,	// 256 16bit stereo samples
	ABS_2048Bytes = 3,	// 512 16bit stereo samples
	ABS_4096Bytes = 4,	// 1024 16bit stereo samples
	ABS_128Bytes = 5,	// 32 16bit stereo samples - minimum allowed
};

struct EAudioContext
{
	struct SPPlatform *m_platform;
	enum EAPUSampleRate m_sampleRate;
	uint32_t m_bufferSize;
};

int APUInitAudio(struct EAudioContext* _context, struct SPPlatform* _platform);
void APUShutdownAudio(struct EAudioContext* _context);

void APUSetBufferSize(struct EAudioContext* _context, enum EAPUBufferSize _bufferSize);
void APUStartDMA(struct EAudioContext* _context, uint32_t audioBufferAddress16byteAligned);
void APUSetSampleRate(struct EAudioContext* _context, enum EAPUSampleRate sampleRate);
uint32_t APUFrame(struct EAudioContext* _context);
