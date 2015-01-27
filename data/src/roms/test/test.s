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

	db "\xBA\xC1\xFF\xFF\x00\x17HXEmu Test ROM   "
	
sleep_loop:
	db 0x1A
	jmp sleep_loop

	org 0x4FFE
	db 0x00
