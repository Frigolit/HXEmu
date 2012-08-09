; ================================================================
; @author  Pontus Rodling <frigolit@frigolit.net>
; @license MIT license - See LICENSE for more information
; @version $Id: test.s 162 2012-03-06 17:12:56Z frigolit $
;
; This ROM is to be loaded via custom hardware connected to the
; expansion port on the real HX-20 or loaded via the CTestDevice
; class in HXEmu.
; ================================================================

	code

; ================================================================
; Program start
; ================================================================
	org 0xE000

reset:
	sei
	ldaa 0x00
	ldab 0x00

wait_for_test:
	; Wait for a test to become ready
	lda  0x01
wait_for_test_2:
	anda 0xA000
	beq  wait_for_test_2
	
	; Prepare for the test :3
	clra
	clrb
	lds  0x3FFF
	
	; Run the test
	jsr  0xC000

; ================================================================
; Serial dump function
; ================================================================
serial_dump:
	; Output A, B and X to serial
	sta 0xA002
	stb 0xA002
	stx 0xA002
	
	; Output CCR to serial
	psha
	tpa
	sta 0xA002
	pula
	
	; Done!
	rts

; ================================================================
; TRAP interrupt
; ================================================================
isr_trap:
	; TODO: Tell the testing device we ran into a TRAP
	jmp reset		; Reset

; ================================================================
; Interrupt vectors
; ================================================================
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

