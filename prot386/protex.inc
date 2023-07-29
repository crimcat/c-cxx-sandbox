
; standard descriptor format

struc descr
   lim_l   dw	   0	   ; segment limit 15..0
   base_l  dw	   0	   ; segment base 15..0
   base_m  db	   0	   ; segment base 23..16
   access  db	   0	   ; segment access byte
   lim_h   db	   0	   ; segment limit 19..16 and G,D bits
   base_h  db	   0	   ; segment base 31..24
ends descr

; access byte bits

accPresent	equ  10000000b	  ; segment is located in memory
accCSeg 	equ  00011000b	  ; code segment
accDSeg 	equ  00010000b	  ; data segment
accSys		equ  00000000b	  ; system segment
accAcc		equ  00000001b	  ; access to segment performed
; for data seg
accExDown	equ  00000100b	  ; stack segment
accWr		equ  00000010b	  ; data write enable
; for code seg
accCfr		equ  00000100b	  ; conformed segment
accRd		equ  00000010b	  ; data read enable

; privileges
DPL0		equ  00000000b
DPL1		equ  00100000b
DPL2		equ  01000000b
DPL3		equ  01100000b

GrPage		equ  10000000b	  ; granularity
Use32		equ  01000000b	  ; data width bit (0/1 - 16/32)

; code/data segment types
; code segment:
CSeg_A = accPresent OR accCSeg OR accCfr OR DPL0
; data segment:
DSeg_A = accPresent OR accDSeg OR accWr OR DPL0
; stack segment:
SSeg_A = accPresent OR accDSeg OR accWr OR accExDown OR DPL0

; system segment types

TSS286free	equ  01h	  ; free tss286
LDT		equ  02h	  ; local descriptor table
TSS286busy	equ  03h	  ; busy tss286
ProcGate286	equ  04h	  ; procedure gate 286
TaskGateX86	equ  05h	  ; task gate 286/386
IntGate286	equ  06h	  ; interrupt gate 286
TrapGate286	equ  07h	  ; trap gate 286
TSS386free	equ  09h	  ; free tss386
TSS386busy	equ  0Bh	  ; busy tss386
ProcGate386	equ  0Ch	  ; procedute gate 386
IntGate386	equ  0Eh	  ; interrupt gate 386
TrapGate386	equ  0Fh	  ; trap gate 386

CRT_Seg       equ  0B800h	; text video memory segment
CRT_size_L    equ  4000h	; low word of video memory size
CRT_size_H    equ  0		; high nibble of video memory size

CMOS_port     equ  70h		; cmos address
KBD_portA     equ  60h		; keyboard port
KBD_portB     equ  61h		; keyboard port
KBD_status    equ  64h		; keyboard status port
Int_port      equ  21h		; interrupt mask port
A20_port      equ  0D1h 	; address line A20 control
A20_on	      equ  0DFh 	; open A20
A20_off       equ  0DDh 	; close A20

macro enableA20
	mov	al,A20_port
	out	KBD_status,al
	mov	al,A20_on
	out	KBD_portA,al
endm enableA20

macro disableA20
	mov	al,A20_port
	out	KBD_status,al
	mov	al,A20_off
	out	KBD_portA,al
endm disableA20

macro pmjmp _sel,_off
	db  0EAh
	dw  offset _off
	dw  _sel
endm pmjmp

macro rmjmp _lfar
	db  0EAh
	dw  offset _lfar
	dw  seg _lfar
endm rmjmp

macro pmcall _sel,_off
	db  09Ah
	dw  offset _off
	dw  _sel
endm pmcall

macro rmcall _lfar
	db  09Ah
	dw  offset _lfar
	dw  seg _lfar
endm rmcall