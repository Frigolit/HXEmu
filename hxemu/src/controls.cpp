#include "controls.h"

CControls::CControls(CHX20 *mch) {
	machine = mch;
	surface = SDL_CreateRGBSurface(SDL_RLEACCEL, 256, 128, 32, 0, 0, 0, 0);
	widgets = new vector<CWidget *>();

	CButton *btn_f[8];

	for (int i = 0; i < 8; i++) {
		char n[3];
		sprintf(n, "F%d", i + 1);

		CButton *b = new CButton(n, i * 32, 0, 32, 32);
		btn_f[i] = b;
		widgets->push_back(b);
	}

	// Create widgets
	CButton *btn_power = new CButton("Power",       0, 32, 64, 24);
	CButton *btn_reset = new CButton("Reset",      64, 32, 64, 24);
	CButton *btn_nmi   = new CButton("NMI",       128, 32, 64, 24);
	CButton *btn_trace = new CButton("Trace",     192, 32, 64, 24);

	// Bind widget callbacks
	btn_power->set_click_callback(std::bind(&CControls::cb_btn_power, this, std::placeholders::_1));
	btn_reset->set_click_callback(std::bind(&CControls::cb_btn_reset, this, std::placeholders::_1));
	//btn_nmi
	btn_trace->set_click_callback(std::bind(&CControls::cb_btn_trace, this, std::placeholders::_1));

	// Add widgets
	widgets->push_back(btn_power);
	widgets->push_back(btn_reset);
	widgets->push_back(btn_nmi);
	widgets->push_back(btn_trace);

	/*
	widgets->push_back(new CButton("Serial",      0, 56, 64, 24));
	widgets->push_back(new CButton("RS232",      64, 56, 64, 24));
	widgets->push_back(new CButton("CAS/ROM",   128, 56, 64, 24));
	widgets->push_back(new CButton("Printer",   192, 56, 64, 24));

	widgets->push_back(new CButton("Chips",       0, 80, 64, 24));
	widgets->push_back(new CButton("Expansion",  64, 80, 64, 24));
	widgets->push_back(new CButton("Barcode",   128, 80, 64, 24));
	widgets->push_back(new CButton("Audio",     192, 80, 64, 24));
	*/

	/*
	widgets->push_back(new CButton("Chips",       0, 104, 64, 24));
	widgets->push_back(new CButton("Expansion",  64, 104, 64, 24));
	widgets->push_back(new CButton("Barcode",   128, 104, 64, 24));
	widgets->push_back(new CButton("Audio",     192, 104, 64, 24));
	*/

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
void CControls::cb_btn_power(CWidget *widget) {
	printf("cb_btn_power()\n");
	machine->poweroff();
}

void CControls::cb_btn_reset(CWidget *widget) {
	machine->reset();
}

void CControls::cb_btn_trace(CWidget *widget) {
	machine->mcu_master->b_trace = !machine->mcu_master->b_trace;
}

