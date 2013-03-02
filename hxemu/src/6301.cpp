// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: 6301.cpp 168 2012-07-23 20:40:29Z frigolit $
// =============================================================================

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "config.h"
#include "6301.h"

C6301::C6301() {
	// Initialize base stuff
	membus       = new CMemoryBus();
	serial0      = new VirtualSerial();
	ram          = new CRAM(128);
	maskrom      = new CROM(4096);
	
	b_inited     = false;
	b_trace      = false;
	
	r_port1_data = 0;
	r_port1_ddr  = 0;
	r_port2_data = 0;
	r_port2_ddr  = 0;
	
	tracefile    = NULL;
	//tracefile = fopen("cputrace.log", "w");
}

C6301::~C6301() {
	delete(membus);
	delete(serial0);
	delete(ram);
}

void C6301::reset() {
	// Un-halt CPU
	b_halted = false;
	b_sleep = false;
	
	// Reset registers
	r_a  = 0;
	r_b  = 0;
	r_x  = 0;
	r_sp = 0;
	
	r_ccr_i = 1;
	r_ccr_n = 0;
	r_ccr_v = 0;
	r_ccr_z = 0;
	r_ccr_h = 0;
	r_ccr_c = 0;
	
	r_counter = 0;
	r_ocr     = 0xFFFF;
	r_tcsr    = 0;

	// Reset ports
	r_port1_ddr  = 0;
	r_port1_data = 0;
	r_port2_ddr  = 0;
	r_port2_data = 0;
	
	// Reset internal states
	b_inited     = 0;
	b_rdrf_clear = 0;
	b_waitirq    = 0;
	b_nmi        = false;
	b_nmi_enter  = false;
	b_irq_swi    = false;
	
	// Initialize internal registers
	memset(&r_internal, 0, 32);
	r_internal[0x11] |= 0x20;
	r_internal[0x14] |= (1 << 6);
	
	// Reset serial
	serial0->reset();
}

void C6301::jump(uint16_t addr) {
	printf("C6301::jump(0x%04X)\n", addr);
	r_pc = addr;
	b_halted = false;
	b_sleep = false;
}

void C6301::nmi() {
	b_nmi_enter = true;
}

