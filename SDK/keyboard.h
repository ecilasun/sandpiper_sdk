#pragma once

#include "platform.h"

#define KPUCMD_SCANMATRIX   0x00000000
#define KPUCMD_NOOP         0x00000001

struct EKeyboardContext
{
	struct SPPlatform *m_platform;
	uint64_t m_previousKeyStates;
	uint64_t m_keyStates;
};

int KPUInitKeyboard(struct EKeyboardContext* _context, struct SPPlatform* _platform);
void KPUShutdownKeyboard(struct EKeyboardContext* _context);

void KPUScanMatrix(struct EKeyboardContext* _context);

/*#define KEYBOARD_SCANCODE_INDEX 0
#define KEYBOARD_STATE_INDEX 1
#define KEYBOARD_MODIFIERS_LOWER_INDEX 2
#define KEYBOARD_MODIFIERS_UPPER_INDEX 3

#define KEYBOARD_PACKET_SIZE 4

uint8_t KeyboardScanCodeToASCII(uint8_t scanCode, uint8_t uppercase);
void KeyboardFifoWriteReverse(uint8_t *_sequence, const uint32_t _length);
int KeyboardFifoRead();
void KeyboardProcessState(uint8_t *scandata);
void KeyboardReadState(uint8_t *scandata);
//void KeyboardUpdateState(uint8_t *scandata);*/