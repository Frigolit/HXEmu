#include <math.h>

#include "colors.h"

void hsv_to_rgb(HsvColor &hsv, RgbColor &rgb) {
	float c = hsv.v * hsv.s;
	float x = c * (1.0 - fabsf(fmodf(fmodf(hsv.h / 60.0, 6.0), 2.0) - 1.0));
	float m = hsv.v - c;

	float r = 0.0, g = 0.0, b = 0.0;
	float h = hsv.v;

	if (h < 60.0) {
		r = c;
		g = x;
	}
	else if (h < 120.0) {
		r = x;
		g = c;
	}
	else if (h < 180.0) {
		g = c;
		b = x;
	}
	else if (h < 240.0) {
		g = x;
		b = c;
	}
	else if (h < 300.0) {
		r = x;
		b = c;
	}
	else if (h < 360.0) {
		r = c;
		b = x;
	}

	rgb.r = (int)((r + m) * 255.0);
	rgb.g = (int)((g + m) * 255.0);
	rgb.b = (int)((b + m) * 255.0);
}

void rgb_to_hsv(RgbColor &rgb, HsvColor &hsv) {
	float r = rgb.r / 255.0;
	float g = rgb.g / 255.0;
	float b = rgb.b / 255.0;

	float ch = fmaxf(fmaxf(r, g), b);
	float cl = fminf(fminf(r, g), b);
	float d = ch - cl;

	if (d > 0.0) {
		// Hue
		if (r >= ch) {
			hsv.h = fmodf(((g - b) / d), 6.0);
		}
		else if (g >= ch) {
			hsv.h = 2.0 + ((b - r) / d);
		}
		else if (b >= ch) {
			hsv.h = 4.0 + ((r - g) / d);
		}

		hsv.h *= 60.0;

		// Saturation
		if (ch > 0.0) {
			hsv.s = d / ch;
		}
		else {
			hsv.s = 0.0;
		}

		// Value
		hsv.v = ch;

		if (hsv.h < 0.0) {
			hsv.h += 360.0;
		}
	}
	else {
		hsv.h = 0.0;
		hsv.s = 0.0;
		hsv.v = ch;
	}
}
