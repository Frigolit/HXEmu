#ifndef __UI_KEYBOARD_H__
#define __UI_KEYBOARD_H__

#include <vector>

#include "widget.h"
#include "button.h"

#include "../hx20.h"

class CKeyboardWidget : public CWidget {
	public:
		CKeyboardWidget(CHX20 *hx20, int x, int y, int w, int h);
		~CKeyboardWidget();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		void load_keymap(const char *path);

	protected:
		CHX20 *hx20;
		std::vector<CButton*> *keys;
};

#endif

