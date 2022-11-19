#ifdef FRONTEND_SDL2

#include "../../../globals.h"
#include "../fonts.h"

#include "system.h"

SystemDialog::SystemDialog(CHX20 *hx20, int x, int y) : ConfigDialog(hx20, x, y) {
	draw_dialog_title("System configuration");

	// DIP switch
	Label *dip_switch_label = new Label(this, 16, 64, 72, 32);
	dip_switch_label->set_caption("DIP switch:");
	widgets->push_back(dip_switch_label);

	chk_dip0 = new Checkbox(this, dip_switch_label->x + dip_switch_label->w, 64, 32);
	chk_dip1 = new Checkbox(this, chk_dip0->x + chk_dip0->w, chk_dip0->y, chk_dip0->w);
	chk_dip2 = new Checkbox(this, chk_dip1->x + chk_dip1->w, chk_dip0->y, chk_dip0->w);
	chk_dip3 = new Checkbox(this, chk_dip2->x + chk_dip2->w, chk_dip0->y, chk_dip0->w);
	widgets->push_back(chk_dip0);
	widgets->push_back(chk_dip1);
	widgets->push_back(chk_dip2);
	widgets->push_back(chk_dip3);

	bool dip0, dip1, dip2, dip3;
	hx20->ioctl->get_dip_switch(&dip0, &dip1, &dip2, &dip3);
	chk_dip0->set_checked(dip0);
	chk_dip1->set_checked(dip1);
	chk_dip2->set_checked(dip2);
	chk_dip3->set_checked(dip3);
}

#endif
