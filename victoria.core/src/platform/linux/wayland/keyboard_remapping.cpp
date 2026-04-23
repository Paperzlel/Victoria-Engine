#include "platform/linux/wayland/keyboard_remapping.h"

HashTable<xkb_keycode_t, Key, KeyboardRemapping::HashTableKeyHasher> KeyboardRemapping::scancode_map;
HashTable<Key, xkb_keycode_t, KeyboardRemapping::HashTableKeyHasher> KeyboardRemapping::inv_scancode_map;

void KeyboardRemapping::initialize() {
	// Directly translate scancode to our key enum. XKB doesn't actually send the proper keycode over for some reason,
	// so we have to map it manually.
	// Keycodes are gotten from /usr/share/X11/xkb/keycodes/evdev
	scancode_map[0x09] = Key::ESCAPE;
	scancode_map[0x0a] = Key::KEY_1;
	scancode_map[0x0b] = Key::KEY_2;
	scancode_map[0x0c] = Key::KEY_3;
	scancode_map[0x0d] = Key::KEY_4;
	scancode_map[0x0e] = Key::KEY_5;
	scancode_map[0x0f] = Key::KEY_6;
	scancode_map[0x10] = Key::KEY_7;
	scancode_map[0x11] = Key::KEY_8;
	scancode_map[0x12] = Key::KEY_9;
	scancode_map[0x13] = Key::KEY_0;
	scancode_map[0x14] = Key::MINUS;
	scancode_map[0x15] = Key::PLUS;
	scancode_map[0x16] = Key::BACKSPACE;
	scancode_map[0x17] = Key::TAB;
	scancode_map[0x18] = Key::Q;
	scancode_map[0x19] = Key::W;
	scancode_map[0x1a] = Key::E;
	scancode_map[0x1b] = Key::R;
	scancode_map[0x1c] = Key::T;
	scancode_map[0x1d] = Key::Y;
	scancode_map[0x1e] = Key::U;
	scancode_map[0x1f] = Key::I;
	scancode_map[0x20] = Key::O;
	scancode_map[0x21] = Key::P;
	scancode_map[0x22] = Key::OPENBRACKET;
	scancode_map[0x23] = Key::CLOSEDBRACKET;
	scancode_map[0x24] = Key::ENTER;
	scancode_map[0x25] = Key::CTRL;
	scancode_map[0x26] = Key::A;
	scancode_map[0x27] = Key::S;
	scancode_map[0x28] = Key::D;
	scancode_map[0x29] = Key::F;
	scancode_map[0x2a] = Key::G;
	scancode_map[0x2b] = Key::H;
	scancode_map[0x2c] = Key::J;
	scancode_map[0x2d] = Key::K;
	scancode_map[0x2e] = Key::L;
	scancode_map[0x2f] = Key::SEMICOLON;
	scancode_map[0x30] = Key::APOSTROPHE;
	scancode_map[0x31] = Key::BACKTICK;
	scancode_map[0x32] = Key::SHIFT;
	scancode_map[0x33] = Key::BACKSLASH;
	scancode_map[0x34] = Key::Z;
	scancode_map[0x35] = Key::X;
	scancode_map[0x36] = Key::C;
	scancode_map[0x37] = Key::V;
	scancode_map[0x38] = Key::B;
	scancode_map[0x39] = Key::N;
	scancode_map[0x3a] = Key::M;
	scancode_map[0x3b] = Key::COMMA;
	scancode_map[0x3c] = Key::PERIOD;
	scancode_map[0x3d] = Key::SLASH;
	scancode_map[0x3e] = Key::SHIFT;
	scancode_map[0x3f] = Key::KP_MULTIPLY;
	scancode_map[0x40] = Key::ALT;
	scancode_map[0x41] = Key::SPACEBAR;
	scancode_map[0x42] = Key::CAPSLOCK;
	scancode_map[0x43] = Key::F1;
	scancode_map[0x44] = Key::F2;
	scancode_map[0x45] = Key::F3;
	scancode_map[0x46] = Key::F4;
	scancode_map[0x47] = Key::F5;
	scancode_map[0x48] = Key::F6;
	scancode_map[0x49] = Key::F7;
	scancode_map[0x4a] = Key::F8;
	scancode_map[0x4b] = Key::F9;
	scancode_map[0x4c] = Key::F10;
	scancode_map[0x4d] = Key::NUMLOCK;
	scancode_map[0x4e] = Key::SCREENLOCK;
	scancode_map[0x4f] = Key::KP_7;
	scancode_map[0x50] = Key::KP_8;
	scancode_map[0x51] = Key::KP_9;
	scancode_map[0x52] = Key::KP_SUBTRACT;
	scancode_map[0x53] = Key::KP_4;
	scancode_map[0x54] = Key::KP_5;
	scancode_map[0x55] = Key::KP_6;
	scancode_map[0x56] = Key::KP_ADD;
	scancode_map[0x57] = Key::KP_1;
	scancode_map[0x58] = Key::KP_2;
	scancode_map[0x59] = Key::KP_3;
	scancode_map[0x5a] = Key::KP_0;
	scancode_map[0x5b] = Key::KP_DECIMAL;
	// scancode_map[0x5c]
	// scancode_map[0x5d]
	scancode_map[0x5e] = Key::BACKTICK;
	scancode_map[0x5f] = Key::F11;
	scancode_map[0x60] = Key::F12;
	scancode_map[0x61] = Key::BACKSLASH;
	// scancode_map[0x62] = Key::NONE; // Katakana
	// scancode_map[0x63] = Key::NONE; // Hiragana
	// scancode_map[0x64] = Key::NONE; // Henkan
	// scancode_map[0x65] = Key::NONE; // Hiragana/Katakana toggle
	// scancode_map[0x66] = Key::NONE; // Muhenkan
	// scancode_map[0x67] = Key::NONE; // Keypad JP Comma
	scancode_map[0x68] = Key::KP_ENTER;
	scancode_map[0x69] = Key::CTRL;
	scancode_map[0x6a] = Key::KP_DIVIDE;
	scancode_map[0x6b] = Key::PRINTSCREEN;
	scancode_map[0x6c] = Key::ALT;
	// scancode_map[0x6d]
	scancode_map[0x6e] = Key::HOME;
	scancode_map[0x6f] = Key::UP;
	scancode_map[0x70] = Key::PAGEUP;
	scancode_map[0x71] = Key::LEFT;
	scancode_map[0x72] = Key::RIGHT;
	scancode_map[0x73] = Key::END;
	scancode_map[0x74] = Key::DOWN;
	scancode_map[0x75] = Key::PAGEDOWN;
	scancode_map[0x76] = Key::INSERT;
	scancode_map[0x77] = Key::KEY_DELETE;
	// scancode_map[0x78]
	scancode_map[0x79] = Key::VOLUMEMUTE;
	scancode_map[0x7a] = Key::VOLUMEDOWN;
	scancode_map[0x7b] = Key::VOLUMEUP;
	// scancode_map[0x7c] = Key::POWER;
	scancode_map[0x7d] = Key::KP_EQUAL;
	// scancode_map[0x7e] // KP_PlusMinus
	scancode_map[0x7f] = Key::PAUSE;
	// scancode_map[0x80]
	scancode_map[0x81] = Key::KP_COMMA;
	// scancode_map[0x82] = Key::NONE; // Hangul latin toggle
	// scancode_map[0x83] = Key::NONE; // Hangul to Hanja conversion
	// scancode_map[0x84] = Key::NONE; // Yen
	scancode_map[0x85] = Key::META;
	scancode_map[0x86] = Key::META;
	scancode_map[0x87] = Key::MENU;

	// ...

	scancode_map[0xbf] = Key::F13;
	scancode_map[0xc0] = Key::F14;
	scancode_map[0xc1] = Key::F15;
	scancode_map[0xc2] = Key::F16;
	scancode_map[0xc3] = Key::F17;
	scancode_map[0xc4] = Key::F18;
	scancode_map[0xc5] = Key::F19;
	scancode_map[0xc6] = Key::F20;
	scancode_map[0xc7] = Key::F21;
	scancode_map[0xc8] = Key::F22;
	scancode_map[0xc9] = Key::F23;
	scancode_map[0xca] = Key::F24;

	// Add inverse key/value pairs
	for (const KeyValue<xkb_keycode_t, Key> &kv : scancode_map) {
		inv_scancode_map.insert(kv.value, kv.key);
	}
}

xkb_keycode_t KeyboardRemapping::get_xkb_keycode_from_key(Key p_key) {
	xkb_keycode_t *key = inv_scancode_map.get_ptr(p_key);

	if (key == nullptr) {
		return 0x00;
	}

	return *key;
}

Key KeyboardRemapping::get_key_from_xkb_keycode(xkb_keycode_t p_xkb_key) {
	Key *key = scancode_map.get_ptr(p_xkb_key);
	if (key == nullptr) {
		return Key::NONE;
	}

	return *key;
}
