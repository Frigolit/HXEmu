#ifdef FRONTEND_SDL2

#ifndef HXEMU_UI_KEYBOARD_H
#define HXEMU_UI_KEYBOARD_H

#include <vector>
#include <unordered_map>

#include "widget.h"
#include "keyboard_button.h"

#include "../../../hx20.h"

class CKeyboardWidget : public CWidget {
	public:
		CKeyboardWidget(CHX20 *hx20, int x, int y, int w, int h);
		~CKeyboardWidget();

		void sdl_keydown(int keysym);
		void sdl_keyup(int keysym);

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		virtual void mousedown(int x, int y);
		virtual void mouseup(int x, int y);

		void load_keymap(const char *path);

	protected:
		CHX20 *hx20;
		std::vector<CKeyboardButton*> *keys;
		std::unordered_map<int, CKeyboardButton*> *sdl_keymap;

		void button_callback(CButton *widget, int released, int);
};

#endif
#endif
