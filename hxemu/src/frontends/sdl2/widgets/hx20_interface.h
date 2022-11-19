#ifdef FRONTEND_SDL2

#ifndef HXEMU_UI_HX20_INTERFACE_H
#define HXEMU_UI_HX20_INTERFACE_H

#include <functional>

#include "widget.h"
#include "keyboard.h"

#include "../../../hx20.h"
#include "../lcd_interface.h"

class CHX20InterfaceWidget : public CWidget {
	public:
		CHX20InterfaceWidget(CHX20 *hx20, int x, int y);
		~CHX20InterfaceWidget();

		virtual void sdl_keydown(int keysym);
		virtual void sdl_keyup(int keysym);

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		virtual CWidget* mousedown(int x, int y);
		virtual CWidget* mouseup(int x, int y);

	protected:
		CHX20 *hx20;
		CKeyboardWidget *keyboard;
		Sdl2LcdInterface *lcd;
};

#endif
#endif
