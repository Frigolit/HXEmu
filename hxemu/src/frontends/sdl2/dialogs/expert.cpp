#ifdef FRONTEND_SDL2

#include "../../../globals.h"
#include "../fonts.h"

#include "expert.h"

ExpertDialog::ExpertDialog(CHX20 *hx20, int x, int y) : ConfigDialog(hx20, x, y) {
	draw_dialog_title("Expert features");

	// Test button
	btn_trace = new CButton(this, "CPU trace toggle", 16, 64, 130, 24);
	btn_trace->set_click_callback(std::bind(&ExpertDialog::cb_trace_click, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	widgets->push_back(btn_trace);
}

void ExpertDialog::cb_trace_click(CButton *btn, int a, int b) {
	if (a == 0) {
		hx20->mcu_master->b_trace ^= 1;
	}
}

#endif
