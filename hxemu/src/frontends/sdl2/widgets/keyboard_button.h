#ifdef FRONTEND_SDL2

#ifndef HXEMU_UI_KEYBOARD_BUTTON_H
#define HXEMU_UI_KEYBOARD_BUTTON_H

#include "button.h"

class CKeyboardButton : public CButton {
	public:
		CKeyboardButton(std::string c, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b);
		~CKeyboardButton();

		void set_keycode(int krtn, int bits);
		void get_keycode(int &krtn, int &bits);

	protected:
		int krtn_index;
		int krtn_bits;
};

#endif
#endif
