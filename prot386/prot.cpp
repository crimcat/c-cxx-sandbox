#include <process.h>
#include "386p.hpp"
#include "pmio.hpp"

// model SMALL!

#ifndef __SMALL__
#error Wrong building memory model
#endif

descriptor gdt[15];
idtdescr idt[32];

descreg gdtr;
descreg idtr;

const word d_0 = mksel(0,PL0,tGDT);
const word d_gdt = mksel(1,PL0,tGDT);
const word d_idt = mksel(2,PL0,tGDT);
const word d_dstd = mksel(3,PL0,tGDT);
const word d_cstd = mksel(4,PL0,tGDT);
const word d_ds4G = mksel(5,PL0,tGDT);
const word d_cs4G = mksel(6,PL0,tGDT);
const word d_crt = mksel(7,PL0,tGDT);
const word d_vid = mksel(8,PL0,tGDT);
const word d_tmp = mksel(9,PL0,tGDT);
const word d_ds = mksel(10,PL0,tGDT);
const word d_es = mksel(11,PL0,tGDT);
const word d_ss = mksel(12,PL0,tGDT);
const word d_cs = mksel(13,PL0,tGDT);
const word d_dummy = mksel(14,PL0,tGDT);

// exeptions handlers
void exc_00(void);
void exc_01(void);
void exc_02(void);
void exc_03(void);
void exc_04(void);
void exc_05(void);
void exc_06(void);
void exc_07(void);
void exc_08(void);
void exc_09(void);
void exc_10(void);
void exc_11(void);
void exc_12(void);
void exc_13(void);
void exc_14(void);
void exc_15(void);
void exc_16(void);
void exc_17(void);
void exc_18(void);
void exc_19(void);
void exc_20(void);
void exc_21(void);
void exc_22(void);
void exc_23(void);
void exc_24(void);
void exc_25(void);
void exc_26(void);
void exc_27(void);
void exc_28(void);
void exc_29(void);
void exc_30(void);
void exc_31(void);

void switchProtMode(word,word,word);
// pmEnter(selector cs,selector ds,selector ss)

void switchRealMode(word,word);
// rmReturn(right code selector for RM,right data selector for RM)

const byte accTrap = sstTrapGate286 | DPL0 | accPresent;

