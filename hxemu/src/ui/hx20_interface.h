#ifndef __UI_HX20_H__
#define __UI_HX20_H__

#include <functional>

#include "widget.h"
#include "keyboard.h"

#include "../hx20.h"

class CHX20InterfaceWidget : public CWidget {
	public:
		CHX20InterfaceWidget(CHX20 *hx20, int x, int y);
		~CHX20InterfaceWidget();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		virtual void mousedown(int x, int y);
		virtual void mouseup(int x, int y);

	protected:
		CHX20 *hx20;
		CKeyboardWidget *keyboard;
};

#endif

