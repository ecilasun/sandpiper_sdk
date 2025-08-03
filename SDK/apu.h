#pragma once

#include "platform.h"

int APUInitAudio(struct EAudioContext* _context, struct SPPlatform* _platform);
void APUShutdownAudio(struct EAudioContext* _context);

void APUSetBufferSize(struct EAudioContext* _context, enum EAPUBufferSize _bufferSize);
void APUStartDMA(struct EAudioContext* _context, uint32_t _audioBufferAddress16byteAligned);
void APUSync(struct EAudioContext* _context);
void APUSetSampleRate(struct EAudioContext* _context, enum EAPUSampleRate _sampleRate);
void APUSwapChannels(struct EAudioContext* _context, uint32_t _swap);
uint32_t APUFrame(struct EAudioContext* _context);
uint32_t APUGetWordCount(struct EAudioContext* _context);
void APUWaitSync(struct EAudioContext *_context);