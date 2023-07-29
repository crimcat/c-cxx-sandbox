// protected mode i/o library header

typedef unsigned int word;

void setcrtselector(word);
void gotoxy(int,int);
void settextattr(word);
void clrscr(char a);
void clrscr(void);
void outcharxy(int y,int x,char c);
void outchar(char c);
void printxy(int y,int x,char* s);
void print(char* s);
void hexprint(word);
void hexprintxy(int,int,word);
void decprint(word);
void decprintxy(int,int,word);
