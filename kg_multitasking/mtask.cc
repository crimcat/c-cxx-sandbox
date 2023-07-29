// multitasking shell for procedures

#include <stdio.h>
#include <dos.h>
#include <conio.h>

struct TSS {	// трехкопеечный описатель задачи (TSS)
	unsigned int tSS;	// SS задачи
	unsigned int tSP;	// SP задачи
	char* stack;	// указатель на стек задачи
	TSS* nextTSS;	// вперед->
	TSS* prevTSS;	// <-назад
};

TSS* mainTSS = (TSS *)0;	// указатель на TSS main в списке диспетчеризации
TSS* curTSS;	// указатель на TSS текущей задачи
TSS* auxTSS;	// просто так указатель

int taskcount = 0;	// число задач в списке диспетчеризации
int switchalert = 0;	// флаг - необходимо переключить задачу
			// switchalert выставляется, если была попытка
			// переключения, но не вышло (были в DOS'e, например)
void interrupt (* oldtimervec)(...);	// старый вектор таймера

void begintaskarea(void);	// форвард начала области памяти для задач
void interrupt deletetask(void);	// форвард удаления задачи
void interrupt switchtask(...);	// форвард переключения задач

// две inline-функции для вкл/выкл диспетчеризации
void inline startswitching(void) { setvect(0x08,&switchtask); }
void inline stopswitching(void) { setvect(0x08,oldtimervec); }

int registertask(void (* task)(), int ssize = 4096) {
	// здесь регистрируем задачу в списке диспетчеризации
	// по умолчанию стек - 4096 байт
	asm cli;
	TSS* newTSS = new TSS;
	if(!newTSS) return 0;	// ноу мемори - вылетаем
	newTSS->stack = new char[ssize]; // стек для задачки
	if(!(newTSS->stack)) {
		delete newTSS;
		return 0;	// ноу мемори - вылетаем
	}
	newTSS->tSP = ((long)(newTSS->stack) & 0x0000FFFFl) + ssize;
	newTSS->tSS = ((long)(newTSS->stack) & 0xFFFF0000l) >> 16;
	// впихиваем новозарегистрированную в очередь
	newTSS->nextTSS = curTSS->nextTSS;
	newTSS->prevTSS = curTSS;
	curTSS->nextTSS = newTSS;
	newTSS->nextTSS->prevTSS = newTSS;
	// формируем стек задачи
	asm {
		mov	cx,ss
		mov	dx,sp
		les	si,task	// потом уже нельзя: стек не тот и можно обломаться
		mov	ax,es
		les	di,newTSS
		// переключаемся на стек регистрируемой задачи
		mov	ss,word ptr es:[di]
		mov	sp,word ptr es:[di + 2]
		// заталкиваем в стек задачи адрес возврата по RET на deletetask
		push cs
		push offset deletetask
		// заталкиваем в стек задачи адрес возврата по IRET в switchtask
		sti
		pushf
		cli
		push ax
		push si
		// заталкиваем фрейм стека для interrupt функций
		push ax
		push bx
		push cx
		push dx
		push es
		push ds
		push si
		push di
		push bp
		// приводим SS:SP в порядок и задачкин стек запоминаем
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
	return 1;	// зарегистрировано
}

void interrupt switchtask(...) {
	// здесь хлопаем задачками (сиречь, процедурками)
	asm {
		mov switchalert,1	//	флаг, что неплохо бы переключиться
		mov	ax,cs	// проверяем, что вызов произошел не во время работы
		mov	bp,sp	// какой-то DOS или там BIOS или даже RTL
		cmp	ax,word ptr ss:[bp + 20]
		jnz	nelzya	// сегменты не совпали - вылетаем с установленным флагом
		mov	ax,word ptr ss:[bp + 18]
		cmp	ax,offset begintaskarea
		jb	nelzya	// смещения не те - вылетаем с установленнм флагом
		// а теперь переключаемся - просто подменяем стек
		les	di,curTSS
		mov	word ptr es:[di],ss
		mov	word ptr es:[di + 2],sp
		les	di,dword ptr es:[di + 8]	// прыгаем на следующую задачу
		cli
		mov	ss,word ptr es:[di]	// а это -
		mov	sp,word ptr es:[di + 2]	// - это новый стек.
		mov	word ptr ds:[offset curTSS],di	// ну, и новый указатель
		mov	word ptr ds:[offset curTSS + 2],es	// на задачу.
		mov	switchalert,0	// переключились успешно
	}
	nelzya:
	oldtimervec();	// и про DOS не забываем
}

void interrupt deletetask(void) {
	// грохаем задачу в списке диспетчеризации по ее завершению
	asm cli;
	// уничтожаем TSS завершившийся задачи
	auxTSS = curTSS;
	curTSS->prevTSS->nextTSS = curTSS->nextTSS;
	curTSS->nextTSS->prevTSS = curTSS->prevTSS;
	curTSS = curTSS->nextTSS;
	delete auxTSS->stack;
	delete auxTSS;
	if(!--taskcount) stopswitching(); // если задач больше нет - хватит
	asm {
		// вперед! на стек следующей задачи
		les	di,curTSS
		mov	ss,word ptr es:[di]
		mov	sp,word ptr es:[di + 2]
	}
}

void begintaskarea(void) {}	// начало области для задач

void inline idle(void) {
	// проверка необходимости переключения
	// желательно вызывать после функций DOS, BIOS или RTL
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
	// создаем TSS main : стандартное начало программы
	mainTSS = new TSS;
	mainTSS->stack = (char *)0;
	mainTSS->nextTSS = mainTSS;
	mainTSS->prevTSS = mainTSS;
	curTSS = mainTSS;
	oldtimervec = getvect(0x08);
	startswitching();
	// многозадачность стартует после регистрации первой задачи
	registertask(&atask);
	registertask(&ctask);
	registertask(&btask);
	while(taskcount);
}
