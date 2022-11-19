#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_DIALOGS_SYSTEM
#define HXEMU_FRONTENDS_SDL2_DIALOGS_SYSTEM

#include <functional>

#include "config_dialog.h"

#include "../widgets/button.h"
#include "../widgets/checkbox.h"

class SystemDialog : public ConfigDialog {
	public:
		SystemDialog(CHX20 *hx20, int x, int y);

	protected:
		Checkbox *chk_test;
};

#endif
#endif
