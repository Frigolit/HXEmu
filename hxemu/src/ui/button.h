#ifndef __UI_BUTTON_H__
#define __UI_BUTTON_H__

#include "widget.h"
#include <functional>

class CButton : public CWidget {
	public:
		CButton(const char *c, int x, int y, int w, int h);
		~CButton();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		virtual void mouseup(int x, int y);

		void set_caption(const char *new_caption);
		void set_click_callback(std::function<void(CWidget*)> callback);

	protected:
		char *caption;
		bool updated;

		std::function<void(CWidget*)> cb_click;
};

#endif

