// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#include <stdio.h>

#include "iocontroller.h"

CIOController::CIOController() {
	memset(&keyboard_map, 0, 256);
	
	for (int i = 0; i < 6; i++) {
		lcd_ctls[i] = NULL;
	}
	
	lcd_clk_counter = 0;
	
	r_9g = 0x00;
}

CIOController::~CIOController() {
	for (int i = 0; i < 6; i++) {
		lcd_ctls[i] = NULL;
	}
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
	#ifdef DEBUG_IOCTL
		printf("CIOController::read(0x%04X) = 0x", addr);
	#endif
	
	if (addr == 0x20) {
		#ifdef DEBUG_IOCTL
			printf("%02X\n", 0x08);
		#endif
		return 0x08;
	}
	else if (addr == 0x22) {
		// KRTN 0-7
		uint8_t l = ~ram[0x20];
		uint8_t n = 0xFF;
		
		if (l & 0x01) n ^= keyboard_map['0'] | (keyboard_map['1'] << 1) | (keyboard_map['2'] << 2) | (keyboard_map['3'] << 3) | (keyboard_map['4'] << 4) | (keyboard_map['5'] << 5) | (keyboard_map['6'] << 6) | (keyboard_map['7'] << 7);
		if (l & 0x02) n ^= keyboard_map['8'] | (keyboard_map['9'] << 1) | (keyboard_map[':'] << 2) | (keyboard_map[';'] << 3) | (keyboard_map[','] << 4) | (keyboard_map['-'] << 5) | (keyboard_map['.'] << 6) | (keyboard_map['/'] << 7);
		if (l & 0x04) n ^= keyboard_map['@'] | (keyboard_map['a'] << 1) | (keyboard_map['b'] << 2) | (keyboard_map['c'] << 3) | (keyboard_map['d'] << 4) | (keyboard_map['e'] << 5) | (keyboard_map['f'] << 6) | (keyboard_map['g'] << 7);
		if (l & 0x08) n ^= keyboard_map['h'] | (keyboard_map['i'] << 1) | (keyboard_map['j'] << 2) | (keyboard_map['k'] << 3) | (keyboard_map['l'] << 4) | (keyboard_map['m'] << 5) | (keyboard_map['n'] << 6) | (keyboard_map['o'] << 7);
		if (l & 0x10) n ^= keyboard_map['p'] | (keyboard_map['q'] << 1) | (keyboard_map['r'] << 2) | (keyboard_map['s'] << 3) | (keyboard_map['t'] << 4) | (keyboard_map['u'] << 5) | (keyboard_map['v'] << 6) | (keyboard_map['w'] << 7);
		if (l & 0x20) n ^= keyboard_map['x'] | (keyboard_map['y'] << 1) | (keyboard_map['z'] << 2) | (keyboard_map['['] << 3) | (keyboard_map[']'] << 4) | (keyboard_map['\\'] << 5) | (keyboard_map[0] << 6) | (keyboard_map[0] << 7);
		if (l & 0x40) n ^= keyboard_map['\n'] | (keyboard_map[' '] << 1) | (keyboard_map['\t'] << 2) | (keyboard_map[0] << 3) | (keyboard_map[0] << 4) | (keyboard_map[0] << 5) | (keyboard_map[0] << 6) | (keyboard_map[0] << 7);
		if (l & 0x80) n ^= keyboard_map[0] | (keyboard_map[0] << 1) | (keyboard_map[0] << 2) | (keyboard_map[0] << 3) | (keyboard_map[0] << 4) | (keyboard_map[0] << 5) | (keyboard_map[0] << 6) | (keyboard_map[0] << 7);
		
		#ifdef DEBUG_IOCTL
			printf("%02X\n", n);
		#endif
		return n;
	}
	else if (addr == 0x26) {
		#ifdef DEBUG_IOCTL
			printf("FF\n");
		#endif
		return 0xFF;
	}
	else if (addr == 0x28) {
		bool b_power = true;
		bool b_busy = true;
		
		uint8_t l = ~ram[0x20];
		uint8_t k = 0x03;
		
		if (l & 0x01) k ^= 0x00;
		if (l & 0x02) k ^= 0x00;
		if (l & 0x04) k ^= 0x20;
		if (l & 0x08) k ^= 0x00;
		if (l & 0x10) k ^= 0x00;
		if (l & 0x20) k ^= (keyboard_map[0x00] | keyboard_map[0x01]) << 1;	// Shift
		if (l & 0x40) k ^= 0x00;
		if (l & 0x80) k ^= 0x00;
		
		uint8_t n = (b_power << 7) | (b_busy << 6) | k;
		
		#ifdef DEBUG_IOCTL
			printf("%02X\n", n);
		#endif
		return n;
	}
	else if (addr == 0x2A || addr == 0x2B) {
		if (++lcd_clk_counter == 8) {
			lcd_clk_counter = 0;
			
			if (r_9g & 0x07) {
				uint8_t n = (r_9g & 0x07) - 1;
				
				if (r_9g & 0x08) lcd_ctls[n]->command(ram[0x2A]);
				else lcd_ctls[n]->data(ram[0x2A]);
			}
		}
	}
	else if (addr == 0x2C) {
		//return 0x00;
	}
	
	#ifdef DEBUG_IOCTL
		printf("00\n");
	#endif
	return 0x00;
}

void CIOController::write(uint16_t addr, uint8_t data) {
	#ifdef DEBUG_IOCTL
		printf("CIOController::write(0x%04X, 0x%02X)\n", addr, data);
	#endif
	
	if (addr == 0x26) r_9g = data;
	else ram[addr] = data;
}

