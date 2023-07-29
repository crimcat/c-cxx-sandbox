ideal
p386
model small
radix 10

include "protex.inc"

public @switchProtMode$quiuiui,@switchRealMode$quiui

extrn @excexit$qui:near
extrn _gdtr
extrn _idtr

dataseg
oldSS	dw  ?
oldDS	dw  ?
oldES	dw  ?
oldGS dw	 ?
oldFS dw  ?

codeseg
proc @switchProtMode$quiuiui near
	mov	[oldSS],ss
	mov	[oldDS],ds
	mov	[oldES],es
	mov	[oldGS],gs
	mov	[oldFS],fs
	push	bp
	mov	bp,sp
	mov	ax,[bp+4]
	mov	[word cs:j_pm+3],ax
	mov	cx,[bp+6]
	mov	dx,[bp+8]
	cli
	mov	al,8Fh
	out	CMOS_port,al
	enableA20
	lgdt	[qword _gdtr]
	lidt	[qword _idtr]
	mov	eax,1
	mov	cr0,eax
j_pm:	pmjmp	0,flushPM
label flushPM far
	mov	ss,dx
	mov	ds,cx
	pop	bp
	retn
endp @switchProtMode$quiuiui

proc @switchRealMode$quiui near
	push	bp
	mov	bp,sp
	mov	ax,[bp+6]
	mov	bx,[bp+4]
	pop	bp
	push	bx
	push	offset cscor
	retf
label cscor far
	mov	[word _idtr],03FFh
	mov	[dword _idtr+2],0
	lidt	[qword _idtr]
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	mov	ss,ax	; right selectors for real mode
	mov	eax,0
	mov	cr0,eax
	rmjmp	flushRM
label flushRM far
	mov	ss,[oldSS]
	mov	es,[oldES]
	mov	ds,[oldDS]
	mov	fs,[oldFS]
	mov	gs,[oldGS]
	disableA20
	in	al,Int_port
	and	al,0FCh
	out	Int_port,al
	mov	al,0Dh
	out	CMOS_port,al
	sti
	retn
endp @switchRealMode$quiui

macro excproc lbl
public @exc_&lbl&$qv
proc @exc_&lbl&$qv near
	push	lbl
	push	0AAAAh
	jmp	@excexit$qui
endp @exc_&lbl&$qv
endm excproc

excproc 00
excproc 01
excproc 02
excproc 03
excproc 04
excproc 05
excproc 06
excproc 07
excproc 08
excproc 09
excproc 10
excproc 11
excproc 12
excproc 13
excproc 14
excproc 15
excproc 16
excproc 17
excproc 18
excproc 19
excproc 20
excproc 21
excproc 22
excproc 23
excproc 24
excproc 25
excproc 26
excproc 27
excproc 28
excproc 29
excproc 30
excproc 31

	end