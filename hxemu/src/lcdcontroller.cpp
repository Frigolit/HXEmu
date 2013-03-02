// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @info    http://frigolit.net/archive/datasheets/PD7227.pdf
// =============================================================================

#include <stdio.h>
#include <stdlib.h>

#include "lcdcontroller.h"
#include "lcd.h"

CLCDController::CLCDController() {
	lcd = NULL;
	ram = (uint8_t *)malloc(128);
	
	reset();
}

CLCDController::~CLCDController() {
	free(ram);
}

void CLCDController::reset() {
	memset(ram, 0xFF, 128);
	ptr = 0;
	ptrop = 0;
	mode = 0;
}

void CLCDController::set_lcd(CLCD *l, uint8_t x, uint8_t y) {
	lcd = l;
	lcd_x = x;
	lcd_y = y;
}

void CLCDController::apply_ptr_op(uint8_t n) {
	if (n == 0) ++ptr;
	else if (n == 1) --ptr;
}

void CLCDController::command(uint8_t n) {
	if (n & 0x80) {
		// Load Data Pointer (LDPI)
		ptr = n & 0x7F;
		#ifdef DEBUG_LCD
			printf("CLCDController::command(0x%02X): Load Data Pointer (LDPI): ptr = 0x%02X\n", n, ptr);
		#endif
	}
	else if (n >= 0x10 && n <= 0x17) {
		// Set Frame Frequency (SFF)
		// Ignored!
		#ifdef DEBUG_LCD
			printf("CLCDController::command(0x%02X): Set Frame Frequency (SFF)\n", n);
		#endif
	}
	else if (n >= 0x18 && n <= 0x1F) {
		// Set Multiplexing Mode (SMM)
		// Ignored!
		#ifdef DEBUG_LCD
			printf("CLCDController::command(0x%02X): Set Multiplexing Mode (SMM)\n", n);
		#endif
	}
	else if (n == 0x08) {
		// Display Off (DISP OFF)
		#ifdef DEBUG_LCD
			printf("CLCDController::command(0x%02X): Display Off (DISP OFF)\n", n);
		#endif
	}
	else if (n == 0x09) {
		// Display On (DISP ON)
		#ifdef DEBUG_LCD
			printf("CLCDController::command(0x%02X): Display On (DISP OFF)\n", n);
		#endif
	}
	else if (n >= 0x60 && n <= 0x63) {
		// Set Read Mode (SRM)
		mode = 1;
		ptrop = n & 0x03;
		
		#ifdef DEBUG_LCD
			printf("CLCDController::command(0x%02X): Set Read Mode (SRM)\n", n);
		#endif
	}
	else if (n >= 0x64 && n <= 0x67) {
		// Set Write Mode (SWM)
		mode = 2;
		ptrop = n & 0x03;
		
		#ifdef DEBUG_LCD
			printf("CLCDController::command(0x%02X): Set Write Mode (SWM)\n", n);
		#endif
	}
	else if (n >= 0x68 && n <= 0x6B) {
		// Set OR Mode (SORM)
		mode = 3;
		ptrop = n & 0x03;
		
		#ifdef DEBUG_LCD
			printf("CLCDController::command(0x%02X): Set OR Mode (SORM)\n", n);
		#endif
	}
	else if (n >= 0x6C && n <= 0x6F) {
		// Set AND Mode (SANDM)
		mode = 4;
		ptrop = n & 0x03;
		
		#ifdef DEBUG_LCD
			printf("CLCDController::command(0x%02X): Set AND Mode (SANDM)\n", n);
		#endif
	}
	else if (n == 0x72) {
		// Set Character Mode (SCM)
		mode = 5;
		
		#ifdef DEBUG_LCD
			printf("CLCDController::command(0x%02X): Set Character Mode (SCM)\n", n);
		#endif
	}
	else if (n >= 0x40 && n <= 0x5F) {
		#ifdef DEBUG_LCD
			printf("CLCDController::command(0x%02X): Bit Set\n", n);
		#endif
		
		// Bit set
		if (lcd && (ptr & 0x3F) < 40) {
			uint8_t bt = (n & 0x1C) >> 2;
			
			uint8_t yb = (ptr & 0x40) >> 3;
			uint8_t x = ptr & 0x3F;
			
			uint8_t n = (ram[ptr] |= (1 << bt));
			
			for (uint8_t y = 0; y < 8; y++) {
				if (n & (1 << y)) lcd->set_pixel(lcd_x + x, lcd_y + y + yb);
				else lcd->clear_pixel(lcd_x + x, lcd_y + y + yb);
			}
		}
		
		apply_ptr_op(n & 0x03);
	}
	else if (n >= 0x20 && n <= 0x3F) {
		#ifdef DEBUG_LCD
			printf("CLCDController::command(0x%02X): Bit Clear\n", n);
		#endif
		
		// Bit clear
		if (lcd && (ptr & 0x3F) < 40) {
			uint8_t bt = (n & 0x1C) >> 2;
			
			uint8_t yb = (ptr & 0x40) >> 3;
			uint8_t x = ptr & 0x3F;
			
			uint8_t n = (ram[ptr] &= ~(1 << bt));
			
			for (uint8_t y = 0; y < 8; y++) {
				if (n & (1 << y)) lcd->set_pixel(lcd_x + x, lcd_y + y + yb);
				else lcd->clear_pixel(lcd_x + x, lcd_y + y + yb);
			}
		}
		
		apply_ptr_op(n & 0x03);
	}
}

void CLCDController::data(uint8_t n) {
	if (mode == 1) {
		// Read mode
		printf("CLCDController()->data(0x%02X): fixme: read mode is not yet implemented\n", n);
	}
	else if (mode == 2) {
		// Write mode
		if (lcd && (ptr & 0x3F) < 40) {
			uint8_t yb = (ptr & 0x40) >> 3;
			uint8_t x = ptr & 0x3F;
			
			for (uint8_t y = 0; y < 8; y++) {
				if (n & (1 << y)) lcd->set_pixel(lcd_x + x, lcd_y + y + yb);
				else lcd->clear_pixel(lcd_x + x, lcd_y + y + yb);
			}
			
			ram[ptr] = n;
		}
		
		apply_ptr_op(ptrop);
	}
	else if (mode == 3) {
		// OR mode
		printf("CLCDController()->data(0x%02X): fixme: or mode is not yet implemented\n", n);
	}
	else if (mode == 4) {
		// AND mode
		printf("CLCDController()->data(0x%02X): fixme: and mode is not yet implemented\n", n);
	}
	else if (mode == 5) {
		// Character mode
		printf("CLCDController()->data(0x%02X): fixme: character mode is not yet implemented\n", n);
	}
}