void initPMtables(void) {
	// init GDT
	initGDTdescr(&gdt[0],0,0,0,0);	// null descriptor
	initGDTdescr(&gdt[1],linaddr(_DS,&gdt),sizeof(gdt) - 1,
		accPresent | DPL0 | accCD | accDSEG | accExUp | accWrt,
		GranByte | Use16);	// gdt:gdt
	initGDTdescr(&gdt[2],0,0,0,0);	// gdt:idt
	initGDTdescr(&gdt[3],linaddr(_DS,0),0xFFFFl,
		accPresent | DPL0 | accCD | accDSEG | accExUp | accWrt,
		GranByte | Use16);	// gdt:dstd
	initGDTdescr(&gdt[4],linaddr(_CS,0),0xFFFFl,
		accPresent | DPL0 | accCD | accCSEG | accConf | accNotRd,
		GranByte | Use16);	// gdt:cstd
	initGDTdescr(&gdt[5],0,0x000FFFFF,
		accPresent | DPL0 | accCD | accDSEG | accExUp | accWrt,
		GranPage | Use16);	// gdt:ds4G
	initGDTdescr(&gdt[6],0,0x000FFFFF,
		accPresent | DPL0 | accCD | accCSEG | accConf | accNotRd,
		GranPage | Use16);	// gdt:cs4G
	initGDTdescr(&gdt[7],linaddr(0xB800,0),4000,
		accPresent | DPL0 | accCD | accDSEG | accExUp | accWrt,
		GranByte | Use16);	// gdt:crt
	initGDTdescr(&gdt[8],linaddr(0xA000,0),0xFFFF,
		accPresent | DPL0 | accCD | accDSEG | accExUp | accWrt,
		GranByte | Use16);	// gdt:vid
	initGDTdescr(&gdt[9],0,0,0,0);	// gdt:tmp
	initGDTdescr(&gdt[10],linaddr(_DS,0),0xFFFFl,
		accPresent | DPL0 | accCD | accDSEG | accExUp | accWrt,
		GranByte | Use16);	// gdt:ds
	initGDTdescr(&gdt[11],linaddr(_DS,0),0xFFFFl,
		accPresent | DPL0 | accCD | accDSEG | accExUp | accWrt,
		GranByte | Use16);	// gdt:es
	initGDTdescr(&gdt[12],linaddr(_SS,0),0xFFFFl,
		accPresent | DPL0 | accCD | accDSEG | accExUp | accWrt,
		GranByte | Use16);	// gdt:ss
	initGDTdescr(&gdt[13],linaddr(_CS,0),0xFFFFl,
		accPresent | DPL0 | accCD | accCSEG | accConf | accNotRd,
		GranByte | Use16);	// gdt:cs
	initGDTdescr(&gdt[14],0,0,0,0);	// gdt:dummy
	gdtr.lim = sizeof(gdt) - 1;
	gdtr.base = dword(_DS << 4) + dword(&gdt) & 0xFFFF;
	//-------------------------------------------------
	initIDTdescr(&idt[0],word(&exc_00),d_cs,0,accTrap);
	initIDTdescr(&idt[1],word(&exc_01),d_cs,0,accTrap);
	initIDTdescr(&idt[2],word(&exc_02),d_cs,0,accTrap);
	initIDTdescr(&idt[3],word(&exc_03),d_cs,0,accTrap);
	initIDTdescr(&idt[4],word(&exc_04),d_cs,0,accTrap);
	initIDTdescr(&idt[5],word(&exc_05),d_cs,0,accTrap);
	initIDTdescr(&idt[6],word(&exc_06),d_cs,0,accTrap);
	initIDTdescr(&idt[7],word(&exc_07),d_cs,0,accTrap);
	initIDTdescr(&idt[8],word(&exc_08),d_cs,0,accTrap);
	initIDTdescr(&idt[9],word(&exc_09),d_cs,0,accTrap);
	initIDTdescr(&idt[10],word(&exc_10),d_cs,0,accTrap);
	initIDTdescr(&idt[11],word(&exc_11),d_cs,0,accTrap);
	initIDTdescr(&idt[12],word(&exc_12),d_cs,0,accTrap);
	initIDTdescr(&idt[13],word(&exc_13),d_cs,0,accTrap);
	initIDTdescr(&idt[14],word(&exc_14),d_cs,0,accTrap);
	initIDTdescr(&idt[15],word(&exc_15),d_cs,0,accTrap);
	initIDTdescr(&idt[16],word(&exc_16),d_cs,0,accTrap);
	initIDTdescr(&idt[17],word(&exc_17),d_cs,0,accTrap);
	initIDTdescr(&idt[18],word(&exc_18),d_cs,0,accTrap);
	initIDTdescr(&idt[19],word(&exc_19),d_cs,0,accTrap);
	initIDTdescr(&idt[20],word(&exc_20),d_cs,0,accTrap);
	initIDTdescr(&idt[21],word(&exc_21),d_cs,0,accTrap);
	initIDTdescr(&idt[22],word(&exc_22),d_cs,0,accTrap);
	initIDTdescr(&idt[23],word(&exc_23),d_cs,0,accTrap);
	initIDTdescr(&idt[24],word(&exc_24),d_cs,0,accTrap);
	initIDTdescr(&idt[25],word(&exc_25),d_cs,0,accTrap);
	initIDTdescr(&idt[26],word(&exc_26),d_cs,0,accTrap);
	initIDTdescr(&idt[27],word(&exc_27),d_cs,0,accTrap);
	initIDTdescr(&idt[28],word(&exc_28),d_cs,0,accTrap);
	initIDTdescr(&idt[29],word(&exc_29),d_cs,0,accTrap);
	initIDTdescr(&idt[30],word(&exc_30),d_cs,0,accTrap);
	initIDTdescr(&idt[31],word(&exc_31),d_cs,0,accTrap);
	idtr.lim = sizeof(idt) - 1;
	idtr.base = dword(_DS << 4) + dword(&idt) & 0xFFFF;
}

void excexit(word n) {	// write exception message and quit
	setcrtselector(d_crt);
	settextattr(0x0F);
	clrscr();
	printxy(1,0,"Exception = ");
	hexprint(n);
	printxy(2,0,"DS = "); hexprint(_DS);
	print("; ES = "); hexprint(_ES);
	print("; SS = "); hexprint(_SS);
	print("; SP = "); hexprint(_SP);
	printxy(3,0,"Stack status:");
	word* pstk = (word*)(_BP + 4);
	for(int i = 0; i < 6; i++) {
		hexprintxy(4 + i,0,*pstk++);
		print("  ");
		hexprint(*pstk++);
	}
	printxy(10,0,"Abnormal returning to the real mode.");
	switchRealMode(d_cstd,d_dstd);
	exit(1);
}

int main(void) {
	initPMtables();
	switchProtMode(d_cs,d_ds,d_ss);
	_ES = d_es;
	setcrtselector(d_crt);
	clrscr();
	printxy(1,10,"We are in protected mode!");
	print(" Now jump back to real mode...");
	gotoxy(2,10);
	for(int i = 0; i < 55; i++) outchar('-');
	decprintxy(7,30,55555);
	switchRealMode(d_cstd,d_dstd);
	return 0;
}