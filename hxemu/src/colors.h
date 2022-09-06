#ifndef __COLORS_H__
#define __COLORS_H__

struct RgbColor {
	int r;
	int g;
	int b;
};

struct HsvColor {
	float h;	// Hue:        0.0 - 360.0
	float s;	// Saturation: 0.0 - 1.0
	float v;	// Value:      0.0 - 1.0
};

void hsv_to_rgb(HsvColor &hsv, RgbColor &rgb);
void rgb_to_hsv(RgbColor &rgb, HsvColor &hsv);

#endif