void C6301::step() {
	if (!b_inited) {
		opmode = r_port2_data >> 5;
		printf("C6301::step(): debug: opmode = %d\n", opmode);

		// Load reset vector
		r_pc = (memread(0xFFFE) << 8) | memread(0xFFFF);

		b_inited = true;
		return;
	}

	if (b_halted) return;
	
	// Increase free running counter
	++r_counter;
	
	// Interrupts
	if (b_nmi_enter) {
		if (b_sleep) b_sleep = false;
		b_nmi_enter = false;
		interrupt_enter(0xFFFC, 0xFFFD);
	}
	else if (b_irq_swi) {
		b_irq_swi = false;
		if (!r_ccr_i) interrupt_enter(0xFFFA, 0xFFFB);
	}
	else if (b_irq1) {
		if (b_sleep) b_sleep = false;
		if (!r_ccr_i) interrupt_enter(0xFFF8, 0xFFF9);
	}
	
	// Check timer against OCR (Output Compare Register)
	if (r_ocr == r_counter) {
		r_tcsr |= 0x40;
		if (r_tcsr & 0x08) {
			if (b_sleep) b_sleep = false;
			
			if (!r_ccr_i) {
				printf("C6301::step(): debug: triggering OCR interrupt\n");
				interrupt_enter(0xFFF4, 0xFFF5);
			}
		}
	}
	
	// Check timer overflow
	if (!r_counter) {
		r_tcsr |= 0x20;
		if (r_tcsr & 0x04) {
			if (b_sleep) b_sleep = false;
			
			if (!r_ccr_i) {
				printf("C6301::step(): debug: triggering TOF interrupt\n");
				interrupt_enter(0xFFF2, 0xFFF3);
			}
		}
	}
	
	// Process serial
	if (serial0->peek()) {
		if (!(r_internal[0x11] & 0x80)) {
			r_internal[0x11] |= 0x80;
			r_internal[0x12] = serial0->recv();
			b_rdrf_clear = 0;
		}
	}
	
	// Sleeping?
	if (b_sleep) return;
	
	// Checks for errors in the CCR-register which would indicate that my code is broken :P
	if (r_ccr_i > 1) { printf("\e[31m\e[1mC6301::step(): fixme: ccr_i > 1\e[0m\n"); exit(1); }
	if (r_ccr_n > 1) { printf("\e[31m\e[1mC6301::step(): fixme: ccr_n > 1\e[0m\n"); exit(1); }
	if (r_ccr_v > 1) { printf("\e[31m\e[1mC6301::step(): fixme: ccr_v > 1\e[0m\n"); exit(1); }
	if (r_ccr_z > 1) { printf("\e[31m\e[1mC6301::step(): fixme: ccr_z > 1\e[0m\n"); exit(1); }
	if (r_ccr_h > 1) { printf("\e[31m\e[1mC6301::step(): fixme: ccr_h > 1\e[0m\n"); exit(1); }
	if (r_ccr_c > 1) { printf("\e[31m\e[1mC6301::step(): fixme: ccr_c > 1\e[0m\n"); exit(1); }
	
	// Fetch next instruction
	uint8_t op = get_next_byte();
	
	if (b_trace && tracefile != NULL) {
		char buf[255];
		sprintf(buf, "A=%02X B=%02X X=%04X SP=%04X PC=%04X CCR=%c%c%c%c%c%c OP=%02X\n", r_a, r_b, r_x, r_sp, r_pc - 1, (r_ccr_h ? 'H' : 'h'), (r_ccr_i ? 'I' : 'i'), (r_ccr_n ? 'N' : 'n'), (r_ccr_z ? 'Z' : 'z'), (r_ccr_v ? 'V' : 'v'), (r_ccr_c ? 'C' : 'c'), op);
		fputs(buf, tracefile);
	}
	
	#ifdef DEBUG_CPU
		printf("\e[34m\e[1mC6301::step(): debug: === A: %02X, B: %02X, X: %04X, SP: %04X, PC: %04X, CCR: [%c%c%c%c%c%c], OP: %02X ==\e[0m\n", r_a, r_b, r_x, r_sp, r_pc - 1, (r_ccr_h ? 'H' : ' '), (r_ccr_i ? 'I' : ' '), (r_ccr_n ? 'N' : ' '), (r_ccr_z ? 'Z' : ' '), (r_ccr_v ? 'V' : ' '), (r_ccr_c ? 'C' : ' '), op);
	#else
		if (b_trace) printf("\e[34m\e[1mC6301::step(): debug: === A: %02X, B: %02X, X: %04X, SP: %04X, PC: %04X, CCR: [%c%c%c%c%c%c], OP: %02X ==\e[0m\n", r_a, r_b, r_x, r_sp, r_pc - 1, (r_ccr_h ? 'H' : ' '), (r_ccr_i ? 'I' : ' '), (r_ccr_n ? 'N' : ' '), (r_ccr_z ? 'Z' : ' '), (r_ccr_v ? 'V' : ' '), (r_ccr_c ? 'C' : ' '), op);
	#endif
	
	// Execute it~
	switch (op) {
		// === Low 0 ===
			case 0x10:	// SBA - Subtract Accumulators
				{
					uint8_t a = r_a;
					uint8_t b = r_b;
					uint8_t r = a - b;
					
					r_a = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((a & ~b & ~r) | (~a & b & r)) >> 7;
					r_ccr_c = ((~a & b) | (b & r) | (r & ~a)) >> 7;
				}
				break;
				
			case 0x20:	// BRA - Branch Always
				{
					int8_t n = get_next_byte();
					r_pc += n;
				}
				break;
				
			case 0x30:	// TSX - Transfer from Stack Pointer to Index Register
				r_x = r_sp + 1;
				break;
				
			case 0x40:	// NEG A - Negate Accumulator A
				{
					uint8_t r = (r_a == 0x80 ? 0x80 : -r_a);
					r_b = r;
				
					r_ccr_z = !r;
					r_ccr_v = (r == 0x80);
					r_ccr_c = !!r;
				}
				break;
			
			case 0x50:	// NEG B - Negate Accumulator B
				{
					uint8_t r = (r_b == 0x80 ? 0x80 : -r_b);
					r_b = r;
				
					r_ccr_z = !r;
					r_ccr_v = (r == 0x80);
					r_ccr_c = !!r;
				}
				break;
				
			case 0x60:	// NEG - Negate (indexed)
				{
					uint16_t a = r_x + get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = (m == 0x80 ? 0x80 : -m);
					memwrite(a, r);
				
					r_ccr_z = !r;
					r_ccr_v = (r == 0x80);
					r_ccr_c = !!r;
				}
				break;
				
			case 0x70:	// NEG - Negate (extended)
				{
					uint16_t a = (get_next_byte() << 8) | get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = (m == 0x80 ? 0x80 : -m);
					memwrite(a, r);
				
					r_ccr_z = !r;
					r_ccr_v = (r == 0x80);
					r_ccr_c = !!r;
				}
				break;
			
			case 0x80:	// SUB A - Subtract Accumulator A (immediate)
				{
					uint8_t m = get_next_byte();
					uint8_t x = r_a;
					uint8_t r = x - m;
					r_a = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0x90:	// SUB A - Subtract Accumulator A (direct)
				{
					uint8_t m = memread(get_next_byte());
					uint8_t x = r_a;
					uint8_t r = x - m;
					r_a = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xA0:	// SUB A - Subtract Accumulator A (indexed)
				{
					uint8_t m = memread(r_x + (uint16_t)get_next_byte());
					uint8_t x = r_a;
					uint8_t r = x - m;
					r_a = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xB0:	// SUB A - Subtract Accumulator A (extended)
				{
					uint8_t m = memread((get_next_byte() << 8) | get_next_byte());
					uint8_t x = r_a;
					uint8_t r = x - m;
					r_a = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
			
			case 0xC0:	// SUB B - Subtract Accumulator B (immediate)
				{
					uint8_t m = get_next_byte();
					uint8_t x = r_b;
					uint8_t r = x - m;
					r_b = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xD0:	// SUB B - Subtract Accumulator B (direct)
				{
					uint8_t m = memread(get_next_byte());
					uint8_t x = r_b;
					uint8_t r = x - m;
					r_b = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xE0:	// SUB B - Subtract Accumulator B (indexed)
				{
					uint8_t m = memread(r_x + (uint16_t)get_next_byte());
					uint8_t x = r_b;
					uint8_t r = x - m;
					r_b = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xF0:	// SUB B - Subtract Accumulator B (extended)
				{
					uint8_t m = memread((get_next_byte() << 8) | get_next_byte());
					uint8_t x = r_b;
					uint8_t r = x - m;
					r_b = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
		
		// === Low 1 ===
			case 0x01:	// NOP - No Operation
				break;
				
			case 0x11:	// CBA - Compare Accumulators
				{
					uint8_t a = r_a;
					uint8_t b = r_b;
					uint8_t r = a - b;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((a & ~b & ~r) | (~a & b & r)) >> 7;
					r_ccr_c = ((~a & b) | (b & r) | (r & ~a)) >> 7;
				}
				break;
			
			case 0x21:	// BRN - Branch Never
				{
					//int8_t n = (int8_t)get_next_byte();
					
					printf("C6301::step(): debug: BRANCH NEVAHR\n");
					get_next_byte();
					// Just read the byte, don't do anything :P
				}
				break;
				
			case 0x31:	// INS - Increment Stack Pointer
				++r_sp;
				break;
			
			case 0x61:	// AIM - AND Immediate (indexed)
				{
					uint8_t m = get_next_byte();
					uint16_t a = r_x + (uint16_t)get_next_byte();
					uint8_t r = m & memread(a);
					memwrite(a, r);
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
			
			case 0x71:	// AIM - AND Immediate (direct)
				{
					uint8_t m = get_next_byte();
					uint16_t a = (uint8_t)get_next_byte();
					uint8_t r = m & memread(a);
					memwrite(a, r);
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0x81:	// CMP - Compare and Test Accumulator A (immediate)
				{
					uint8_t x = r_a;
					uint8_t m = get_next_byte();
					
					uint8_t r = x - m;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0x91:	// CMP - Compare and Test Accumulator A (direct)
				{
					uint8_t x = r_a;
					uint8_t m = memread(get_next_byte());
					
					uint8_t r = x - m;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xA1:	// CMP - Compare and Test Accumulator A (indexed)
				{
					uint8_t x = r_a;
					uint8_t m = memread(r_x + get_next_byte());
					
					uint8_t r = x - m;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xB1:	// CMP - Compare and Test Accumulator A (extended)
				{
					uint8_t x = r_a;
					uint8_t m = memread((get_next_byte() << 8) | get_next_byte());
					
					uint8_t r = x - m;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xC1:	// CMP - Compare and Test Accumulator B (immediate)
				{
					uint8_t x = r_b;
					uint8_t m = get_next_byte();
					
					uint8_t r = x - m;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xD1:	// CMP - Compare and Test Accumulator B (direct)
				{
					uint8_t x = r_b;
					uint8_t m = memread(get_next_byte());
					
					uint8_t r = x - m;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xE1:	// CMP - Compare and Test Accumulator B (indexed)
				{
					uint8_t x = r_b;
					uint8_t m = memread(r_x + get_next_byte());
					
					uint8_t r = x - m;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xF1:	// CMP - Compare and Test Accumulator B (extended)
				{
					uint8_t x = r_b;
					uint8_t m = memread((get_next_byte() << 8) | get_next_byte());
					
					uint8_t r = x - m;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
		
		// === Low 2 ===
			case 0x22:	// BHI - Branch if Higher
				{
					int8_t n = (int8_t)get_next_byte();
					
					if (!(r_ccr_c | r_ccr_z)) {
						r_pc += n;
					}
				}
				break;
				
			case 0x32:	// PUL A - Pop Accumulator A from Stack
				r_a = stack_pop();
				break;
			
			case 0x62:	// OIM - OR Immediate (indexed)
				{
					uint8_t n = get_next_byte();
					uint16_t a = r_x + (uint16_t)get_next_byte();
					uint8_t r = memread(a) | n;
					memwrite(a, r);
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
			
			case 0x72:	// OIM - OR Immediate (direct)
				{
					uint8_t n = get_next_byte();
					uint16_t a = (uint16_t)get_next_byte();
					uint8_t r = memread(a) | n;
					memwrite(a, r);
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0x82:	// SBC A - Subtract Accumulator A with Carry (immediate)
				{
					uint8_t m = get_next_byte();
					uint8_t x = r_a;
					uint8_t r = x - m - r_ccr_c;
					r_a = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0x92:	// SBC A - Subtract Accumulator A with Carry (direct)
				{
					uint8_t m = memread(get_next_byte());
					uint8_t x = r_a;
					uint8_t r = x - m - r_ccr_c;
					r_a = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xA2:	// SBC A - Subtract Accumulator A with Carry (indexed)
				{
					uint8_t m = memread(r_x + (uint16_t)get_next_byte());
					uint8_t x = r_a;
					uint8_t r = x - m - r_ccr_c;
					r_a = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xB2:	// SBC A - Subtract Accumulator A with Carry (extended)
				{
					uint8_t m = memread((get_next_byte() << 8) | get_next_byte());
					uint8_t x = r_a;
					uint8_t r = x - m - r_ccr_c;
					r_a = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
			
			case 0xC2:	// SBC B - Subtract Accumulator B with Carry (immediate)
				{
					uint8_t m = get_next_byte();
					uint8_t x = r_b;
					uint8_t r = x - m - r_ccr_c;
					r_b = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xD2:	// SBC B - Subtract Accumulator B with Carry (direct)
				{
					uint8_t m = memread(get_next_byte());
					uint8_t x = r_b;
					uint8_t r = x - m - r_ccr_c;
					r_b = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xE2:	// SBC B - Subtract Accumulator B with Carry (indexed)
				{
					uint8_t m = memread(r_x + (uint16_t)get_next_byte());
					uint8_t x = r_b;
					uint8_t r = x - m - r_ccr_c;
					r_b = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
				
			case 0xF2:	// SBC B - Subtract Accumulator B with Carry (extended)
				{
					uint8_t m = memread((get_next_byte() << 8) | get_next_byte());
					uint8_t x = r_b;
					uint8_t r = x - m - r_ccr_c;
					r_b = r;
			
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 7;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 7;
				}
				break;
		
		// === Low 3 ===
			case 0x23:	// BLS - Branch if Lower or Same
				{
					int8_t n = (int8_t)get_next_byte();
					
					if (r_ccr_c | r_ccr_z) {
						r_pc += n;
					}
				}
				break;
				
			case 0x33:	// PUL B - Pop Accumulator B from Stack
				r_b = stack_pop();
				break;
				
			case 0x43:	// COM A - Complement Accumulator A
				r_a = ~r_a;
				r_ccr_n = r_a >> 7;
				r_ccr_z = !r_a;
				r_ccr_v = 0;
				r_ccr_c = 1;
				break;
				
			case 0x53:	// COM B - Complement Accumulator B
				r_b = ~r_b;
				r_ccr_n = r_b >> 7;
				r_ccr_z = !r_b;
				r_ccr_v = 0;
				r_ccr_c = 1;
				break;
				
			case 0x63:	// COM - Complement (indexed)
				{
					uint16_t a = r_x + get_next_byte();
					uint8_t r = ~memread(a);
					memwrite(a, r);
				
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
					r_ccr_c = 1;
				}
				break;
				
			case 0x73:	// COM - Complement (extended)
				{
					uint16_t a = (get_next_byte() << 8) | get_next_byte();
					uint8_t r = ~memread(a);
					memwrite(a, r);
				
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
					r_ccr_c = 1;
				}
				break;
				
			case 0x83:	// SUBD - Double Subtract without Carry (immediate)
				{
					uint16_t m  = (get_next_byte() << 8) | get_next_byte();
					uint16_t ab = (r_a << 8) | r_b;
					uint16_t r  = ab - m;
					
					r_a = r >> 8;
					r_b = r & 0x00FF;
					
					r_ccr_n = r >> 15;
					r_ccr_z = !r;
					r_ccr_v = ((ab & ~m & ~r) | (~ab & m & r)) >> 15;
					r_ccr_c = ((~ab & m) | (m & r) | (r & ~ab)) >> 15;
				}
				break;
				
			case 0x93:	// SUBD - Double Subtract without Carry (direct)
				{
					uint16_t m  = memread_double((uint16_t)get_next_byte());
					uint16_t ab = (r_a << 8) | r_b;
					uint16_t r  = ab - m;
					
					r_a = r >> 8;
					r_b = r & 0x00FF;
					
					r_ccr_n = r >> 15;
					r_ccr_z = !r;
					r_ccr_v = ((ab & ~m & ~r) | (~ab & m & r)) >> 15;
					r_ccr_c = ((~ab & m) | (m & r) | (r & ~ab)) >> 15;
				}
				break;
				
			case 0xA3:	// SUBD - Double Subtract without Carry (indexed)
				{
					uint16_t m  = memread_double((uint16_t)get_next_byte() + r_x);
					uint16_t ab = (r_a << 8) | r_b;
					uint16_t r  = ab - m;
					
					r_a = r >> 8;
					r_b = r & 0x00FF;
					
					r_ccr_n = r >> 15;
					r_ccr_z = !r;
					r_ccr_v = ((ab & ~m & ~r) | (~ab & m & r)) >> 15;
					r_ccr_c = ((~ab & m) | (m & r) | (r & ~ab)) >> 15;
				}
				break;
				
			case 0xB3:	// SUBD - Double Subtract without Carry (extended)
				{
					uint16_t m  = memread_double((get_next_byte() << 8) | get_next_byte());
					uint16_t ab = (r_a << 8) | r_b;
					uint16_t r  = ab - m;
					
					r_a = r >> 8;
					r_b = r & 0x00FF;
					
					r_ccr_n = r >> 15;
					r_ccr_z = !r;
					r_ccr_v = ((ab & ~m & ~r) | (~ab & m & r)) >> 15;
					r_ccr_c = ((~ab & m) | (m & r) | (r & ~ab)) >> 15;
				}
				break;
				
			case 0xC3:	// ADDD - Double Add without Carry (immediate)
				{
					uint16_t m  = (get_next_byte() << 8) | get_next_byte();
					uint16_t ab = (r_a << 8) | r_b;
					uint16_t r  = m + ab;
					
					r_a = r >> 8;
					r_b = r & 0x00FF;
					
					r_ccr_n = r >> 15;
					r_ccr_z = !r;
					r_ccr_v = ((ab & m & ~r) | (~ab & ~m & r)) >> 15;
					r_ccr_c = ((ab & m) | (m & ~r) | (~r & ab)) >> 15;
				}
				break;
				
			case 0xD3:	// ADDD - Double Add without Carry (direct)
				{
					uint16_t m  = memread_double((uint16_t)get_next_byte());
					uint16_t ab = (r_a << 8) | r_b;
					uint16_t r  = m + ab;
					
					r_a = r >> 8;
					r_b = r & 0x00FF;
					
					r_ccr_n = r >> 15;
					r_ccr_z = !r;
					r_ccr_v = ((ab & m & ~r) | (~ab & ~m & r)) >> 15;
					r_ccr_c = ((ab & m) | (m & ~r) | (~r & ab)) >> 15;
				}
				break;
				
			case 0xE3:	// ADDD - Double Add without Carry (indexed)
				{
					uint16_t m  = memread_double((uint16_t)get_next_byte() + r_x);
					uint16_t ab = (r_a << 8) | r_b;
					uint16_t r  = m + ab;
					
					r_a = r >> 8;
					r_b = r & 0x00FF;
					
					r_ccr_n = r >> 15;
					r_ccr_z = !r;
					r_ccr_v = ((ab & m & ~r) | (~ab & ~m & r)) >> 15;
					r_ccr_c = ((ab & m) | (m & ~r) | (~r & ab)) >> 15;
				}
				break;
				
			case 0xF3:	// ADDD - Double Add without Carry (extended)
				{
					uint16_t m  = memread_double((get_next_byte() << 8) | get_next_byte());
					uint16_t ab = (r_a << 8) | r_b;
					uint16_t r  = m + ab;
					
					r_a = r >> 8;
					r_b = r & 0x00FF;
					
					r_ccr_n = r >> 15;
					r_ccr_z = !r;
					r_ccr_v = ((ab & m & ~r) | (~ab & ~m & r)) >> 15;
					r_ccr_c = ((ab & m) | (m & ~r) | (~r & ab)) >> 15;
				}
				break;
		
		// === Low 4 ===
			case 0x04:	// LSRD - Logical Shift Double Right
				{
					uint16_t d = (r_a << 8) | r_b;
					uint16_t r = d >> 1;
					
					r_ccr_n = 0;
					r_ccr_z = !r;
					r_ccr_c = d & 1;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
					
					r_a = (uint8_t)(r >> 8);
					r_b = r & 0x00FF;
				}
				break;
				
			case 0x24:	// BCC - Branch if Carry Clear
				{
					int8_t n = (int8_t)get_next_byte();
					
					if (!r_ccr_c) {
						r_pc += n;
					}
				}
				break;
				
			case 0x34:	// DES - Decrement Stack Pointer
				r_sp--;
				break;
				
			case 0x44:	// LSR A - Logical Shift Right Accumulator A
				{
					r_ccr_c = r_a & 1;
					r_a = r_a >> 1;
					
					r_ccr_n = 0;
					r_ccr_z = (r_a == 0);
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
			
			case 0x54:	// LSR B - Logical Shift Right Accumulator B
				{
					r_ccr_c = r_b & 1;
					r_b = r_b >> 1;
					
					r_ccr_n = 0;
					r_ccr_z = !r_b;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x64:	// LSR - Logical Shift Right (indexed)
				{
					uint16_t a = r_x + get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = m >> 1;
					memwrite(a, r);
					
					r_ccr_c = m & 1;
					r_ccr_n = 0;
					r_ccr_z = !r;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x74:	// LSR - Logical Shift Right (extended)
				{
					uint16_t a = (get_next_byte() << 8) | get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = m >> 1;
					memwrite(a, r);
					
					r_ccr_c = m & 1;
					r_ccr_n = 0;
					r_ccr_z = !r;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x84:	// AND - Logical AND Accumulator A (immediate)
				r_a &= get_next_byte();
				
				r_ccr_n = r_a >> 7;
				r_ccr_z = !r_a;
				r_ccr_v = 0;
				break;
				
			case 0x94:	// AND - Logical AND Accumulator A (direct)
				r_a &= memread((uint16_t)get_next_byte());
				
				r_ccr_n = r_a >> 7;
				r_ccr_z = !r_a;
				r_ccr_v = 0;
				break;
				
			case 0xA4:	// AND - Logical AND Accumulator A (indexed)
				r_a &= memread(r_x + get_next_byte());
				
				r_ccr_n = r_a >> 7;
				r_ccr_z = !r_a;
				r_ccr_v = 0;
				break;
				
			case 0xB4:	// AND - Logical AND Accumulator A (extended)
				r_a &= memread((get_next_byte() << 8) | get_next_byte());
				
				r_ccr_n = r_a >> 7;
				r_ccr_z = !r_a;
				r_ccr_v = 0;
				break;
				
			case 0xC4:	// AND - Logical AND Accumulator B (immediate)
				r_b &= get_next_byte();
				
				r_ccr_n = r_b >> 7;
				r_ccr_z = !r_b;
				r_ccr_v = 0;
				break;
				
			case 0xD4:	// AND - Logical AND Accumulator B (direct)
				r_b &= memread((uint16_t)get_next_byte());
				
				r_ccr_n = r_b >> 7;
				r_ccr_z = !r_b;
				r_ccr_v = 0;
				break;
				
			case 0xE4:	// AND - Logical AND Accumulator B (indexed)
				r_b &= memread(r_x + get_next_byte());
				
				r_ccr_n = r_b >> 7;
				r_ccr_z = !r_b;
				r_ccr_v = 0;
				break;
				
			case 0xF4:	// AND - Logical AND Accumulator B (extended)
				r_b &= memread((get_next_byte() << 8) | get_next_byte());
				
				r_ccr_n = r_b >> 7;
				r_ccr_z = !r_b;
				r_ccr_v = 0;
				break;
		
		// === Low 5 ===
			case 0x05:	// ASLD - Arithmetic Shift Left Double
				{
					uint16_t d = (r_a << 8) | r_b;
					uint16_t r = d << 1;
					
					r_ccr_n = (uint8_t)(r >> 15);
					r_ccr_z = !r;
					r_ccr_c = (uint8_t)(d >> 15);
					
					r_ccr_v = r_ccr_n ^ r_ccr_c;
					
					r_a = (uint8_t)(r >> 8);
					r_b = r & 0x00FF;
				}
				break;
				
			case 0x25:	// BCS - Branch if Carry Set
				{
					int8_t n = (int8_t)get_next_byte();
					
					if (r_ccr_c) {
						r_pc += n;
					}
				}
				break;
				
			case 0x35:	// TXS - Transfer from Index Register to Stack Pointer
				r_sp = r_x - 1;
				break;
				
			case 0x65:	// EIM - XOR Immediate (indexed)
				{
					uint8_t n = get_next_byte();
					uint16_t a = r_x + (uint16_t)get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = m ^ n;
					memwrite(a, r);
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
			
			case 0x75:	// EIM - XOR Immediate (direct)
				{
					uint8_t n = get_next_byte();
					uint16_t a = (uint16_t)get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = m ^ n;
					memwrite(a, r);
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0x85:	// BIT - Bit Test Accumulator A (immediate)
				{
					uint8_t r = r_a & get_next_byte();
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0x95:	// BIT - Bit Test Accumulator A (direct)
				{
					uint8_t r = r_a & memread((uint16_t)get_next_byte());
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xA5:	// BIT - Bit Test Accumulator A (indexed)
				{
					uint8_t r = r_a & memread(r_x + (uint16_t)get_next_byte());
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xB5:	// BIT - Bit Test Accumulator A (extended)
				{
					uint8_t r = r_a & memread((get_next_byte() << 8) | get_next_byte());
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xC5:	// BIT - Bit Test Accumulator B (immediate)
				{
					uint8_t r = r_b & get_next_byte();
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xD5:	// BIT - Bit Test Accumulator B (direct)
				{
					uint8_t r = r_b & memread((uint16_t)get_next_byte());
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xE5:	// BIT - Bit Test Accumulator B (indexed)
				{
					uint8_t r = r_b & memread(r_x + (uint16_t)get_next_byte());
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xF5:	// BIT - Bit Test Accumulator B (extended)
				{
					uint8_t r = r_b & memread((get_next_byte() << 8) | get_next_byte());
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
		
		// === Low 6 ===
			case 0x06:	// TAP - Transfer from Accumulator A to CCR
				r_ccr_h = !!(r_a & 0x20);
				r_ccr_i = !!(r_a & 0x10);
				r_ccr_n = !!(r_a & 0x08);
				r_ccr_z = !!(r_a & 0x04);
				r_ccr_v = !!(r_a & 0x02);
				r_ccr_c = !!(r_a & 0x01);
				break;
				
			case 0x16:	// TAB - Transfer Accumulator A to Accumulator B
				r_b = r_a;
				r_ccr_n = r_b >> 7;
				r_ccr_z = !r_b;
				r_ccr_v = 0;
				break;
				
			case 0x26:	// BNE - Branch if Not Equal
				{
					int8_t n = (int8_t)get_next_byte();
					
					if (!r_ccr_z) {
						r_pc += n;
					}
				}
				break;
				
			case 0x36:	// PSH A - Push Accumulator A onto Stack
				stack_push(r_a);
				break;
			
			case 0x46:	// ROR A - Rotate Right Accumulator A
				{
					uint8_t m = r_a;
					uint8_t r = (m >> 1) | ((r_ccr_c) << 7);
					r_a = r;
					
					r_ccr_c = m & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
			
			case 0x56:	// ROR B - Rotate Right Accumulator B
				{
					uint8_t m = r_b;
					uint8_t r = (m >> 1) | ((r_ccr_c) << 7);
					r_b = r;
					
					r_ccr_c = m & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x66:	// ROR - Rotate Right (indexed)
				{
					uint16_t a = r_x + get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = (m >> 1) | ((r_ccr_c) << 7);
					memwrite(a, r);
					
					r_ccr_c = m & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x76:	// ROR - Rotate Right (extended)
				{
					uint16_t a = (get_next_byte() << 8) | get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = (m >> 1) | ((r_ccr_c) << 7);
					memwrite(a, r);
				
					r_ccr_c = m & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
			
			case 0x86:	// LDA A - Load Accumulator A (immediate)
				r_a = get_next_byte();
				r_ccr_n = r_a >> 7;
				r_ccr_z = !r_a;
				r_ccr_v = 0;
				break;
			
			case 0x96:	// LDA A - Load Accumulator A (direct)
				r_a = memread((uint16_t)get_next_byte());
				r_ccr_n = r_a >> 7;
				r_ccr_z = !r_a;
				r_ccr_v = 0;
				break;
				
			case 0xA6:  // LDA A - Load Accumulator A (indexed)
				r_a = memread(r_x + get_next_byte());
				r_ccr_n = r_a >> 7;
				r_ccr_z = !r_a;
				r_ccr_v = 0;
				break;
				
			case 0xB6:  // LDA A - Load Accumulator A (extended)
				r_a = memread((get_next_byte() << 8) | get_next_byte());
				r_ccr_n = r_a >> 7;
				r_ccr_z = !r_a;
				r_ccr_v = 0;
				break;
			
			case 0xC6:	// LDA B - Load Accumulator B (immediate)
				r_b = get_next_byte();
				r_ccr_n = r_b >> 7;
				r_ccr_z = !r_b;
				r_ccr_v = 0;
				break;
			
			case 0xD6:	// LDA B - Load Accumulator B (direct)
				r_b = memread((uint16_t)get_next_byte());
				r_ccr_n = r_b >> 7;
				r_ccr_z = !r_b;
				r_ccr_v = 0;
				break;
				
			case 0xE6:  // LDA B - Load Accumulator B (indexed)
				r_b = memread(r_x + get_next_byte());
				r_ccr_n = r_b >> 7;
				r_ccr_z = !r_b;
				r_ccr_v = 0;
				break;
				
			case 0xF6:  // LDA B - Load Accumulator B (extended)
				r_b = memread((get_next_byte() << 8) | get_next_byte());
				r_ccr_n = r_b >> 7;
				r_ccr_z = !r_b;
				r_ccr_v = 0;
				break;
			
		// === Low 7 ===
			case 0x07:	// TPA - Transfer from CCR to Accumulator A
				r_a = 0xC0 | (r_ccr_h << 5) | (r_ccr_i << 4) | (r_ccr_n << 3) | (r_ccr_z << 2) | (r_ccr_v << 1) | (r_ccr_c);
				break;
				
			case 0x17:	// TBA - Transfer Accumulator B to Accumulator A
				r_a = r_b;
				r_ccr_n = r_a >> 7;
				r_ccr_z = !r_a;
				r_ccr_v = 0;
				break;
				
			case 0x27:	// BEQ - Branch if Equal
				{
					int8_t n = (int8_t)get_next_byte();
					
					if (r_ccr_z) {
						r_pc += n;
					}
				}
				break;
				
			case 0x37:	// PSH B - Push Accumulator B onto Stack
				stack_push(r_b);
				break;
				
			case 0x47:	// ASR A - Arithmetic Shift Right Accumulator A
				{
					uint8_t m = r_a;
					uint8_t r = (m >> 1) | (m & (1 << 7));
					r_a = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_c = m & 1;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x57:	// ASR B - Arithmetic Shift Right Accumulator B
				{
					uint8_t m = r_b;
					uint8_t r = m >> 1 | (m & (1 << 7));
					r_b = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_c = m & 1;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x67:	// ASR - Arithmetic Shift Right (indexed)
				{
					uint16_t a = r_x + get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = m >> 1 | (m & (1 << 7));
					memwrite(a, r);
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_c = m & 1;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x77:	// ASR - Arithmetic Shift Right (extended)
				{
					uint16_t a = (get_next_byte() << 8) | get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = m >> 1 | (m & (1 << 7));
					memwrite(a, r);
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_c = m & 1;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
			
			case 0x97:	// STA A - Store Accumulator A (direct)
				memwrite(get_next_byte(), r_a);
				r_ccr_n = r_a >> 7;
				r_ccr_z = !r_a;
				r_ccr_v = 0;
				break;
				
			case 0xA7:  // STA A - Store Accumulator A (indexed)
				memwrite(r_x + get_next_byte(), r_a);
				r_ccr_n = r_a >> 7;
				r_ccr_z = !r_a;
				r_ccr_v = 0;
				break;
				
			case 0xB7:  // STA A - Store Accumulator A (extended)
				memwrite((get_next_byte() << 8) | get_next_byte(), r_a);
				r_ccr_n = r_a >> 7;
				r_ccr_z = !r_a;
				r_ccr_v = 0;
				break;
			
			case 0xD7:	// STA B - Store Accumulator A (direct)
				memwrite(get_next_byte(), r_b);
				r_ccr_n = r_b >> 7;
				r_ccr_z = !r_b;
				r_ccr_v = 0;
				break;
				
			case 0xE7:  // STA B - Store Accumulator B (indexed)
				memwrite(r_x + get_next_byte(), r_b);
				r_ccr_n = r_b >> 7;
				r_ccr_z = !r_b;
				r_ccr_v = 0;
				break;
				
			case 0xF7:  // STA B - Store Accumulator B (extended)
				memwrite((get_next_byte() << 8) | get_next_byte(), r_b);
				r_ccr_n = r_b >> 7;
				r_ccr_z = !r_b;
				r_ccr_v = 0;
				break;
		
		// === Low 8 ===
			case 0x08:	// INX - Increment Index Register
				++r_x;
				r_ccr_z = !r_x;
				break;
				
			case 0x18:	// XGDX - Exchange Double Accumulator and Index Register
				{
					uint16_t t = r_x;
					r_x = (r_a << 8) | r_b;
					r_a = t >> 8;
					r_b = t & 0x00FF;
				}
				break;
				
			case 0x28:	// BVC - Branch if Overflow Clear
				{
					int8_t n = (int8_t)get_next_byte();
					
					if (!r_ccr_v) {
						r_pc += n;
					}
				}
				break;
				
			case 0x38:	// PULX - Pop Index Register from Stack
				r_x = (stack_pop() << 8) | stack_pop();
				break;
				
			case 0x48:	// ASL A - Arithmetic Shift Left Accumulator A
				{
					uint8_t m = r_a;
					uint8_t r = m << 1;
					r_a = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_c = m >> 7;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x58:	// ASL B - Arithmetic Shift Left Accumulator B
				{
					uint8_t m = r_b;
					uint8_t r = m << 1;
					r_b = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_c = m >> 7;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x68:	// ASL - Arithmetic Shift Left (indexed)
				{
					uint16_t a = r_x + get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = m << 1;
					memwrite(a, r);
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_c = m >> 7;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x78:	// ASL - Arithmetic Shift Left (extended)
				{
					uint16_t a = (get_next_byte() << 8) | get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = m << 1;
					memwrite(a, r);
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_c = m >> 7;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x88:	// EORA - XOR Accumulator A (immediate)
				{
					uint8_t m = get_next_byte();
					uint8_t r = r_a ^ m;
					
					r_a = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
			
			case 0x98:	// EORA - XOR Accumulator A (direct)
				{
					uint8_t m = memread((uint16_t)get_next_byte());
					uint8_t r = r_a ^ m;
					
					r_a = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xA8:	// EORA - XOR Accumulator A (indexed)
				{
					uint8_t m = memread(r_x + (uint16_t)get_next_byte());
					uint8_t r = r_a ^ m;
					
					r_a = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xB8:	// EORA - XOR Accumulator A (extended)
				{
					uint8_t m = memread((get_next_byte() << 8) | get_next_byte());
					uint8_t r = r_a ^ m;
					
					r_a = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xC8:	// EORB - XOR Accumulator B (immediate)
				{
					uint8_t m = get_next_byte();
					uint8_t r = r_b ^ m;
					
					r_b = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
			
			case 0xD8:	// EORB - XOR Accumulator B (direct)
				{
					uint8_t m = memread((uint16_t)get_next_byte());
					uint8_t r = r_b ^ m;
					
					r_b = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xE8:	// EORB - XOR Accumulator B (indexed)
				{
					uint8_t m = memread(r_x + (uint16_t)get_next_byte());
					uint8_t r = r_b ^ m;
					
					r_b = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xF8:	// EORB - XOR Accumulator B (extended)
				{
					uint8_t m = memread((get_next_byte() << 8) | get_next_byte());
					uint8_t r = r_b ^ m;
					
					r_b = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
			
		// === Low 9 ===
			case 0x09:	// DEX - Decrement Index Register
				--r_x;
				r_ccr_z = !r_x;
				break;
				
			case 0x19:	// DAA - Decimal Adjust Accumulator A
				{
					#ifndef NOPRINTF
						printf("C6301::step(): warning: instruction 0x19 (DAA) has hardly been tested at all, expect weird results\n");
					#endif
					
					uint8_t m = r_a;
					uint8_t hi = m >> 4;
					uint8_t lo = m & 0x0F;
					uint8_t r = m;
					
					if (r_ccr_c) {
						if (r_ccr_h) {
							if (hi <= 0x03 && lo <= 0x03) r += 0x66;
						}
						else {
							if (hi <= 0x02) {
								if (lo <= 0x09) r += 0x60;
								else r += 0x66;
							}
						}
					}
					else {
						if (r_ccr_h) {
							if (hi >= 0x0A && lo <= 3) {
								r += 0x66;
								r_ccr_c = 1;
							}
							else if (hi <= 0x09 && lo <= 0x03) {
								r += 0x06;
							}
						}
						else {
							if (hi <= 0x08 && lo >= 0x0A) {
								r += 0x06;
							}
							else if (hi >= 0x0A && lo <= 0x09) {
								r += 0x06;
								r_ccr_c = 1;
							}
							else if (hi >= 0x09 && lo >= 0x0A) {
								r += 0x66;
								r_ccr_c = 1;
							}
						}
					}
					
					r_a = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
				}
				break;
				
			case 0x29:	// BVS - Branch if Overflow Set
				{
					int8_t n = (int8_t)get_next_byte();
					
					if (r_ccr_v) {
						r_pc += n;
					}
				}
				break;
				
			case 0x39:  // RTS - Return from Subroutine
				r_pc = (stack_pop() << 8) | stack_pop();
				break;
				
			case 0x49:	// ROL A - Rotate Left Accumulator A
				{
					uint8_t m = r_a;
					uint8_t r = (m << 1) | r_ccr_c;
					r_a = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_c = m >> 7;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x59:	// ROL B - Rotate Left Accumulator B
				{
					uint8_t m = r_b;
					uint8_t r = (m << 1) | r_ccr_c;
					r_b = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_c = m >> 7;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x69:	// ROL - Rotate Left (indexed)
				{
					uint16_t a = r_x + get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = (m << 1) | r_ccr_c;
					memwrite(a, r);
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_c = m >> 7;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x79:	// ROL - Rotate Left (extended)
				{
					uint16_t a = (get_next_byte() << 8) | get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = (m << 1) | r_ccr_c;
					memwrite(a, r);
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_c = m >> 7;
					r_ccr_v = r_ccr_n ^ r_ccr_c;
				}
				break;
				
			case 0x89:	// ADC A - Add Accumulator A with Carry (immediate)
				{
					uint8_t x = r_a;
					uint8_t m = get_next_byte();
					uint8_t r = x + m + r_ccr_c;
					
					r_a = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
				
			case 0x99:	// ADC A - Add Accumulator A with Carry (direct)
				{
					uint8_t x = r_a;
					uint8_t m = memread((uint16_t)get_next_byte());
					uint8_t r = x + m + r_ccr_c;
					
					r_a = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
				
			case 0xA9:	// ADC A - Add Accumulator A with Carry (indexed)
				{
					uint8_t x = r_a;
					uint8_t m = memread((uint16_t)get_next_byte() + r_x);
					uint8_t r = x + m + r_ccr_c;
					
					r_a = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
				
			case 0xB9:	// ADC A - Add Accumulator A with Carry (extended)
				{
					uint8_t x = r_a;
					uint8_t m = memread((get_next_byte() << 8) | get_next_byte());
					uint8_t r = x + m + r_ccr_c;
					
					r_a = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
				
			case 0xC9:	// ADC B - Add Accumulator B with Carry (immediate)
				{
					uint8_t x = r_b;
					uint8_t m = get_next_byte();
					uint8_t r = x + m + r_ccr_c;
					
					r_b = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
				
			case 0xD9:	// ADC B - Add Accumulator B with Carry (direct)
				{
					uint8_t x = r_b;
					uint8_t m = memread((uint16_t)get_next_byte());
					uint8_t r = x + m + r_ccr_c;
					
					r_b = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
				
			case 0xE9:	// ADC B - Add Accumulator B with Carry (indexed)
				{
					uint8_t x = r_b;
					uint8_t m = memread((uint16_t)get_next_byte() + r_x);
					uint8_t r = x + m + r_ccr_c;
					
					r_b = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
				
			case 0xF9:	// ADC B - Add Accumulator B with Carry (extended)
				{
					uint8_t x = r_b;
					uint8_t m = memread((get_next_byte() << 8) | get_next_byte());
					uint8_t r = x + m + r_ccr_c;
					
					r_b = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
			
		// === Low A ===
			case 0x0A:	// CLV - Clear Overflow
				r_ccr_v = 0;
				break;
				
			case 0x1A:	// SLP - Sleep
				//printf("C6301::step(): debug: entering sleep mode\n");
				b_sleep = true;
				break;
				
			case 0x2A:	// BPL - Branch if Plus
				{
					int8_t n = (int8_t)get_next_byte();
					
					if (!r_ccr_n) {
						r_pc += n;
					}
				}
				break;
			
			case 0x3A:  // ABX - Add Accumulator B to Index Register
				r_x += (uint16_t)r_b;
				break;
			
			case 0x4A:	// DEC A - Decrement Accumulator A
				{
					uint8_t m = r_a;
					uint8_t r = m - 1;
					r_a = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = (m == 0x80);
				}
				break;
			
			case 0x5A:	// DEC B - Decrement Accumulator B
				{
					uint8_t m = r_b;
					uint8_t r = m - 1;
					r_b = r;
				
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = (m == 0x80);
				}
				break;
				
			case 0x6A:	// DEC - Decrement (indexed)
				{
					uint16_t a = r_x + (uint16_t)get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = m - 1;
					memwrite(a, r);
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = (m == 0x80);
				}
				break;
				
			case 0x7A:	// DEC - Decrement (extended)
				{
					uint16_t a = (get_next_byte() << 8) | get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = m - 1;
					memwrite(a, r);
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = (m == 0x80);
				}
				break;
			
			case 0x8A:	// ORA - OR Accumulator A (immediate)
				{
					uint8_t m = get_next_byte();
					uint8_t r = r_a | m;
					
					r_a = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
			
			case 0x9A:	// ORA - OR Accumulator A (direct)
				{
					uint8_t m = memread((uint16_t)get_next_byte());
					uint8_t r = r_a | m;
					
					r_a = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xAA:	// ORA - OR Accumulator A (indexed)
				{
					uint8_t m = memread(r_x + (uint16_t)get_next_byte());
					uint8_t r = r_a | m;
					
					r_a = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xBA:	// ORA - OR Accumulator A (extended)
				{
					uint8_t m = memread((get_next_byte() << 8) | get_next_byte());
					uint8_t r = r_a | m;
					
					r_a = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xCA:	// ORB - OR Accumulator B (immediate)
				{
					uint8_t m = get_next_byte();
					uint8_t r = r_b | m;
					
					r_b = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
			
			case 0xDA:	// ORB - OR Accumulator B (direct)
				{
					uint8_t m = memread((uint16_t)get_next_byte());
					uint8_t r = r_b | m;
					
					r_b = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xEA:	// ORB - OR Accumulator B (indexed)
				{
					uint8_t m = memread(r_x + (uint16_t)get_next_byte());
					uint8_t r = r_b | m;
					
					r_b = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0xFA:	// ORB - OR Accumulator B (extended)
				{
					uint8_t m = memread((get_next_byte() << 8) | get_next_byte());
					uint8_t r = r_b | m;
					
					r_b = r;
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
			
		// === Low B ===
			case 0x0B:	// SEV - Set Overflow
				r_ccr_v = 1;
				break;
				
			case 0x1B:	// ABA - Add Accumulator A to Accumulator B
				{
					uint8_t a = r_a;
					uint8_t b = r_b;
					uint8_t r = a + b;
				
					r_a = r;
				
					r_ccr_h = ((a & b) | (b & ~r) | (~r & a) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((a & b & ~r) | (~a & ~b & r)) >> 7;
					r_ccr_c = ((a & b) | (b & ~r) | (~r & a)) >> 7;
				}
				break;
				
			case 0x2B:	// BMI - Branch if Minus
				{
					int8_t n = (int8_t)get_next_byte();
					
					if (r_ccr_n) {
						r_pc += n;
					}
				}
				break;
				
			case 0x3B:	// RTI - Return from Interrupt
				interrupt_leave();
				break;
			
			case 0x6B:	// TIM - Test Immediate (indexed)
				{
					uint8_t n = get_next_byte();
					uint16_t m = r_x + (uint16_t)get_next_byte();
					uint8_t r = memread(m) & n;
				
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
			
			case 0x7B:	// TIM - Test Immediate (direct)
				{
					uint8_t n = get_next_byte();
					uint16_t m = (uint16_t)get_next_byte();
					uint8_t r = memread(m) & n;
				
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = 0;
				}
				break;
				
			case 0x8B:	// ADD - Add Accumulator A without Carry (immediate)
				{
					uint8_t x = r_a;
					uint8_t m = get_next_byte();
					uint8_t r = x + m;
					
					r_a = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
				
			case 0x9B:	// ADD - Add Accumulator A without Carry (direct)
				{
					uint8_t x = r_a;
					uint8_t m = memread((uint16_t)get_next_byte());
					uint8_t r = x + m;
					
					r_a = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
				
			case 0xAB:	// ADD - Add Accumulator A without Carry (indexed)
				{
					uint8_t x = r_a;
					uint8_t m = memread((uint16_t)get_next_byte() + r_x);
					uint8_t r = x + m;
					
					r_a = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
				
			case 0xBB:	// ADD - Add Accumulator A without Carry (extended)
				{
					uint8_t x = r_a;
					uint8_t m = memread((get_next_byte() << 8) | get_next_byte());
					uint8_t r = x + m;
					
					r_a = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
				
			case 0xCB:	// ADD - Add Accumulator B without Carry (immediate)
				{
					uint8_t x = r_b;
					uint8_t m = get_next_byte();
					uint8_t r = x + m;
					
					r_b = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
				
			case 0xDB:	// ADD - Add Accumulator B without Carry (direct)
				{
					uint8_t x = r_b;
					uint8_t m = memread((uint16_t)get_next_byte());
					uint8_t r = x + m;
					
					r_b = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
				
			case 0xEB:	// ADD - Add Accumulator B without Carry (indexed)
				{
					uint8_t x = r_b;
					uint8_t m = memread((uint16_t)get_next_byte() + r_x);
					uint8_t r = x + m;
					
					r_b = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
				
			case 0xFB:	// ADD - Add Accumulator B without Carry (extended)
				{
					uint8_t x = r_b;
					uint8_t m = memread((get_next_byte() << 8) | get_next_byte());
					uint8_t r = x + m;
					
					r_b = r;
					
					r_ccr_h = (((x & m) | (m & ~r) | (~r & x)) >> 3) & 1;
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = ((x & m & ~r) | (~x & ~m & r)) >> 7;
					r_ccr_c = ((x & m) | (m & ~r) | (~r & x)) >> 7;
				}
				break;
			
		// === Low C ===
			case 0x0C:	// CLC - Clear Carry
				r_ccr_c = 0;
				break;
				
			case 0x2C:	// BGE - Branch if Greater or Equal to Zero
				{
					int8_t n = (int8_t)get_next_byte();
					
					if (!((r_ccr_n + r_ccr_v) % 2)) {
						r_pc += n;
					}
				}
				break;
			
			case 0x3C:	// PSH X - Push Index Register onto Stack
				stack_push(r_x & 0x00FF);
				stack_push(r_x >> 8);
				break;
			
			case 0x4C:	// INC A - Increment Accumulator A
				{
					uint8_t m = r_a;
					uint8_t r = m + 1;
					r_a = r;
				
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = (r == 0x80);
				}
				break;
			
			case 0x5C:	// INC B - Increment Accumulator B
				{
					uint8_t m = r_b;
					uint8_t r = m + 1;
					r_b = r;
				
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = (r == 0x80);
				}
				break;
				
			case 0x6C:	// INC - Increment (indexed)
				{
					uint16_t a = r_x + (uint16_t)get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = m + 1;
					memwrite(a, r);
				
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = (r == 0x80);
				}
				break;
				
			case 0x7C:	// INC - Increment (extended)
				{
					uint16_t a = (get_next_byte() << 8) | get_next_byte();
					uint8_t m = memread(a);
					uint8_t r = m + 1;
					memwrite(a, r);
					
					r_ccr_n = r >> 7;
					r_ccr_z = !r;
					r_ccr_v = (r == 0x80);
				}
				break;
				
			case 0x8C:	// CPX - Compare Index Register (immediate)
				{
					uint16_t m = (get_next_byte() << 8) | get_next_byte();
					uint16_t x = r_x;
					uint16_t r = x - m;
					
					r_ccr_n = r >> 15;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 15;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 15;
				}
				break;
				
			case 0x9C:	// CPX - Compare Index Register (direct)
				{
					uint16_t m = memread_double((uint16_t)get_next_byte());
					uint16_t x = r_x;
					uint16_t r = x - m;
					
					r_ccr_n = r >> 15;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 15;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 15;
				}
				break;
				
			case 0xAC:	// CPX - Compare Index Register (indexed)
				{
					uint16_t m = memread_double(r_x + get_next_byte());
					uint16_t x = r_x;
					uint16_t r = x - m;
					
					r_ccr_n = r >> 15;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 15;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 15;
				}
				break;
				
			case 0xBC:	// CPX - Compare Index Register (extended)
				{
					uint16_t m = memread_double((get_next_byte() << 8) | get_next_byte());
					uint16_t x = r_x;
					uint16_t r = x - m;
					
					r_ccr_n = r >> 15;
					r_ccr_z = !r;
					r_ccr_v = ((x & ~m & ~r) | (~x & m & r)) >> 15;
					r_ccr_c = ((~x & m) | (m & r) | (r & ~x)) >> 15;
				}
				break;
				
			case 0xCC:	// LDD - Load Double Accumulator (immediate)
				r_a = get_next_byte();
				r_b = get_next_byte();
				
				r_ccr_n = r_a >> 7;
				r_ccr_z = !r_a && !r_b;
				r_ccr_v = 0;
				break;
				
			case 0xDC:	// LDD - Load Double Accumulator (direct)
				{
					uint16_t a = (uint16_t)get_next_byte();
					r_a = memread(a);
					r_b = memread(a + 1);
				
					r_ccr_n = r_a >> 7;
					r_ccr_z = !r_a && !r_b;
					r_ccr_v = 0;
				}
				break;
				
			case 0xEC:	// LDD - Load Double Accumulator (indexed)
				{
					uint16_t a = r_x + get_next_byte();
					r_a = memread(a);
					r_b = memread(a + 1);
				
					r_ccr_n = r_a >> 7;
					r_ccr_z = !r_a && !r_b;
					r_ccr_v = 0;
				}
				break;
				
			case 0xFC:	// LDD - Load Double Accumulator (extended)
				{
					uint16_t a = (get_next_byte() << 8) | get_next_byte();
					r_a = memread(a);
					r_b = memread(a + 1);
				
					r_ccr_n = r_a >> 7;
					r_ccr_z = !r_a && !r_b;
					r_ccr_v = 0;
				}
				break;
			
		// === Low D ===
			case 0x0D:	// SEC - Set Carry
				r_ccr_c = 1;
				break;
				
			case 0x2D:	// BLT - Branch if Less than Zero
				{
					int8_t n = (int8_t)get_next_byte();
					
					if (r_ccr_n ^ r_ccr_v) {
						r_pc += n;
					}
				}
				break;
				
			case 0x3D:	// MUL - Multiply Unsigned
				{
					uint16_t r = r_a * r_b;
					r_a = (r >> 8);
					r_b = (r & 0xFF);
					r_ccr_c = r_b >> 7;
				}
				break;
				
			case 0x4D:	// TST - Test Accumulator A
				{
					uint8_t m = r_a;
					
					r_ccr_n = m >> 7;
					r_ccr_z = !m;
					r_ccr_v = 0;
					r_ccr_c = 0;
				}
				break;
				
			case 0x5D:	// TST - Test Accumulator B
				{
					uint8_t m = r_b;
					
					r_ccr_n = m >> 7;
					r_ccr_z = !m;
					r_ccr_v = 0;
					r_ccr_c = 0;
				}
				break;
				
			case 0x6D:	// TST - Test (indexed)
				{
					uint8_t m = memread(r_x + get_next_byte());
					
					r_ccr_n = m >> 7;
					r_ccr_z = !m;
					r_ccr_v = 0;
					r_ccr_c = 0;
				}
				break;
				
			case 0x7D:	// TST - Test (extended)
				{
					uint8_t m = memread((get_next_byte() << 8) | get_next_byte());
					
					r_ccr_n = m >> 7;
					r_ccr_z = !m;
					r_ccr_v = 0;
					r_ccr_c = 0;
				}
				break;
			
			case 0x8D:	// BSR - Branch to Subroutine
				{
					int8_t n = (int8_t)get_next_byte();
					stack_push((uint8_t)(r_pc & 0x00FF));
					stack_push((uint8_t)(r_pc >> 8));
					r_pc += n;
				}
				break;
			
			case 0x9D:	// JSR - Jump to Subroutine (direct)
				{
					uint16_t n = (uint16_t)get_next_byte();
					stack_push((uint8_t)(r_pc & 0x00FF));
					stack_push((uint8_t)(r_pc >> 8));
					r_pc = n;
				}
				break;
			
			case 0xAD:	// JSR - Jump to Subroutine (indexed)
				{
					uint16_t n = r_x + get_next_byte();
					stack_push((uint8_t)(r_pc & 0x00FF));
					stack_push((uint8_t)(r_pc >> 8));
					r_pc = n;
				}
				break;
			
			case 0xBD:	// JSR - Jump to Subroutine (extended)
				{
					uint16_t n = (get_next_byte() << 8) | get_next_byte();
					stack_push((uint8_t)(r_pc & 0x00FF));
					stack_push((uint8_t)(r_pc >> 8));
					r_pc = n;
				}
				break;
				
			case 0xDD:	// STD - Store Double Accumulator (direct)
				{
					uint16_t a = (uint16_t)get_next_byte();
					memwrite(a, r_a);
					memwrite(a + 1, r_b);
					
					r_ccr_n = r_a >> 7;
					r_ccr_z = !r_a && !r_b;
					r_ccr_v = 0;
				}
				break;
				
			case 0xED:	// STD - Store Double Accumulator (indexed)
				{
					uint16_t a = r_x + get_next_byte();
					memwrite(a, r_a);
					memwrite(a + 1, r_b);
					
					r_ccr_n = r_a >> 7;
					r_ccr_z = !r_a && !r_b;
					r_ccr_v = 0;
				}
				break;
				
			case 0xFD:	// STD - Store Double Accumulator (extended)
				{
					uint16_t a = (get_next_byte() << 8) | get_next_byte();
					memwrite(a, r_a);
					memwrite(a + 1, r_b);
					
					r_ccr_n = r_a >> 7;
					r_ccr_z = !r_a && !r_b;
					r_ccr_v = 0;
				}
				break;
			
		// === Low E ===
			case 0x0E:	// CLI - Clear Interrupt
				r_ccr_i = 0;
				break;
				
			case 0x2E:	// BGT - Branch if Greater than Zero
				{
					int8_t n = (int8_t)get_next_byte();
					
					if (!(r_ccr_z & (!((r_ccr_n + r_ccr_v) % 2)))) {
						r_pc += n;
					}
				}
				break;
				
			case 0x3E:	// WAI - Wait for Interrupt
				b_waitirq = 1;
				break;
				
			case 0x6E:	// JMP - Jump (indexed)
				r_pc = r_x + (uint16_t)get_next_byte();
				break;
				
			case 0x7E:	// JMP - Jump (extended)
				r_pc = (get_next_byte() << 8) | get_next_byte();
				break;
			
			case 0x8E:	// LDS - Load Stack Pointer (immediate)
				r_sp = (get_next_byte() << 8) | get_next_byte();
				r_ccr_n = r_sp >> 15;
				r_ccr_z = !r_sp;
				r_ccr_v = 0;
				break;
				
			case 0x9E:	// LDS - Load Stack Pointer (direct)
				r_sp = memread_double((uint16_t)get_next_byte());
				r_ccr_n = r_sp >> 15;
				r_ccr_z = !r_sp;
				r_ccr_v = 0;
				break;
				
			case 0xAE:	// LDS - Load Stack Pointer (indexed)
				r_sp = memread_double(r_x + get_next_byte());
				r_ccr_n = r_sp >> 15;
				r_ccr_z = !r_sp;
				r_ccr_v = 0;
				break;
				
			case 0xBE:	// LDS - Load Stack Pointer (extended)
				r_sp = memread_double((get_next_byte() << 8) | get_next_byte());
				r_ccr_n = r_sp >> 15;
				r_ccr_z = !r_sp;
				r_ccr_v = 0;
				break;
			
			case 0xCE:	// LDX - Load Index Register (immediate)
				r_x = (get_next_byte() << 8) | get_next_byte();
				r_ccr_n = r_x >> 15;
				r_ccr_z = !r_x;
				r_ccr_v = 0;
				break;
				
			case 0xDE:	// LDX - Load Index Register (direct)
				r_x = memread_double((uint16_t)get_next_byte());
				r_ccr_n = r_x >> 15;
				r_ccr_z = !r_x;
				r_ccr_v = 0;
				break;
				
			case 0xEE:	// LDX - Load Index Register (indexed)
				r_x = memread_double(r_x + get_next_byte());
				r_ccr_n = r_x >> 15;
				r_ccr_z = !r_x;
				r_ccr_v = 0;
				break;
				
			case 0xFE:	// LDX - Load Index Register (extended)
				r_x = memread_double((get_next_byte() << 8) | get_next_byte());
				r_ccr_n = r_x >> 15;
				r_ccr_z = !r_x;
				r_ccr_v = 0;
				break;
			
		// === Low F ===
			case 0x0F:	// SEI - Set Interrupt
				r_ccr_i = 1;
				break;
				
			case 0x2F:	// BLE - Branch if Less than or Equal to Zero
				{
					int8_t n = (int8_t)get_next_byte();
					
					if (r_ccr_z | (r_ccr_n ^ r_ccr_v)) {
						r_pc += n;
					}
				}
				break;
				
			case 0x3F:	// SWI - Software Interrupt
				b_irq_swi = true;
				break;
			
			case 0x4F:	// CLR A - Clear Accumulator A
				r_a = 0;
				r_ccr_n = 0;
				r_ccr_z = 1;
				r_ccr_v = 0;
				r_ccr_c = 0;
				break;
			
			case 0x5F:	// CLR B - Clear Accumulator B
				r_b = 0;
				r_ccr_n = 0;
				r_ccr_z = 1;
				r_ccr_v = 0;
				r_ccr_c = 0;
				break;
				
			case 0x6F:	// CLR - Clear (indexed)
				memwrite(r_x + (uint16_t)get_next_byte(), 0);
				r_ccr_n = 0;
				r_ccr_z = 1;
				r_ccr_v = 0;
				r_ccr_c = 0;
				break;
				
			case 0x7F:	// CLR - Clear (extended)
				memwrite((get_next_byte() << 8) | get_next_byte(), 0);
				r_ccr_n = 0;
				r_ccr_z = 1;
				r_ccr_v = 0;
				r_ccr_c = 0;
				break;
				
			case 0x9F:	// STS - Store Stack Pointer (direct)
				{
					uint16_t a = (uint16_t)get_next_byte();
					memwrite(a++, (uint8_t)(r_sp >> 8));
					memwrite(a, (uint8_t)(r_sp & 0x00FF));
					
					r_ccr_n = r_sp >> 15;
					r_ccr_z = !r_sp;
					r_ccr_v = 0;
				}
				break;	
			
			case 0xAF:	// STS - Store Stack Pointer (indexed)
				{
					uint16_t a = r_x + (uint16_t)get_next_byte();
					memwrite(a++, (uint8_t)(r_sp >> 8));
					memwrite(a, (uint8_t)(r_sp & 0x00FF));
					
					r_ccr_n = r_sp >> 15;
					r_ccr_z = !r_sp;
					r_ccr_v = 0;
				}
				break;
			
			case 0xBF:	// STS - Store Stack Pointer (extended)
				{
					uint16_t a = (get_next_byte() << 8) | get_next_byte();
					memwrite(a++, (uint8_t)(r_sp >> 8));
					memwrite(a, (uint8_t)(r_sp & 0x00FF));
					
					r_ccr_n = r_sp >> 15;
					r_ccr_z = !r_sp;
					r_ccr_v = 0;
				}
				break;
			
			case 0xDF:	// STX - Store Index Register (direct)
				{
					uint16_t a = (uint16_t)get_next_byte();
					memwrite(a++, (uint8_t)(r_x >> 8));
					memwrite(a, (uint8_t)(r_x & 0x00FF));
					
					r_ccr_n = r_x >> 15;
					r_ccr_z = !r_x;
					r_ccr_v = 0;
				}
				break;	
			
			case 0xEF:	// STX - Store Index Register (indexed)
				{
					uint16_t a = r_x + (uint16_t)get_next_byte();
					memwrite(a++, (uint8_t)(r_x >> 8));
					memwrite(a, (uint8_t)(r_x & 0x00FF));
					
					r_ccr_n = r_x >> 15;
					r_ccr_z = !r_x;
					r_ccr_v = 0;
				}
				break;
			
			case 0xFF:	// STX - Store Index Register (extended)
				{
					uint16_t a = (get_next_byte() << 8) | get_next_byte();
					memwrite(a++, (uint8_t)(r_x >> 8));
					memwrite(a, (uint8_t)(r_x & 0x00FF));
					
					r_ccr_n = r_x >> 15;
					r_ccr_z = !r_x;
					r_ccr_v = 0;
				}
				break;
			
		// === Default ===
			default:
				#ifndef NOPRINTF
					printf("C6301::step(): error: illegal instruction 0x%02X\n", op);
				#endif
				trap();
				break;
	}
}

inline uint8_t C6301::get_next_byte() {
	return memread(r_pc++);
}

void C6301::trap() {
	printf("C6301::trap(): debug: halting on trap\n");
	b_halted = true;
	//interrupt_enter(0xFFEE, 0xFFEF);
}

void C6301::interrupt_enter(uint16_t vecl, uint16_t vech) {
	if (b_nmi) printf("C6301::interrupt_enter(): error: blocked by on-going NMI interrupt\n");
	else printf("C6301::interrupt_enter()\n");
	b_sleep = false;	// Stop sleeping
	
	uint8_t ccr = r_ccr_c | (r_ccr_v << 1) | (r_ccr_z << 2) | (r_ccr_n << 3) | (r_ccr_i << 4) | (r_ccr_h << 5);
	
	stack_push(r_pc & 0x00FF);
	stack_push(r_pc >> 8);
	stack_push(r_x & 0x00FF);
	stack_push(r_x >> 8);
	stack_push(r_a);
	stack_push(r_b);
	stack_push(ccr);
	
	r_pc = (memread(vecl) << 8) | memread(vech);
	
	r_ccr_i = 1;
}

void C6301::interrupt_leave() {
	printf("C6301::interrupt_leave()\n");
	uint8_t ccr = stack_pop();
	
	r_b  = stack_pop();
	r_a  = stack_pop();
	r_x  = (stack_pop() << 8) | stack_pop();
	r_pc = (stack_pop() << 8) | stack_pop();
	
	r_ccr_c = ccr & 0x01;
	r_ccr_v = (ccr & 0x02) >> 1;
	r_ccr_z = (ccr & 0x04) >> 2;
	r_ccr_n = (ccr & 0x08) >> 3;
	r_ccr_i = (ccr & 0x10) >> 4;
	r_ccr_h = (ccr & 0x20) >> 5;
	
	b_nmi = false;
}

// ================================================================================
// Stack functions
// ================================================================================
void C6301::stack_push(uint8_t data) {
	#ifdef DEBUG_CPU
		printf("C6301::stack_push(): debug: pushing 0x%02X to 0x%04X\n", data, r_sp);
	#endif
	memwrite(r_sp--, data);
}

uint8_t C6301::stack_pop() {
	#ifdef DEBUG_CPU
		uint8_t n = memread(++r_sp);
		printf("C6301::stack_pop(): debug: popping 0x%02X from 0x%04X\n", n, r_sp);
		return n;
	#else
		return memread(++r_sp);
	#endif
}

uint8_t C6301::memread(uint16_t addr) {	
	uint8_t r;
	
	if (addr <= 0x001F) {
		r = r_internal[(uint8_t)addr];
		
		if (addr == 0x00) {
			return r_port1_ddr;
		}
		else if (addr == 0x01) {
			return r_port2_ddr;
		}
		else if (addr == 0x02) {
			return r_port1_data;
		}
		else if (addr == 0x03) {
			return r_port2_data;
		}
		if (opmode == 4 && ((addr >= 0x04 && addr <= 0x07) || addr == 0x0F)) {
			// Externally mapped addresses in mode 4: 0x04, 0x05, 0x06, 0x07 and 0x0F
			return membus->read(addr);
		}
		else if (addr == 0x08) {
			r_tcsr &= 0x1F;
			return r_tcsr;
		}
		else if (addr == 0x09) {
			return r_counter >> 8;
		}
		else if (addr == 0x0A) {
			return r_counter & 0xFF;
		}
		else if (addr == 0x0011 && (r & 0x80) && !b_rdrf_clear) {
			b_rdrf_clear = 1;
		}
		else if (addr == 0x0012 && b_rdrf_clear) {
			r_internal[0x11] &= 0x7F;
			b_rdrf_clear = 0;
		}
		
		return r;
	}
	else if (addr >= 0x0080 && addr <= 0x00FF && (r_internal[0x14] & (1 << 6))) {
		// Internal RAM
		r = ram->read(addr & 0x007F);
	}
	else if (opmode == 7 && addr >= 0xF000) {
		// Mask ROM
		printf("\e[31mC6301::memwrite(): write to mask rom ignored\n");
	}
	else {
		// Read from external memory bus
		r = membus->read(addr);
	}
	
	#ifdef DEBUG_CPU
		printf("\e[32mC6301::memread(): reading from address \e[1m0x%04X\e[22m returned \e[1m0x%02X\e[0m\n", addr, r);
	#else
		if (b_trace) printf("\e[32mC6301::memread(): reading from address \e[1m0x%04X\e[22m returned \e[1m0x%02X\e[0m\n", addr, r);
	#endif
	
	return r;
}

void C6301::memwrite(uint16_t addr, uint8_t data) {
	#ifdef DEBUG_CPU
		printf("\e[31mC6301::memwrite(): writing \e[1m0x%02X\e[22m to address \e[1m0x%04X\e[0m\n", data, addr);
	#else
		if (b_trace) printf("\e[31mC6301::memwrite(): writing \e[1m0x%02X\e[22m to address \e[1m0x%04X\e[0m\n", data, addr);
	#endif
	
	if (addr == 0x00) {
		// Port 1 Data Direction Register
		printf("Port 1 DDR:  ");
		
		for (int i = 7; i >= 0; i--) {
			if (data & (1 << i)) putchar('O');
			else putchar('I');
		}
		
		putchar('\n');
		
		r_port1_ddr = data;
		r_port1_data = r_port1_data & ~data;
	}
	else if (addr == 0x01) {
		// Port 2 Data Direction Register
		printf("Port 2 DDR:  ---");
		
		for (int i = 4; i >= 0; i--) {
			if (data & (1 << i)) putchar('O');
			else putchar('I');
		}
		
		putchar('\n');
		
		r_port2_ddr = data & 0x1F;
		r_port2_data = r_port2_data & ~data;
	}
	else if (addr == 0x02) {
		// Port 2 Data Register
		printf("Port 1 Data: ");
		
		for (int i = 7; i >= 0; i--) {
			if (r_port1_ddr & (1 << i)) {
				if (data & (1 << i)) putchar('H');
				else putchar('L');
			}
			else putchar('-');
		}
		
		putchar('\n');
		
		r_port1_data = (r_port1_data & ~r_port1_ddr) | (data & r_port1_ddr);
	}
	else if (addr == 0x03) {
		// Port 2 Data Register
		printf("Port 2 Data: ---");
		
		for (int i = 4; i >= 0; i--) {
			if (r_port2_ddr & (1 << i)) {
				if (data & (1 << i)) putchar('H');
				else putchar('L');
			}
			else putchar('-');
		}
		
		putchar('\n');
		
		r_port2_data = (r_port2_data & ~r_port2_ddr) | (data & r_port2_ddr);
	}
	else if (opmode == 4 && ((addr >= 0x0004 && addr <= 0x0007) || addr == 0x000F)) {
		membus->write(addr, data);
	}
	else if (addr == 0x0B) {
		// Output compare register (high byte)
		r_ocr = (data << 8) | (r_ocr & 0x00FF);
	}
	else if (addr == 0x0C) {
		// Output compare register (low byte)
		r_ocr = (r_ocr & 0xFF00) | data;
	}
	else if (addr == 0x08) {
		// Timer Control/Status Register (TCSR)
		r_tcsr = (r_tcsr & 0xE0) | (data & 0x1F);
		printf("r_tcsr set to %02X\n", r_tcsr);
	}
	else if (addr == 0x11) {
		// Transmit/Receive Control and Status Register (TRCSR)
		r_internal[0x11] = (r_internal[0x11] & 0xE1) | data;
	}
	else if (addr == 0x12) {
		// Receive Data Register - Can't write here, TRAP!
		printf("C6301::memwrite(): error: cannot write to receive data register (0x12)\n");
		trap();
	}
	else if (addr == 0x13) {
		// Transmit Data Register
		serial0->send(data);
	}
	else if (addr == 0x14) {
		// RAM Control Register
		r_internal[0x14] = data & 0xC0;
	}
	else if (addr >= 0x0080 && addr <= 0x00FF && (r_internal[0x14] & (1 << 6))) {
		// Internal RAM
		ram->write(addr & 0x007F, data);
	}
	else if (addr <= 0x1F) {
		printf("\e[31mC6301::memwrite(): unhandled internal register 0x%02X (0x%02X)\e[0m\n", addr, data);
	}
	else if (opmode == 7 && addr >= 0xF000) {
		// Mask ROM
		printf("\e[31mC6301::memwrite(): write to mask rom ignored\n");
	}
	else {
		// Write to external memory bus
		membus->write(addr, data);
	}
}

uint16_t C6301::memread_double(uint16_t addr) {
	return (memread(addr) << 8) | memread(addr + 1);
}

void C6301::memwrite_double(uint16_t addr, uint16_t data) {
	memwrite(addr, (uint8_t)((data & 0xFF00) >> 8));
	memwrite(addr + 1, (uint8_t)(data & 0x00FF));
}

uint8_t C6301::get_port1() {
	return r_port1_data;
}

uint8_t C6301::get_port2() {
	return r_port2_data;
}

uint8_t C6301::get_port3() {
	printf("C6301::get_port3(): fixme: stub\n");
	return 0x00;
}

uint8_t C6301::get_port4() {
	printf("C6301::get_port4(): fixme: stub\n");
	return 0x00;
}

void C6301::set_port1(uint8_t d) {
	r_port1_data = d & ~r_port1_ddr;
}

void C6301::set_port2(uint8_t d) {
	r_port2_data = d & ~r_port2_ddr;
}

void C6301::set_port3(uint8_t d) {
	printf("C6301::set_port3(): fixme: stub\n");
}

void C6301::set_port4(uint8_t d) {
	printf("C6301::set_port4(): fixme: stub\n");
}

