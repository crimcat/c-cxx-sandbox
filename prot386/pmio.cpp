// protected mode i/o library

typedef unsigned int word;

const int maxX = 80;
const int maxY = 25;

static int curX = 0;
static int curY = 0;
static word attr = 0x0F;
static word crtsel = 0;

void setcrtselector(word sel) { crtsel = sel; }

void gotoxy(int y,int x) { curY = y; curX = x; }

void settextattr(word a) { attr = a; }

void clrscr(char a) {
	curX = curY = 0;
	int cnt = maxX * maxY;
	word sv = a << 8;
	asm push es;
	_ES = crtsel;
	word _es* pcur = 0;
	while(--cnt) *pcur++ = sv;
	asm pop es;
}

void clrscr(void) { clrscr(attr); }

void outchar(char c) {
	asm push es;
	_ES = crtsel;
	word _es* pcur = (word _es*)(curX * 2 + curY * 160);
	*pcur = (attr << 8) | c;
	asm pop es;
	if(++curX == maxX) {
		curX = 0;
		if(++curY == maxY) curY = 0;
	}
}

void outcharxy(int y,int x,char c) {
	curY = y;
	curX = x;
	outchar(c);
}

void print(char* s) {
	asm push es;
	_ES = crtsel;
	word _es* pcur = (word _es*)(curX * 2 + curY * 160);
	while(*s) {
		*pcur++ = (attr << 8) | *s++;
		if(++curX == maxX) {
			curX = 0;
			if(++curY == maxY) curY = 0;
		}
	}
	asm pop es;
}

void printxy(int y,int x,char* s) {
	curY = y;
	curX = x;
	print(s);
}

void hexprint(word num) {
	const char sym[] = "0123456789ABCDEF";
	word mask = 0xF000;
	for(int i = 0; i < 4; i++) {
		outchar(sym[(num & mask) >> (12 - 4 * i)]);
		mask >>= 4;
	}
}

void hexprintxy(int y,int x,word num) {
	curY = y;
	curX = x;
	hexprint(num);
}

void decprint(word num) {
	const char sym[] = "0123456789";
	char ws[] = "00000";
	int i = 5;
	do {
		ws[--i] = sym[num % 10];
		num /= 10;
	} while(num);
	print(&ws[i]);
}

void decprintxy(int y,int x,word num) {
	curY = y;
	curX = x;
	decprint(num);
}