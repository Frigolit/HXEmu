#ifdef FRONTEND_SDL2

#include "keyboard_button.h"

CKeyboardButton::CKeyboardButton(std::string c, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) : CButton(c, x, y, w, h, r, g, b) {
	krtn_index = 0;
	krtn_bits = 0;
}

void CKeyboardButton::set_keycode(int krtn, int bits) {
	krtn_index = krtn;
	krtn_bits = bits;
}

void CKeyboardButton::get_keycode(int &krtn, int &bits) {
	krtn = krtn_index;
	bits = krtn_bits;
}

#endif
