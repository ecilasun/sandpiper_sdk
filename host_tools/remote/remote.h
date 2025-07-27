#pragma once

#include "platform.h"
#include "common.h"
#include "serial.h"
#include "vpu.h"
#include "apu.h"
#include "lz4.h"

struct AppCtx
{
	VideoCapture* video;
	AudioPlayback* audio;
	CSerialPort* serial;
	SDL_GameController* gamecontroller;
};
