#ifndef __UI_KEYBOARD_H__
#define __UI_KEYBOARD_H__

#include <vector>

#include "widget.h"
#include "keyboard_button.h"

#include "../hx20.h"

class CKeyboardWidget : public CWidget {
	public:
		CKeyboardWidget(CHX20 *hx20, int x, int y, int w, int h);
		~CKeyboardWidget();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		virtual void mousedown(int x, int y);
		virtual void mouseup(int x, int y);

		void load_keymap(const char *path);

	protected:
		CHX20 *hx20;
		std::vector<CKeyboardButton*> *keys;

		void button_callback(CButton *widget, int released, int);
};

#endif

