#ifdef FRONTEND_SDL2

#include "../../../globals.h"
#include "keyboard.h"

#include <json/json.h>

#include <fstream>
#include <iostream>

CKeyboardWidget::CKeyboardWidget(CHX20 *hx20, int x, int y, int w, int h) {
	::CWidget();

	this->hx20 = hx20;

	CWidget::x = x;
	CWidget::y = y;
	CWidget::w = w;
	CWidget::h = h;

	surface = SDL_CreateRGBSurface(SDL_RLEACCEL, w, h, 32, 0, 0, 0, 0);

	keys = new std::vector<CKeyboardButton *>();
	sdl_keymap = new std::unordered_map<int, CKeyboardButton*>();
	update();

	// FIXME: Should be called by creator
	char keymappath[512];
	get_data_path(keymappath, "ui/keymaps/se.json", 512);

	load_keymap(keymappath);
}

CKeyboardWidget::~CKeyboardWidget() {
	while (!keys->empty()) {
		CButton *btn = keys->back();
		keys->pop_back();
		delete(btn);
	}

	delete(keys);

	SDL_FreeSurface(surface);
}

bool CKeyboardWidget::update() {
	// Clear surface
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xA2, 0xA2, 0xA2));

	// Draw all keys
	for (int i = 0; i < (int)keys->size(); i++) {
		keys->at(i)->draw(surface);
	}

	return true;
}

void CKeyboardWidget::draw(SDL_Surface *dest) {
	CWidget::draw(dest);
}

void CKeyboardWidget::load_keymap(const char *path) {
	std::cout << "Loading keymap: " << path << std::endl;

	// Destroy existing buttons (if any)
	while (!keys->empty()) {
		CButton *btn = keys->back();
		keys->pop_back();
		delete(btn);
	}

	sdl_keymap->clear();

	update();

	// Parse keymap file
	Json::Value root;
	std::ifstream ifs;
	ifs.open(path);

	Json::CharReaderBuilder builder;
	builder["collectComments"] = true;
	JSONCPP_STRING errs;
	if (!parseFromStream(builder, ifs, &root, &errs)) {
		std::cerr << "ERROR: Failed loading keymap: " << path << std::endl;
		return;
	}

	// temp hack for format change
	root = root["keyboard"];

	if (!root.isArray()) {
		std::cerr << "ERROR: Keymap root is not an array: " << path << std::endl;
		return;
	}

	// Loop through keys
	// TODO: Improve value checks
	for (unsigned int i = 0; i < root.size(); i++) {
		Json::Value key = root[i];

		if (key["line"].empty() || key["bit"].empty()) {
			continue;
		}

		float x = key["x"].asFloat() * 60.0;
		int y = key["y"].asInt();
		float w = (key["w"].empty() ? 1.0 : key["w"].asFloat()) * 60.0;
		int c = key["color"].empty() ? 0 : key["color"].asInt();

		uint8_t krtn = (uint8_t)(key["line"].asInt());
		uint16_t value = 1 << (uint16_t)(key["bit"].asInt());

		uint8_t c_r = 64;
		uint8_t c_g = 64;
		uint8_t c_b = 64;

		if (c == 1) {
			c_r = c_g = c_b = 128;
		}
		else if (c == 2) {
			c_r = 128;
			c_g = c_b = 16;
		}

		CKeyboardButton *btn = new CKeyboardButton(key["label"].asString(), (int)x, y * 48, (int)w, 48, c_r, c_g, c_b);
		btn->set_keycode(krtn, value);
		btn->set_click_callback(std::bind(&CKeyboardWidget::button_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

		if (!key["sdlkey"].empty()) {
			int keysym = key["sdlkey"].asInt();
			sdl_keymap->emplace(keysym, btn);
		}

		keys->push_back(btn);
	}

	std::cout << "Loaded " << keys->size() << " key(s)" << std::endl;

	update();
}

void CKeyboardWidget::button_callback(CButton *widget, int released, int discard) {
	CKeyboardButton *btn = (CKeyboardButton*)widget;

	int line, bitmask;
	btn->get_keycode(line, bitmask);

	printf("keyboard - released=%d, line=%d, bitmask=0x%04X\n", released, line, bitmask);

	if (released) {
		hx20->ioctl->krtn_map[line] &= ~bitmask;
		hx20->keyboard_up(0);
	}
	else {
		hx20->ioctl->krtn_map[line] |= bitmask;
		hx20->keyboard_down(0);
	}
}

CWidget* CKeyboardWidget::mousedown(int x, int y) {
	int sz = keys->size();

	for (int i = 0; i < sz; i++) {
		CWidget *w = keys->at(i);
		if (w->visible && (x >= w->x) && (y >= w->y) && (x < w->x + w->w) && (y < w->y + w->h)) {
			return w->mousedown(x - w->x, y - w->y);
		}
	}

	return NULL;
}

CWidget* CKeyboardWidget::mouseup(int x, int y) {
	int sz = keys->size();

	for (int i = 0; i < sz; i++) {
		CWidget *w = keys->at(i);
		if (w->visible && (x >= w->x) && (y >= w->y) && (x < w->x + w->w) && (y < w->y + w->h)) {
			return w->mouseup(x - w->x, y - w->y);
		}
	}

	return NULL;
}

void CKeyboardWidget::sdl_keydown(int keysym) {
	if (sdl_keymap->count(keysym) == 1) {
		button_callback(sdl_keymap->at(keysym), 0, 0);
	}
	else {
		printf("CKeyboardWidget::sdl_keydown(): debug: unhandled keysym %d\n", keysym);
	}
}

void CKeyboardWidget::sdl_keyup(int keysym) {
	if (sdl_keymap->count(keysym) == 1) {
		button_callback(sdl_keymap->at(keysym), 1, 0);
	}
}

#endif
