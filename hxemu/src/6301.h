// =============================================================================
// @author Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef HXEMU_6301_H
#define HXEMU_6301_H

#include <stdint.h>
#include <stdio.h>

#include "ram.h"
#include "rom.h"
#include "memorybus.h"
#include "virtualserial.h"

class C6301 {
	public:
		C6301();
		~C6301();

		void reset();
		void step();
		void trap();
		void nmi();
		void jump(uint16_t addr);

		uint8_t get_port1();
		uint8_t get_port2();
		uint8_t get_port3();
		uint8_t get_port4();

		void set_port1(uint8_t d);
		void set_port2(uint8_t d);
		void set_port3(uint8_t d);
		void set_port4(uint8_t d);

		CMemoryDevice *membus;
		CROM *maskrom;
		VirtualSerial *serial0;

		bool b_irq1;
		bool b_trace;

	private:
		bool b_inited;

		FILE *tracefile;
		CRAM *ram;

		uint8_t opmode;

		bool b_halted;
		bool b_sleep;

		uint8_t r_port1_ddr;
		uint8_t r_port1_data;
		uint8_t r_port2_ddr;
		uint8_t r_port2_data;

		uint8_t r_a;
		uint8_t r_b;
		uint16_t r_x;
		uint16_t r_sp;
		uint16_t r_pc;

		uint8_t r_ccr_n;
		uint8_t r_ccr_v;
		uint8_t r_ccr_z;
		uint8_t r_ccr_c;
		uint8_t r_ccr_h;
		uint8_t r_ccr_i;

		uint16_t r_counter;
		uint16_t r_ocr;
		uint8_t r_tcsr;

		uint8_t r_internal[32];

		uint8_t b_waitirq;
		bool b_nmi;
		bool b_nmi_enter;
		bool b_irq_swi;

		uint8_t b_rdrf_clear;

		uint8_t memread(uint16_t addr);
		void memwrite(uint16_t addr, uint8_t data);

		uint16_t memread_double(uint16_t addr);
		void memwrite_double(uint16_t addr, uint16_t data);

		void stack_push(uint8_t data);
		uint8_t stack_pop();

		void interrupt_enter(uint16_t, uint16_t);
		void interrupt_leave();

		uint8_t get_next_byte();
};

#endif

