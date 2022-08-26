#ifndef __UI_KEYBOARD_H__
#define __UI_KEYBOARD_H__

#include "widget.h"
#include <functional>

class CKeyboardWidget : public CWidget {
	public:

	protected:
		CButton *btn_pause;
		CButton *btn_menu;
		CButton *btn_break;

		CButton *btn_pf1;
		CButton *btn_pf2;
		CButton *btn_pf3;
		CButton *btn_pf4;
		CButton *btn_pf5;

		CButton **btn_key;
};

#endif

