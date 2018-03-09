#pragma once

#include <stdint.h>

#define KEYBOARD_SCANCODE_INDEX 0
#define KEYBOARD_STATE_INDEX 1
#define KEYBOARD_MODIFIERS_LOWER_INDEX 2
#define KEYBOARD_MODIFIERS_UPPER_INDEX 3

#define KEYBOARD_PACKET_SIZE 4

uint8_t KeyboardScanCodeToASCII(uint8_t scanCode, uint8_t uppercase);
void KeyboardFifoWriteReverse(uint8_t *_sequence, const uint32_t _length);
int KeyboardFifoRead();
void KeyboardProcessState(uint8_t *scandata);
void KeyboardReadState(uint8_t *scandata);
//void KeyboardUpdateState(uint8_t *scandata);
