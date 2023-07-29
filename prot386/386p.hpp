// 386 protected mode header file

#ifndef _386P_HPP_
#define _386P_HPP_

typedef unsigned int word;
typedef unsigned char byte;
typedef unsigned long dword;

// описание дескриптора
typedef struct {
	word llimit;	// мл. слово предела 15..0
	word lbase;	// мл. слово базы 15..0
	byte mbase;	// ср. байт базы 23..16
	byte access;	// байт прав доступа [P|DPL:2|S|{TYPE:4}/{TYPE:3|A}]
	byte hlimGD;	// ст. 4 бита предела и биты G и D [G|D|0|0|L3|L2|L1|L0]
	byte hbase;	// ст. байт базы 31..24
} descriptor;

typedef struct {
	word ldoff;	// мл. слово смещения точки перехода
	word dsel;	// селектор точки перехода
	byte nparams;	// число передаваемых параметров
	byte access;	// байт прав доступа
	word hdoff;	// ст. слово смещения точки перехода
} idtdescr;

typedef struct {
	word lim;
	dword base;
} descreg;

// конструкторы байта прав доступа descriptor.access
const byte accPresent = 0x80;   // сегмент в памяти
const byte accNotPresent = 0x00;        // сегмент отсутствует в памяти
const byte DPL0 = 0x00; // уровень привилегий 0
const byte DPL1 = 0x20; // уровень привилегий 1
const byte DPL2 = 0x40; // уровень привилегий 2
const byte DPL3 = 0x60; // уровень привилегий 3
const byte accCD = 0x10;        // дескриптор сегмента кода или данных
const byte accSySeg = 0x00;     // дескриптор системного сегмента
// :: для дескрипторов системных сегментов (S=0, TYPE - 4 бита):
const byte accTrue = 0x80;      // дескриптор верен
const byte sstWrong = 0x00;     // значение неправильно
const byte sstTSS286free = 0x01;        // свободный TSS 286
const byte sstLDT       = 0x02; // дескриптор LDT
const byte sstTSS286busy = 0x03;        // занятый TSS 286
const byte sstProcGate286 = 0x04;       // шлюз процедуры 286
const byte sstTaskGateX86 = 0x05;       // шлюз задачи 286/386
const byte sstIntGate286 = 0x06;        // шлюз прерывания 286
const byte sstTrapGate286 = 0x07;       // шлюз ловушки 286
const byte sstNotAllowed = 0x08;        // недопустимое значение
const byte sstTSS386free = 0x09;        // свободный TSS 386
const byte sstReservedA = 0x0A; // резервировано Intel
const byte sstTSS386busy = 0x0B;        // занятый TSS 386
const byte sstProcGate386 = 0x0C;       // шлюз процедуры 386
const byte sstReservedD = 0x0D; // резервировано Intel
const byte sstIntGate386 = 0x0E;        // шлюз прерывания 386
const byte sstTrapGate386 = 0x0F;       // шлюз ловушки 386
// :: для дескрипторов сегментов кодов и данных (S=1, TYPE - 3 бита):
const byte accCSEG = 0x08;      // дескриптор сегмента кода
const byte accDSEG = 0x00;      // дескриптор сегмента данных
const byte accAcc = 0x01;       // к сегменту был доступ
const byte accNotAcc = 0x00;    // к сегменту не было доступа
// ::: для сегмента данных:
const byte accExUp = 0x00; // сегмент расширяется вверх
const byte accExDown = 0x04; // сегмент расширяется вниз (стек)
const byte accWrt = 0x02;       // запись воможна
const byte accNotWrt = 0x00; // запись запрещена
// ::: для сегмента кодов:
const byte accConf = 0x04;      // подчиненный сегмент
const byte accNotConf = 0x00; // не подчиненный сегмент
const byte accRd = 0x02;        // чтение возможно
const byte accNotRd = 0x00;     // чтение запрещено

// для descriptor.hlimGD
const byte GranByte = 0x00;     // бит гранулярности G = 0 - размер в байтах
const byte GranPage = 0x80;     // бит гранулярности G = 1 - размер в страницах
const byte Use16 = 0x00;        // бит разрядности D = 0 - 16 бит
const byte Use32 = 0x40;        // бит разрядности D = 1 - 32 бита

#define linaddr(s,o) (((dword)s << 4) + ((dword)o & 0xFFFF))

const word PL0 = 0;
const word PL1 = 1;
const word PL2 = 2;
const word PL3 = 3;

const word tGDT = 0;
const word tLDT = 1;

word inline n2sgdt(word n) {
	return (n << 3);
}

word inline mksel(word n,word rpl,word st) {
	return (n << 3) | (rpl & 3) | ((st & 1) << 2);
}
// mksel(N sel,RPL,GDT/LDT)

void initGDTdescr(descriptor* d,dword abase,dword alimit,byte acc,byte GD) {
	d->access = acc;
	d->llimit = (word)alimit;
	d->lbase = (word)abase;
	d->mbase = (byte)((abase & 0x00FF0000l) >> 16);
	d->hlimGD = (GD & 0xC0) | ((byte)((alimit & 0x000F0000l) >> 16));
	d->hbase = (byte)((abase & 0xFF000000l) >> 24);
}

void initIDTdescr(idtdescr* d,dword abase,word sel,byte n,byte acc) {
	d->access = acc;
	d->dsel = sel;
	d->nparams = n;
	d->ldoff = (word)abase;
	d->hdoff = (word)(abase >> 16);
}

#endif
