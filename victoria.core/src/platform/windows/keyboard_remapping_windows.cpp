#include "platform/windows/keyboard_remapping_windows.h"
#if PLATFORM_WINDOWS

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

HashTable<int, Key, KeyboardRemappingWindows::HashTableKeyHasher> KeyboardRemappingWindows::scancode_map;
HashTable<Key, int, KeyboardRemappingWindows::HashTableKeyHasher> KeyboardRemappingWindows::inv_scancode_map;

void KeyboardRemappingWindows::initialize() {
	// VK_LBUTTON = 0x01
	// VK_RBUTTON = 0x02
	// VK_CANCEL = 0x03
	// VK_MBUTTON = 0x04
	// VK_XBUTTON1 = 0x05
	// VK_XBUTTON2 = 0x06
	// (0x07 is reserved)
	scancode_map[VK_BACK] = Key::BACKSPACE;
	scancode_map[VK_TAB] = Key::TAB;
	// (0x0a and 0x0b are reserved)
	scancode_map[VK_CLEAR] = Key::CLEAR;
	scancode_map[VK_RETURN] = Key::ENTER;
	// (0x0e and 0x0f are undefined)
	scancode_map[VK_SHIFT] = Key::SHIFT;
	scancode_map[VK_CONTROL] = Key::CTRL;
	scancode_map[VK_MENU] = Key::ALT;
	scancode_map[VK_PAUSE] = Key::PAUSE;
	scancode_map[VK_CAPITAL] = Key::CAPSLOCK;
	// below modes are for IME
	// scancode_map[VK_KANA] = Key::NONE; // Kana mode
	// scancode_map[VK_HANGUL] = Key::NONE; // Hangule mode
	// scancode_map[VK_IME_ON] = Key::NONE; // IME on
	// scancode_map[VK_JUNJA] = Key::NONE; // Junja mode
	// scancode_map[VK_FINAL] = Key::NONE; // Final mode
	// scancode_map[VK_HANJA] = Key::NONE; // Hanja mode
	// scancode_map[VK_KANJI] = Key::NONE; // Kanji mode
	// scancode_map[VK_IME_OFF] = Key::NONE; // IME off
	scancode_map[VK_ESCAPE] = Key::ESCAPE;
	// scancode_map[VK_CONVERT] = Key::NONE; // IME convert
	// scancode_map[VK_NONCONVERT] = Key::NONE; // IME nonconvert
	// scancode_map[VK_ACCEPT] = Key::NONE; // IME accept
	// scancode_map[VK_MODECHANGE] = Key::NONE; // IME mode change
	scancode_map[VK_SPACE] = Key::SPACEBAR;
	scancode_map[VK_PRIOR] = Key::PAGEUP;
	scancode_map[VK_NEXT] = Key::PAGEDOWN;
	scancode_map[VK_END] = Key::END;
	scancode_map[VK_HOME] = Key::HOME;
	scancode_map[VK_LEFT] = Key::LEFT;
	scancode_map[VK_UP] = Key::UP;
	scancode_map[VK_RIGHT] = Key::RIGHT;
	scancode_map[VK_DOWN] = Key::DOWN;
	// scancode_map[VK_SELECT] = Key::NONE; // select key
	// scancode_map[VK_PRINT] = Key::NONE; // print key
	// scancode_map[VK_EXECUTE] = Key::NONE; // execute key
	scancode_map[VK_SNAPSHOT] = Key::PRINTSCREEN;
	scancode_map[VK_INSERT] = Key::INSERT;
	scancode_map[VK_DELETE] = Key::KEY_DELETE;
	// scancode_map[VK_HELP] = Key::NONE; // help key
	scancode_map[0x30] = Key::KEY_0;
	scancode_map[0x31] = Key::KEY_1;
	scancode_map[0x32] = Key::KEY_2;
	scancode_map[0x33] = Key::KEY_3;
	scancode_map[0x34] = Key::KEY_4;
	scancode_map[0x35] = Key::KEY_5;
	scancode_map[0x36] = Key::KEY_6;
	scancode_map[0x37] = Key::KEY_7;
	scancode_map[0x38] = Key::KEY_8;
	scancode_map[0x39] = Key::KEY_9;
	// (0x3a to 0x40 is undefined)
	scancode_map[0x41] = Key::A;
	scancode_map[0x42] = Key::B;
	scancode_map[0x43] = Key::C;
	scancode_map[0x44] = Key::D;
	scancode_map[0x45] = Key::E;
	scancode_map[0x46] = Key::F;
	scancode_map[0x47] = Key::G;
	scancode_map[0x48] = Key::H;
	scancode_map[0x49] = Key::I;
	scancode_map[0x4a] = Key::J;
	scancode_map[0x4b] = Key::K;
	scancode_map[0x4c] = Key::L;
	scancode_map[0x4d] = Key::M;
	scancode_map[0x4e] = Key::N;
	scancode_map[0x4f] = Key::O;
	scancode_map[0x50] = Key::P;
	scancode_map[0x51] = Key::Q;
	scancode_map[0x52] = Key::R;
	scancode_map[0x53] = Key::S;
	scancode_map[0x54] = Key::T;
	scancode_map[0x55] = Key::U;
	scancode_map[0x56] = Key::V;
	scancode_map[0x57] = Key::W;
	scancode_map[0x58] = Key::X;
	scancode_map[0x59] = Key::Y;
	scancode_map[0x5a] = Key::Z;
	scancode_map[VK_LWIN] = Key::META;
	scancode_map[VK_RWIN] = Key::META;
	// scancode_map[VK_APPS] = Key::NONE; // Application key (?)
	// (0x5e is reserved)
	// scancode_map[VK_SLEEP] = Key::NONE; // sleep key
	scancode_map[VK_NUMPAD0] = Key::KP_0;
	scancode_map[VK_NUMPAD1] = Key::KP_1;
	scancode_map[VK_NUMPAD2] = Key::KP_2;
	scancode_map[VK_NUMPAD3] = Key::KP_3;
	scancode_map[VK_NUMPAD4] = Key::KP_4;
	scancode_map[VK_NUMPAD5] = Key::KP_5;
	scancode_map[VK_NUMPAD6] = Key::KP_6;
	scancode_map[VK_NUMPAD7] = Key::KP_7;
	scancode_map[VK_NUMPAD8] = Key::KP_8;
	scancode_map[VK_NUMPAD9] = Key::KP_9;
	scancode_map[VK_MULTIPLY] = Key::KP_MULTIPLY;
	scancode_map[VK_ADD] = Key::KP_ADD;
	// scancode_map[VK_SEPARATOR] = Key::NONE; // Separator key (available on SOME OEM keyboards)
	scancode_map[VK_SUBTRACT] = Key::KP_SUBTRACT;
	scancode_map[VK_DECIMAL] = Key::KP_DECIMAL;
	scancode_map[VK_DIVIDE] = Key::KP_DIVIDE;
	scancode_map[VK_F1] = Key::F1;
	scancode_map[VK_F2] = Key::F2;
	scancode_map[VK_F3] = Key::F3;
	scancode_map[VK_F4] = Key::F4;
	scancode_map[VK_F5] = Key::F5;
	scancode_map[VK_F6] = Key::F6;
	scancode_map[VK_F7] = Key::F7;
	scancode_map[VK_F8] = Key::F8;
	scancode_map[VK_F9] = Key::F9;
	scancode_map[VK_F10] = Key::F10;
	scancode_map[VK_F11] = Key::F11;
	scancode_map[VK_F12] = Key::F12;
	scancode_map[VK_F13] = Key::F13;
	scancode_map[VK_F14] = Key::F14;
	scancode_map[VK_F15] = Key::F15;
	scancode_map[VK_F16] = Key::F16;
	scancode_map[VK_F17] = Key::F17;
	scancode_map[VK_F18] = Key::F18;
	scancode_map[VK_F19] = Key::F19;
	scancode_map[VK_F20] = Key::F20;
	scancode_map[VK_F21] = Key::F21;
	scancode_map[VK_F22] = Key::F22;
	scancode_map[VK_F23] = Key::F23;
	scancode_map[VK_F24] = Key::F24;
	// (0x88 - 0x8f is reserved)
	scancode_map[VK_NUMLOCK] = Key::NUMLOCK;
	// scancode_map[VK_SCROLL] = Key::NONE; // scroll lock key
	// TODO: differentiate left/right, this isn't actually needed
	scancode_map[VK_LSHIFT] = Key::SHIFT;
	scancode_map[VK_RSHIFT] = Key::SHIFT;
	scancode_map[VK_LCONTROL] = Key::CTRL;
	scancode_map[VK_RCONTROL] = Key::CTRL;
	scancode_map[VK_LMENU] = Key::ALT;
	scancode_map[VK_RMENU] = Key::ALT;
	// scancode_map[VK_BROWSER_BACK] = Key::NONE; // browser back
	// scancode_map[VK_BROWSER_FORWARD] = Key::NONE; // browser forward
	// scancode_map[VK_BROWSER_REFRESH] = Key::NONE; // browser refresh
	// scancode_map[VK_BROWSER_STOP] = Key::NONE; // browser stop
	// scancode_map[VK_BROWSER_SEARCH] = Key::NONE; // browser search
	// scancode_map[VK_BROWSER_FAVORITES] = Key::NONE; // browser favourites
	// scancode_map[VK_BROWSER_HOME] = Key::NONE; // browser home
	scancode_map[VK_VOLUME_MUTE] = Key::VOLUMEMUTE;
	scancode_map[VK_VOLUME_UP] = Key::VOLUMEUP;
	scancode_map[VK_VOLUME_DOWN] = Key::VOLUMEDOWN;
	// (0xb0 - 0xb9 are the media keys, then reserved)
	scancode_map[VK_OEM_1] = Key::SEMICOLON;
	scancode_map[VK_OEM_PLUS] = Key::PLUS;
	scancode_map[VK_OEM_COMMA] = Key::COMMA;
	scancode_map[VK_OEM_MINUS] = Key::MINUS;
	scancode_map[VK_OEM_PERIOD] = Key::PERIOD;
	scancode_map[VK_OEM_2] = Key::QUESTIONMARK;
	scancode_map[VK_OEM_3] = Key::BACKTICK;
	// (0xc1 - 0xc2 is reserved)
	scancode_map[VK_OEM_4] = Key::OPENBRACKET;
	scancode_map[VK_OEM_5] = Key::BACKSLASH;
	scancode_map[VK_OEM_6] = Key::HASHTAG;
	scancode_map[VK_OEM_7] = Key::AT;
	// scancode_map[VK_OEM_8] = Key::NONE; // right ctrl for canadian CSA (no idea where tbh)
	// (ignoring 0xe0 - 0xe1 as they are reserved and OEM specific respectively)
	scancode_map[VK_OEM_102] = Key::BACKSLASH;
	scancode_map[VK_OEM_CLEAR] = Key::CLEAR;

	for (const KeyValue<int, Key> &kv : scancode_map) {
		inv_scancode_map.insert(kv.value, kv.key);
	}
}

Key KeyboardRemappingWindows::get_key_from_keycode(int p_keycode) {
	Key *kptr = scancode_map.get_ptr(p_keycode);
	if (kptr == nullptr) {
		return Key::NONE;
	}

	return *kptr;
}

int KeyboardRemappingWindows::get_keycode_from_key(Key p_key) {
	int *kptr = inv_scancode_map.get_ptr(p_key);
	if (kptr == nullptr) {
		return 0;
	}

	return *kptr;
}

#endif // PLATFORM_WINDOWS