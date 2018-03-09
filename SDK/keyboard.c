#include "keyboard.h"
//#include <assert.h>

int KPUInitKeyboard(struct EKeyboardContext* _context, struct SPPlatform* _platform)
{
	_context->m_platform = _platform;
	_context->m_keyStates = 0;
	_context->m_previousKeyStates = 0;

	return 0;
}

void KPUShutdownKeyboard(struct EKeyboardContext* _context)
{
	// TODO:
}

void KPUScanMatrix(struct EKeyboardContext* _context)
{
	// Stash previous key states
	_context->m_previousKeyStates = _context->m_keyStates;

	//assert(_context->m_platform != NULL && "call KPUInitKeyboard() first\n");

	// Send a scan command to the matrix scanner
	metal_io_write32(_context->m_platform->keyboardio, 0, KPUCMD_SCANMATRIX);

	// Delay for a very small bit to allow the scan to complete
	// NOTE: Ideally the result would be written to a FIFO so we can check the size before reading it
	//usleep(10);

	// Read the new key states (total of 64 bits for 63 keys, highest bit is unused for now)
	//_context->m_keyStates = metal_io_read32(_context->m_platform->keyboardio, 0) | (metal_io_read32(_context->m_platform->keyboardio, 4) << 32);
}

/*static int s_control = 0;
static int s_alt = 0;
static int s_keyfifocursor = -1;
static uint8_t s_keyfifo[512];

// Scan code to lower case ASCII conversion table
static const char s_scantoasciitable_lowercase[] = {
//   0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	  0,    0,    0,    0,  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l', // 0
	'm',  'n',  'o',  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',  '1',  '2', // 1
	'3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',   10,   27,    8,    9,  ' ',  '-',  '=',  '[', // 2
	']', '\\',  '#',  ';', '\'',  '^',  ',',  '.',  '/',    0,    0,    0,    0,    0,    0,    0, // 3
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // 4
	  0,    0,    0,    0,  '/',  '*',  '-',  '+',   13,  '1',  '2',  '3',  '4',  '5',  '6',  '7', // 5
	'8',  '9',  '0',  '.', '\\',    0,    0,  '=',    0,    0,    0,    0,    0,    0,    0,    0, // 6
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // 7
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // 8
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // 9
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // A
	  0,    0,    0,    0,    0,    0,  '(',  ')',  '{',  '}',    8,    9,    0,    0,    0,    0, // B
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // C
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // D
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // E
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0  // F
};


// Scan code to upper case ASCII conversion table
static const char s_scantoasciitable_uppercase[] = {
//   0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	  0,    0,    0,    0,  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L', // 0
	'M',  'N',  'O',  'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  '!',  '@', // 1
	'#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',   10,   27,    8,    9,  ' ',  '_',  '+',  '{', // 2
	'}',  '|',  '~',  ':',  '"',  '~',  '<',  '>',  '?',    0,    0,    0,    0,    0,    0,    0, // 3
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // 4
	  0,    0,    0,    0,  '/',  '*',  '-',  '+',   13,  '1',  '2',  '3',  '4',  '5',  '6',  '7', // 5
	'8',  '9',  '0',  '.', '\\',    0,    0,  '=',    0,    0,    0,    0,    0,    0,    0,    0, // 6
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // 7
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // 8
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // 9
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // A
	  0,    0,    0,    0,    0,    0,  '(',  ')',  '{',  '}',    8,    9,    0,    0,    0,    0, // B
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // C
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // D
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // E
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0  // F
};

uint8_t KeyboardScanCodeToASCII(uint8_t scanCode, uint8_t uppercase)
{
	uint8_t ascii;
	if (uppercase)
		ascii = s_scantoasciitable_uppercase[scanCode];
	else
		ascii = s_scantoasciitable_lowercase[scanCode];
	return ascii;
}


void KeyboardFifoWriteReverse(uint8_t *_sequence, const uint32_t _length)
{
    for (uint32_t i=0; i<_length; ++i)
        s_keyfifo[++s_keyfifocursor] = _sequence[i];
}

int KeyboardFifoRead()
{
    if (s_keyfifocursor != -1)
        return s_keyfifo[s_keyfifocursor--];
    else
        return -1;
}

void KeyboardProcessState(uint8_t *scandata)
{
	uint8_t scancode = scandata[KEYBOARD_SCANCODE_INDEX];
	uint8_t state = scandata[KEYBOARD_STATE_INDEX];

	uint8_t modifiers_lower = scandata[KEYBOARD_MODIFIERS_LOWER_INDEX];
	uint8_t modifiers_upper = scandata[KEYBOARD_MODIFIERS_UPPER_INDEX];
	uint32_t modifiers = (modifiers_upper << 8) | modifiers_lower;

	// Ignore some unused keys
	if (scancode == 0xE3 || scancode == 0x65 || scancode == 0x39 || scancode == 0xE1 || scancode == 0xE5) // Windows, Menu key, Caps Lock, Shift keys
	{
		// Ignore
		return;
	}

	int isUppercase = (modifiers & 0x2003) ? 1 : 0; // Check if Caps Lock or either shift is down

	// Left or Rigth Control key
	if (scancode == 0xE0 || scancode == 0xE4)
	{
		// Toggle control state
		s_control = state ? 1 : 0;
		return;
	}

	// Left or Right Alt key
	if (scancode == 0xE2 || scancode == 0xE6)
	{
		// Toggle alt state
		s_alt = state ? 1 : 0;
		return;
	}

	// NOTE: Remote side handles trapping CTRL+C and ~ keys so we don't neeed to be concerned with them here
	if (state == 1)// && scancode<256)
	{
        // Note that fifo writes are in reverse order
		if (scancode == 0x52) // Up arrow
		{
			uint8_t sequence[2] = { 72, 224 };
			KeyboardFifoWriteReverse(sequence, 2);
		}
		else if (scancode == 0x51) // Down arrow
		{
			uint8_t sequence[2] = { 80, 224 };
			KeyboardFifoWriteReverse(sequence, 2);
		}
		else if (scancode == 0x50) // Left arrow
		{
			uint8_t sequence[2] = { 75, 224 };
			KeyboardFifoWriteReverse(sequence, 2);
		}
		else if (scancode == 0x4F) // Right arrow
		{
			uint8_t sequence[2] = { 77, 224 };
			KeyboardFifoWriteReverse(sequence, 2);
		}
		else if (scancode == 0x48) // Pause, consider same as CTRL+C
		{
			uint8_t sequence[2] = { 3 };
			KeyboardFifoWriteReverse(sequence, 1);
		}
		else if (scancode == 0x49) // Insert
        {
			uint8_t sequence[2] = { 82, 224 };
			KeyboardFifoWriteReverse(sequence, 2);
        }
		else if (scancode == 0x4A) // Home
        {
			uint8_t sequence[2] = { 71, 224 };
			KeyboardFifoWriteReverse(sequence, 2);
        }
		else if (scancode == 0x4C) // Delete
        {
			uint8_t sequence[2] = { 83, 224 };
			KeyboardFifoWriteReverse(sequence, 2);
        }
		else if (scancode == 0x4D) // End
        {
			uint8_t sequence[2] = { 79, 224 };
			KeyboardFifoWriteReverse(sequence, 2);
        }
        // 0x4B) // PageUp
        // 0x4E) // PageDown
		uint8_t ascii = KeyboardScanCodeToASCII(scancode, isUppercase);
		KeyboardFifoWriteReverse(&ascii, 1);
	}
}

void KeyboardReadState(uint8_t *scandata)
{
    u32 packetsize = 0;
    while (packetsize != KEYBOARD_PACKET_SIZE)
    {
        int read = XUartPs_Recv(uart, scandata+packetsize, 1);
        if (read)
        {
            //xil_printf("%02X ", scandata[packetsize]);
            packetsize++;
        }
    }
}

void KeyboardUpdateState(uint8_t *scandata)
{
    // NOTE: This should be a fifo

	volatile struct SKeyboardState* keys = (volatile struct SKeyboardState*)KERNEL_INPUTBUFFER;

	keys->scancode = scandata[KEYBOARD_SCANCODE_INDEX];
	keys->state = scandata[KEYBOARD_STATE_INDEX];
	uint8_t modifiers_lower = scandata[KEYBOARD_MODIFIERS_LOWER_INDEX];
	uint8_t modifiers_upper = scandata[KEYBOARD_MODIFIERS_UPPER_INDEX];
	keys->modifiers = (modifiers_upper << 8) | modifiers_lower;
	int isUppercase = (keys->modifiers & 0x2003) ? 1 : 0;
	keys->ascii = KeyboardScanCodeToASCII(keys->scancode, isUppercase);

	keys->count = keys->count + 1;
}*/
