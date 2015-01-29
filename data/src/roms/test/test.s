; =============================================================================
; @author  Pontus Rodling <frigolit@frigolit.net>
; @license MIT license - See LICENSE for more information
;
; This ROM is to be loaded at 0x4000 via the test device.
; =============================================================================

	code

; =============================================================================
; Program start
; =============================================================================
	org 0x4000

; Program header
	db "\xBA\xC1\xFF\xFF\x00\x17HXEmu Test ROM   "

entrypoint:
	sei
	ldx #str_dbg_welcome
	jsr print_debug_string

	; === Reset device ===
	ldx #str_dbg_resetdevice
	jsr print_debug_string

	ldaa #0x01
	sta 0x5FF0
	jsr wait_for_device

next_test:
	; Request next test
	ldx #str_dbg_reqtest
	jsr print_debug_string

	ldaa #0x02
	sta 0x5FF0
	jsr wait_for_device

	; TODO: Check if we're done or not

	; Execute test
	ldx #str_dbg_runtest
	jsr print_debug_string

	jsr 0x5000

	; Tell device we're done with the test
	ldaa #0x04
	sta 0x5FF0

	jmp next_test

sleep_loop:
	db 0x1A
	jmp sleep_loop

wait_for_device:
	lda #0x80
	cmpa 0x5FF0
	bne wait_for_device
	rts

print_debug_string:
	ldab #0x00
print_debug_string_loop:
	db "\xA6\x00"	; LDA A (idx)
	cba
	beq print_debug_string_done

	sta 0x5FF2
	inx
	jmp print_debug_string_loop
print_debug_string_done:
	rts

str_dbg_welcome:
	db "=== HXEmu Test ROM v0.1 ===\n\0"
str_dbg_resetdevice:
	db "Resetting test device...\n\0"
str_dbg_reqtest:
	db "Waiting for program...\n\0"
str_dbg_runtest:
	db "Executing program...\n\0"

; =============================
	org 0x4FFF
	db 0x00

