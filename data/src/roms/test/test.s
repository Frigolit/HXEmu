; =============================================================================
; @author  Pontus Rodling <frigolit@frigolit.net>
; @license MIT license - See LICENSE for more information
;
; This ROM is to be loaded into the ROM #0-3 socket on the HX-20.
; =============================================================================

	code

; =============================================================================
; Program start
; =============================================================================
	org 0x8000

reset:
	sei
	ldaa #0x00
	ldab #0x00
	
	lds #0x3FFF
	;db 0x8E,0x3F,0xFF	; Load stack pointer 0x3FFF
	
	jsr init_lcd
	
	ldaa #0x00
	ldab #0x00
	jsr lcd_draw_pixel
	
sleep_loop:
	db 0x1A
	jmp sleep_loop

; =============================================================================
; Initialize LCD
; =============================================================================
init_lcd:
	; Set frame frequency
	; -------------------------------------
	ldaa #0x10
	sta 0x2A
	jsr lcd_exec_cmd_all
	
	; Set multiplexing mode
	; -------------------------------------
	ldaa #0x1E		; Send command 1E to first controller
	sta 0x2A
	
	ldaa #0x09
	sta 0x26
	jsr lcd_exec
	
	ldaa #0x1C		; Send command 1C to remaining controllers
	sta 0x2A
	
	ldaa #0x0A
	sta 0x26
	jsr lcd_exec
	
	ldaa #0x0B
	sta 0x26
	jsr lcd_exec
	
	ldaa #0x0C
	sta 0x26
	jsr lcd_exec
	
	ldaa #0x0D
	sta 0x26
	jsr lcd_exec
	
	ldaa #0x0E
	sta 0x26
	jsr lcd_exec
	
	; Disable display
	; -------------------------------------
	ldaa #0x08
	sta 0x2A
	jsr lcd_exec_cmd_all
	
	; Clear display
	; -------------------------------------
	ldaa #0x64	; Set write mode
	sta 0x2A
	jsr lcd_exec_cmd_all
	
	ldx #0x80	; width = 128
	
init_lcd_loop:
	clr 0x2A
	jsr lcd_exec_data_all
	
	dex
	bne init_lcd_loop
	
	; Enable display
	; -------------------------------------
	ldaa #0x09
	sta 0x2A
	jsr lcd_exec_cmd_all
	
	rts

; =============================================================================
; LCD functions
; =============================================================================
lcd_exec:
	ldd 0x2A
	ldd 0x2A
	ldd 0x2A
	ldd 0x2A
	rts

lcd_exec_cmd_all:
	ldaa #0x09
	sta 0x26
	jsr lcd_exec
	
	ldaa #0x0A
	sta 0x26
	jsr lcd_exec
	
	ldaa #0x0B
	sta 0x26
	jsr lcd_exec
	
	ldaa #0x0C
	sta 0x26
	jsr lcd_exec
	
	ldaa #0x0D
	sta 0x26
	jsr lcd_exec
	
	ldaa #0x0E
	sta 0x26
	jsr lcd_exec
	rts

lcd_exec_data_all:
	ldaa #0x01
	sta 0x26
	jsr lcd_exec
	
	ldaa #0x02
	sta 0x26
	jsr lcd_exec
	
	ldaa #0x03
	sta 0x26
	jsr lcd_exec
	
	ldaa #0x04
	sta 0x26
	jsr lcd_exec
	
	ldaa #0x05
	sta 0x26
	jsr lcd_exec
	
	ldaa #0x06
	sta 0x26
	jsr lcd_exec
	rts

lcd_draw_pixel:
	psha
	pshb
	
	
	
	pulb
	pula
	rts

; =============================================================================
; TRAP interrupt
; =============================================================================
isr_trap:
	; TODO: Tell the testing device we ran into a TRAP
	jmp reset		; Reset

; =============================================================================
; Interrupt vectors
; =============================================================================
isr_ignore:
	rti

; Interrupt vectors (located at the top of the 64KiB address space)
	org 0xFFEE
	
	dw isr_trap		; Trap
	dw isr_ignore	; SCI (RDRF + ORFE + TDRE)
	dw isr_ignore	; TOF (Timer Overflow)
	dw isr_ignore	; OCF (Timer Output Compare)
	dw isr_ignore	; ICF (Timer Input Capture)
	dw isr_ignore	; IRQ1
	dw isr_ignore	; Software interrupt
	dw isr_ignore	; NMI
	dw reset		; Reset

