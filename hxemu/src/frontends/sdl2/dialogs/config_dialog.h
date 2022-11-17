#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_DIALOGS_CONFIG_DIALOG
#define HXEMU_FRONTENDS_SDL2_DIALOGS_CONFIG_DIALOG

#include <functional>

#include "../widgets/widget.h"
#include "../fonts.h"
#include "../../../hx20.h"

class ConfigDialog : public CWidget {
	public:
		ConfigDialog(CHX20 *hx20, int x, int y) {
			this->hx20 = hx20;

			CWidget::x = x;
			CWidget::y = y;
			CWidget::w = 992;
			CWidget::h = 528;

			surface = SDL_CreateRGBSurface(SDL_RLEACCEL, w, h, 32, 0, 0, 0, 0);
			SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xC0, 0xC0, 0xC0));
		}

		void draw_dialog_title(const char *title) {
			SDL_Rect bg_rect;
			bg_rect.x = 0;
			bg_rect.y = 0;
			bg_rect.w = w;
			bg_rect.h = 48;
			SDL_FillRect(surface, &bg_rect, SDL_MapRGB(surface->format, 0x4a, 0x82, 0x52));

			// Draw texts
			SDL_Color text_color {
				.r = 0,
				.g = 96,
				.b = 0
			};

			SDL_Color text_color2 {
				.r = 255,
				.g = 255,
				.b = 255
			};

			SDL_Surface *surf_text;
			SDL_Rect rect_text;

			// Title
			surf_text = TTF_RenderUTF8_Blended_Wrapped(font_dialog_title, title, text_color, w);

			rect_text.w = surf_text->w;
			rect_text.h = surf_text->h;
			rect_text.x = (int)(((float)w / 2.0) - ((float)rect_text.w / 2.0));
			rect_text.y = 15;

			SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
			SDL_FreeSurface(surf_text);

			surf_text = TTF_RenderUTF8_Blended_Wrapped(font_dialog_title, title, text_color2, w);
			rect_text.y = 16;

			SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
			SDL_FreeSurface(surf_text);
		}

	protected:
		CHX20 *hx20;

		SDL_Color text_color {
			.r = 32,
			.g = 32,
			.b = 32
		};
};

#endif
#endif
