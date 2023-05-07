#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_DIALOGS_EXPERT
#define HXEMU_FRONTENDS_SDL2_DIALOGS_EXPERT

#include <functional>

#include "config_dialog.h"

#include "../widgets/button.h"

class ExpertDialog : public ConfigDialog {
	public:
		ExpertDialog(CHX20 *hx20, int x, int y);

	protected:
		CButton *btn_trace;

		void cb_trace_click(CButton *btn, int a, int b);
};

#endif
#endif
