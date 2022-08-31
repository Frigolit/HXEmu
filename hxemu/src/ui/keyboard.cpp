#include "keyboard.h"

#include "jsoncpp/json/json.h"
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

	/*
	SDL_Rect bg_rect;
	bg_rect.x = 0;
	bg_rect.y = 0;
	bg_rect.w = w;
	bg_rect.h = h;
	*/

	keys = new std::vector<CButton *>();
	update();

	// FIXME: Should be called by creator
	load_keymap("data/ui/keymaps/eu.json");
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
	for (int i = 0; i < keys->size(); i++) {
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

	if (!root.isArray()) {
		std::cerr << "ERROR: Keymap root is not an array: " << path << std::endl;
		return;
	}

	// Loop through keys
	// TODO: Improve value checks
	for (int i = 0; i < root.size(); i++) {
		Json::Value key = root[i];

		float x = key["x"].asFloat() * 60.0;
		int y = key["y"].asInt();
		float w = (key["w"].empty() ? 1.0 : key["w"].asFloat()) * 60.0;
		int c = key["c"].empty() ? 0 : key["c"].asInt();

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

		CButton *btn = new CButton(key["l"].asString(), (int)x, y * 48, (int)w, 48, c_r, c_g, c_b);
		keys->push_back(btn);
	}

	std::cout << "Loaded " << keys->size() << " key(s)" << std::endl;

	update();
}

/*
array colors = ({
	({ 64, 64, 64 }),
	({ 128, 128, 128 }),
	({ 128, 16, 16 }),
});
*/
