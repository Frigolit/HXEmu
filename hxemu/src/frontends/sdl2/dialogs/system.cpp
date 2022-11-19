#ifdef FRONTEND_SDL2

#include "../../../globals.h"
#include "../fonts.h"

#include "system.h"

SystemDialog::SystemDialog(CHX20 *hx20, int x, int y) : ConfigDialog(hx20, x, y) {
	draw_dialog_title("System configuration");

	// Add elements
	Checkbox *chk_test = new Checkbox(16, 64, 500);
	chk_test->set_caption("Checkbox test");
	widgets->push_back(chk_test);
}

#endif
