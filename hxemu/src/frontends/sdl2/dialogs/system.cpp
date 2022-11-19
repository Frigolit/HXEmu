#ifdef FRONTEND_SDL2

#include "../../../globals.h"
#include "../fonts.h"

#include "system.h"

SystemDialog::SystemDialog(CHX20 *hx20, int x, int y) : ConfigDialog(hx20, x, y) {
	draw_dialog_title("System configuration");

	// Add elements
	chk_test = new Checkbox(this, 16, 64, 500);
	chk_test->set_caption("Checkbox test");
	widgets->push_back(chk_test);

	CButton *btn_test = new CButton(this, "Test button", 16, 128, 128, 24);
	widgets->push_back(btn_test);
}

#endif
