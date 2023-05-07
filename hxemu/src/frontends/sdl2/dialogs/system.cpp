#ifdef FRONTEND_SDL2

#include "../../../globals.h"
#include "../fonts.h"

#include "system.h"

SystemDialog::SystemDialog(CHX20 *hx20, int x, int y) : ConfigDialog(hx20, x, y) {
	draw_dialog_title("System configuration");

	int next_section_y = 58;

	// Machine selection
	// TODO: Load/Save buttons (config files in [~/.local/share/hxemu/]data/machines/)

	// ROMs
	#ifdef DEVMODE
	Label *roms_heading_label = new Label(this, 16, next_section_y, 128, 24);
	roms_heading_label->set_caption("ROM selection");
	roms_heading_label->set_font(font_dialog_text_bold);
	widgets->push_back(roms_heading_label);

	Label *rom0_label = new Label(this, 16, roms_heading_label->y + roms_heading_label->h, 48, 20);
	Label *rom0_todo_label = new Label(this, rom0_label->x + rom0_label->w, rom0_label->y, 300, rom0_label->h);
	rom0_label->set_caption("ROM0:");
	rom0_todo_label->set_caption("[todo: textbox + button]");
	widgets->push_back(rom0_label);
	widgets->push_back(rom0_todo_label);

	Label *rom1_label = new Label(this, rom0_label->x, rom0_label->y + rom0_label->h + 2, rom0_label->w, rom0_label->h);
	Label *rom1_todo_label = new Label(this, rom0_todo_label->x, rom1_label->y, rom0_todo_label->w, rom0_todo_label->h);
	rom1_label->set_caption("ROM1:");
	rom1_todo_label->set_caption("[todo: textbox + button]");
	widgets->push_back(rom1_label);
	widgets->push_back(rom1_todo_label);

	Label *rom2_label = new Label(this, rom0_label->x, rom1_label->y + rom1_label->h + 2, rom0_label->w, rom0_label->h);
	Label *rom2_todo_label = new Label(this, rom0_todo_label->x, rom2_label->y, rom0_todo_label->w, rom0_todo_label->h);
	rom2_label->set_caption("ROM2:");
	rom2_todo_label->set_caption("[todo: textbox + button]");
	widgets->push_back(rom2_label);
	widgets->push_back(rom2_todo_label);

	Label *rom3_label = new Label(this, rom0_label->x, rom2_label->y + rom2_label->h + 2, rom0_label->w, rom0_label->h);
	Label *rom3_todo_label = new Label(this, rom0_todo_label->x, rom3_label->y, rom0_todo_label->w, rom0_todo_label->h);
	rom3_label->set_caption("ROM3:");
	rom3_todo_label->set_caption("[todo: textbox + button]");
	widgets->push_back(rom3_label);
	widgets->push_back(rom3_todo_label);

	Label *rom_option_label = new Label(this, rom0_todo_label->x + rom0_todo_label->w + 32, rom0_label->y, 96, rom0_label->h);
	Label *rom_option_todo_label = new Label(this, rom_option_label->x + rom_option_label->w, rom_option_label->y, rom0_todo_label->w, rom0_todo_label->h);
	rom_option_label->set_caption("Option ROM:");
	rom_option_todo_label->set_caption("[todo: textbox + button]");
	widgets->push_back(rom_option_label);
	widgets->push_back(rom_option_todo_label);

	Label *rom_secondary_label = new Label(this, rom0_todo_label->x + rom0_todo_label->w + 32, rom1_label->y, 96, rom0_label->h);
	Label *rom_secondary_todo_label = new Label(this, rom_secondary_label->x + rom_secondary_label->w, rom_secondary_label->y, rom0_todo_label->w, rom0_todo_label->h);
	rom_secondary_label->set_caption("Sec. CPU ROM:");
	rom_secondary_todo_label->set_caption("[todo: textbox + button]");
	widgets->push_back(rom_secondary_label);
	widgets->push_back(rom_secondary_todo_label);

	next_section_y = rom3_label->y + rom3_label->h + 16;
	#endif

	// DIP switch + Keyboard
	Label *dipsw_heading_label = new Label(this, 16, next_section_y, 200, 24);
	dipsw_heading_label->set_caption("Input configuration");
	dipsw_heading_label->set_font(font_dialog_text_bold);
	widgets->push_back(dipsw_heading_label);

	Label *dip_switch_label = new Label(this, 16, dipsw_heading_label->y + dipsw_heading_label->h, 72, 32);
	dip_switch_label->set_caption("DIP switch:");
	widgets->push_back(dip_switch_label);

	chk_dip0 = new Checkbox(this, dip_switch_label->x + dip_switch_label->w, dip_switch_label->y, 32);
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

	#ifdef DEVMODE
	Label *keyboard_layout_label = new Label(this, chk_dip3->x + chk_dip3->w + 32, dip_switch_label->y, 110, 32);
	keyboard_layout_label->set_caption("Keyboard layout:");
	widgets->push_back(keyboard_layout_label);

	Label *keyboard_layout_todo_label = new Label(this, keyboard_layout_label->x + keyboard_layout_label->w, keyboard_layout_label->y, 96, 32);
	keyboard_layout_todo_label->set_caption("[todo: dropdown]");
	widgets->push_back(keyboard_layout_todo_label);
	#endif
}

#endif
