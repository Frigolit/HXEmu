// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: iocontroller.cpp 167 2012-06-05 15:29:03Z frigolit $
// =============================================================================

#include <stdio.h>

#include "iocontroller.h"

CIOController::CIOController() {
	memset(&keyboard_map, 0, 256);
	
	for (int i = 0; i < 6; i++) {
		lcd_ctls[i] = NULL;
	}
	
	lcd_clk_counter = 0;
}

CIOController::~CIOController() {
	
}

void CIOController::reset() {
	lcd_clk_counter = 0;
	for (int i = 0; i < 6; i++) {
		lcd_ctls[i]->reset();
	}
}

void CIOController::set_lcd_controller(uint8_t n, CLCDController *c) {
	lcd_ctls[n] = c;
}

uint8_t CIOController::read(uint16_t addr) {
	//if (addr != 0x2A && addr != 0x2B) printf("CIOController::read(0x%04X)\n", addr);
	
	if (addr == 0x20) {
		return 0b00001000;
	}
	else if (addr == 0x22) {
		uint8_t l = ~ram[0x20];
		uint8_t n = 0;
		
		if (l & 0x01) {
			n |= keyboard_map['0'] | (keyboard_map['1'] << 1) | (keyboard_map['2'] << 2) | (keyboard_map['3'] << 3);
		}
		
		//printf("0x22 = %02X\n", n);
		return n;
	}
	else if (addr == 0x26) {
		return ram[0x26];
	}
	else if (addr == 0x28) {
		return 0b11000000;
	}
	else if (addr == 0x2A || addr == 0x2B) {
		if (++lcd_clk_counter == 8) {
			lcd_clk_counter = 0;
			
			if (ram[0x26] & 0b00000111) {
				uint8_t n = (ram[0x26] & 0b00000111) - 1;
				
				if (ram[0x26] & 0b00001000) lcd_ctls[n]->command(ram[0x2A]);
				else lcd_ctls[n]->data(ram[0x2A]);
			}
		}
	}
	else if (addr == 0x2C) {
		return 0x00;
	}
	
	return 0x00;
}

void CIOController::write(uint16_t addr, uint8_t data) {
	ram[addr] = data;
}

