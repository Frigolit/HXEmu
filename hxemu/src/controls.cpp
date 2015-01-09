#include "controls.h"

CControls::CControls(CHX20 *mch) {
	machine = mch;
	surface = SDL_CreateRGBSurface(SDL_RLEACCEL, 256, 128, 32, 0, 0, 0, 0);
	widgets = new vector<CWidget *>();

	// Create widgets
	CButton *btn_pause       = new CButton("Pause",         0,  0, 64, 24);
	CButton *btn_menu        = new CButton("Menu",         64,  0, 64, 24);
	CButton *btn_break       = new CButton("Break",       128,  0, 64, 24);
	CButton *btn_reset       = new CButton("Reset",       192,  0, 64, 24);

	CButton *btn_power       = new CButton("Power",         0, 24, 64, 24);
	CButton *btn_monitor     = new CButton("Monitor",      64, 24, 64, 24);
	CButton *btn_nmi         = new CButton("NMI",         128, 24, 64, 24);
	CButton *btn_trace       = new CButton("Trace",       192, 24, 64, 24);

	CButton *btn_peripherals = new CButton("Peripherals",   0, 48, 64, 24);
	CButton *btn_taperom     = new CButton("Tape/ROM",     64, 48, 64, 24);
	CButton *btn_printer     = new CButton("Printer",     128, 48, 64, 24);
	CButton *btn_motherboard = new CButton("Motherboard", 192, 48, 64, 24);

	// Bind widget callbacks
	//btn_pause
	//btn_menu
	//btn_break
	btn_reset->set_click_callback(std::bind(&CControls::cb_btn_reset, this, std::placeholders::_1));
	btn_power->set_click_callback(std::bind(&CControls::cb_btn_power, this, std::placeholders::_1));
	btn_monitor->set_click_callback(std::bind(&CControls::cb_btn_monitor, this, std::placeholders::_1));
	//btn_nmi
	btn_trace->set_click_callback(std::bind(&CControls::cb_btn_trace, this, std::placeholders::_1));
	//btn_peripherals
	//btn_taperom
	//btn_printer
	//btn_motherboard

	// Add widgets
	widgets->push_back(btn_pause);
	widgets->push_back(btn_menu);
	widgets->push_back(btn_break);
	widgets->push_back(btn_reset);

	widgets->push_back(btn_power);
	widgets->push_back(btn_monitor);
	widgets->push_back(btn_nmi);
	widgets->push_back(btn_trace);

	widgets->push_back(btn_peripherals);
	widgets->push_back(btn_taperom);
	widgets->push_back(btn_printer);
	widgets->push_back(btn_motherboard);

	modified = true;
}

CControls::~CControls() {
	SDL_FreeSurface(surface);
}

void CControls::render(SDL_Surface *dest, int x, int y) {
	SDL_Rect src, dst;

	dst.x = x; dst.y = y; dst.w = 256; dst.h = 128;
	src.x = 0; src.y = 0; src.w = 256; src.h = 128;

	//SDL_FillRect(surface, &src, 0);

	int sz = widgets->size();

	for (int i = 0; i < sz; i++) {
		CWidget *w = widgets->at(i);
		if (w->update()) {
			w->draw(surface);
			modified = true;
		}
	}

	if (modified) SDL_BlitSurface(surface, &src, dest, &dst);
	modified = false;
}

void CControls::mousedown(int x, int y) {
	int sz = widgets->size();

	for (int i = 0; i < sz; i++) {
		CWidget *w = widgets->at(i);
		if (w->visible && (x >= w->x) && (y >= w->y) && (x < w->x + w->w) && (y < w->y + w->h)) {
			w->mousedown(x - w->x, y - w->y);
			return;
		}
	}
}

void CControls::mouseup(int x, int y) {
	int sz = widgets->size();

	for (int i = 0; i < sz; i++) {
		CWidget *w = widgets->at(i);
		if (w->visible && (x >= w->x) && (y >= w->y) && (x < w->x + w->w) && (y < w->y + w->h)) {
			w->mouseup(x - w->x, y - w->y);
			return;
		}
	}
}

// Button callbacks
// ============================================================================
void CControls::cb_btn_reset(CWidget *widget) {
	machine->reset();
}

void CControls::cb_btn_power(CWidget *widget) {
	machine->poweroff();
}

void CControls::cb_btn_monitor(CWidget *widget) {
	machine->mcu_master->trap();
}

void CControls::cb_btn_trace(CWidget *widget) {
	machine->mcu_master->b_trace = !machine->mcu_master->b_trace;
}

