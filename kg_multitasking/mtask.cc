// multitasking shell for procedures

#include <stdio.h>
#include <dos.h>
#include <conio.h>

struct TSS {	// ������������� ��������� ������ (TSS)
	unsigned int tSS;	// SS ������
	unsigned int tSP;	// SP ������
	char* stack;	// ��������� �� ���� ������
	TSS* nextTSS;	// ������->
	TSS* prevTSS;	// <-�����
};

TSS* mainTSS = (TSS *)0;	// ��������� �� TSS main � ������ ���������������
TSS* curTSS;	// ��������� �� TSS ������� ������
TSS* auxTSS;	// ������ ��� ���������

int taskcount = 0;	// ����� ����� � ������ ���������������
int switchalert = 0;	// ���� - ���������� ����������� ������
			// switchalert ������������, ���� ���� �������
			// ������������, �� �� ����� (���� � DOS'e, ��������)
void interrupt (* oldtimervec)(...);	// ������ ������ �������

void begintaskarea(void);	// ������� ������ ������� ������ ��� �����
void interrupt deletetask(void);	// ������� �������� ������
void interrupt switchtask(...);	// ������� ������������ �����

// ��� inline-������� ��� ���/���� ���������������
void inline startswitching(void) { setvect(0x08,&switchtask); }
void inline stopswitching(void) { setvect(0x08,oldtimervec); }

int registertask(void (* task)(), int ssize = 4096) {
	// ����� ������������ ������ � ������ ���������������
	// �� ��������� ���� - 4096 ����
	asm cli;
	TSS* newTSS = new TSS;
	if(!newTSS) return 0;	// ��� ������ - ��������
	newTSS->stack = new char[ssize]; // ���� ��� �������
	if(!(newTSS->stack)) {
		delete newTSS;
		return 0;	// ��� ������ - ��������
	}
	newTSS->tSP = ((long)(newTSS->stack) & 0x0000FFFFl) + ssize;
	newTSS->tSS = ((long)(newTSS->stack) & 0xFFFF0000l) >> 16;
	// ��������� ���������������������� � �������
	newTSS->nextTSS = curTSS->nextTSS;
	newTSS->prevTSS = curTSS;
	curTSS->nextTSS = newTSS;
	newTSS->nextTSS->prevTSS = newTSS;
	// ��������� ���� ������
	asm {
		mov	cx,ss
		mov	dx,sp
		les	si,task	// ����� ��� ������: ���� �� ��� � ����� ����������
		mov	ax,es
		les	di,newTSS
		// ������������� �� ���� �������������� ������
		mov	ss,word ptr es:[di]
		mov	sp,word ptr es:[di + 2]
		// ����������� � ���� ������ ����� �������� �� RET �� deletetask
		push cs
		push offset deletetask
		// ����������� � ���� ������ ����� �������� �� IRET � switchtask
		sti
		pushf
		cli
		push ax
		push si
		// ����������� ����� ����� ��� interrupt �������
		push ax
		push bx
		push cx
		push dx
		push es
		push ds
		push si
		push di
		push bp
		// �������� SS:SP � ������� � �������� ���� ����������
		mov	ax,ss
		mov	bx,sp
		mov	sp,dx
		mov	ss,cx
		les	di,newTSS
		mov	word ptr es:[di],ax
		mov	word ptr es:[di + 2],bx
	}
	++taskcount;
	asm sti;
	return 1;	// ����������������
}

void interrupt switchtask(...) {
	// ����� ������� ��������� (������, ������������)
	asm {
		mov switchalert,1	//	����, ��� ������� �� �������������
		mov	ax,cs	// ���������, ��� ����� ��������� �� �� ����� ������
		mov	bp,sp	// �����-�� DOS ��� ��� BIOS ��� ���� RTL
		cmp	ax,word ptr ss:[bp + 20]
		jnz	nelzya	// �������� �� ������� - �������� � ������������� ������
		mov	ax,word ptr ss:[bp + 18]
		cmp	ax,offset begintaskarea
		jb	nelzya	// �������� �� �� - �������� � ������������ ������
		// � ������ ������������� - ������ ��������� ����
		les	di,curTSS
		mov	word ptr es:[di],ss
		mov	word ptr es:[di + 2],sp
		les	di,dword ptr es:[di + 8]	// ������� �� ��������� ������
		cli
		mov	ss,word ptr es:[di]	// � ��� -
		mov	sp,word ptr es:[di + 2]	// - ��� ����� ����.
		mov	word ptr ds:[offset curTSS],di	// ��, � ����� ���������
		mov	word ptr ds:[offset curTSS + 2],es	// �� ������.
		mov	switchalert,0	// ������������� �������
	}
	nelzya:
	oldtimervec();	// � ��� DOS �� ��������
}

void interrupt deletetask(void) {
	// ������� ������ � ������ ��������������� �� �� ����������
	asm cli;
	// ���������� TSS ������������� ������
	auxTSS = curTSS;
	curTSS->prevTSS->nextTSS = curTSS->nextTSS;
	curTSS->nextTSS->prevTSS = curTSS->prevTSS;
	curTSS = curTSS->nextTSS;
	delete auxTSS->stack;
	delete auxTSS;
	if(!--taskcount) stopswitching(); // ���� ����� ������ ��� - ������
	asm {
		// ������! �� ���� ��������� ������
		les	di,curTSS
		mov	ss,word ptr es:[di]
		mov	sp,word ptr es:[di + 2]
	}
}

void begintaskarea(void) {}	// ������ ������� ��� �����

void inline idle(void) {
	// �������� ������������� ������������
	// ���������� �������� ����� ������� DOS, BIOS ��� RTL
	if(switchalert) switchtask();
}

void atask(void) {
	int i;
	for(i = 0; i < 50; i++) {
		for(int j = 0; j < 10000; j++);
		gotoxy(5,5);
		idle();
		cprintf("Task A message: %i",i);
		idle();
	}
}
void btask(void) {
	int i;
	for(i = 0; i < 50; i++) {
		for(int j = 0; j < 20000; j++);
		gotoxy(10,10);
		idle();
		cprintf("Task B message: %i",i);
		idle();
	}
}
void ctask(void) {
	int i;
	for(i = 0; i < 50; i++) {
		for(int j = 0; j < 30000; j++);
		gotoxy(15,15);
		idle();
		cprintf("Task C message: %i",i);
		idle();
	}
}

void main(void) {
	// ������� TSS main : ����������� ������ ���������
	mainTSS = new TSS;
	mainTSS->stack = (char *)0;
	mainTSS->nextTSS = mainTSS;
	mainTSS->prevTSS = mainTSS;
	curTSS = mainTSS;
	oldtimervec = getvect(0x08);
	startswitching();
	// ��������������� �������� ����� ����������� ������ ������
	registertask(&atask);
	registertask(&ctask);
	registertask(&btask);
	while(taskcount);
}
