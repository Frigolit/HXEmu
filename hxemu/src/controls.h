#ifndef __CONTROLS_H__
#define __CONTROLS_H__

#include "hx20.h"
#include "ui.h"
#include <vector>

using namespace std;

class CHX20;

class CControls {
	public:
		CControls(CHX20*);
		~CControls();

		void render(SDL_Surface *target, int x, int y);
		void mousedown(int x, int y);
		void mouseup(int x, int y);

	private:
		CHX20            *machine;
		SDL_Surface      *surface;
		vector<CWidget*> *widgets;

		bool modified;

		void cb_btn_reset(CWidget *widget);
		void cb_btn_power(CWidget *widget);
		void cb_btn_monitor(CWidget *widget);
		void cb_btn_trace(CWidget *widget);
};

#endif

