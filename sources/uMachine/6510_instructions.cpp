
/*#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
*/
#include "Arduino.h"

#include "6510_instructions.h"

enum Mode {
  ACC = 0, // Accumulator
  AIX ,
  ABS ,    // Absolu
  ABX ,    // ABS,X
  ABY ,    // ABS,Y
  IMM ,    // Immediat
  IMP ,    // Implicte
  IND ,    // Indirect
  XIN ,    // (IND,X)
  INY ,    // (IND,Y)
  REL ,    // Relative
  ZPG ,    // Zero page
  AGD ,    // Addr GameDuino
  ZPX ,    // ZP,X
  ZPY ,    // ZP,Y
  ZPI ,	   // NOT USE YET 
  FR1 ,	   // NOT USE YET 
  NUL      // NULL
};


static const String opcodeNames[] = {
  "BRK", "ORA", "NOP", "NOP", "TSB", "ORA", "ASL",  "",  // 0x00-0x07
  "PHP", "ORA", "ASL", "NOP", "TSB", "ORA", "ASL",  "",  // 0x08-0x0f
  "BPL", "ORA", "ORA", "NOP", "TRB", "ORA", "ASL",  "",  // 0x10-0x17
  "CLC", "ORA", "INC", "NOP", "TRB", "ORA", "ASL",  "",  // 0x18-0x1f
  "JSR", "AND", "NOP", "NOP", "BIT", "AND", "ROL",  "",  // 0x20-0x27
  "PLP", "AND", "ROL", "NOP", "BIT", "AND", "ROL",  "",  // 0x28-0x2f
  "BMI", "AND", "AND", "NOP", "BIT", "AND", "ROL",  "",  // 0x30-0x37
  "SEC", "AND", "DEC", "NOP", "BIT", "AND", "ROL",  "",  // 0x38-0x3f
  "RTI", "EOR", "NOP", "NOP", "NOP", "EOR", "LSR",  "",  // 0x40-0x47
  "PHA", "EOR", "LSR", "NOP", "JMP", "EOR", "LSR",  "",  // 0x48-0x4f
  "BVC", "EOR", "EOR", "NOP", "NOP", "EOR", "LSR",  "",  // 0x50-0x57
  "CLI", "EOR", "PHY", "NOP", "NOP", "EOR", "LSR",  "",  // 0x58-0x5f
  "RTS", "ADC", "NOP", "NOP", "STZ", "ADC", "ROR",  "",  // 0x60-0x67
  "PLA", "ADC", "ROR", "NOP", "JMP", "ADC", "ROR",  "",  // 0x68-0x6f
  "BVS", "ADC", "ADC", "NOP", "STZ", "ADC", "ROR",  "",  // 0x70-0x77
  "SEI", "ADC", "PLY", "NOP", "JMP", "ADC", "ROR",  "",  // 0x78-0x7f
  "BRA", "STA", "NOP", "STB", "STY", "STA", "STX",  "",  // 0x80-0x87
  "DEY", "BIT", "TXA", "NOP", "STY", "STA", "STX",  "",  // 0x88-0x8f
  "BCC", "STA", "STA", "NOP", "STY", "STA", "STX",  "",  // 0x90-0x97
  "TYA", "STA", "TXS", "NOP", "STZ", "STA", "STZ",  "",  // 0x98-0x9f
  "LDY", "LDA", "LDX", "LDB", "LDY", "LDA", "LDX",  "",  // 0xa0-0xa7
  "TAY", "LDA", "TAX", "NOP", "LDY", "LDA", "LDX",  "",  // 0xa8-0xaf
  "BCS", "LDA", "LDA", "NOP", "LDY", "LDA", "LDX",  "",  // 0xb0-0xb7
  "CLV", "LDA", "TSX", "NOP", "LDY", "LDA", "LDX",  "",  // 0xb8-0xbf
  "CPY", "CMP", "NOP", "NOP", "CPY", "CMP", "DEC",  "",  // 0xc0-0xc7
  "INY", "CMP", "DEX", "NOP", "CPY", "CMP", "DEC",  "",  // 0xc8-0xcf
  "BNE", "CMP", "CMP", "NOP", "NOP", "CMP", "DEC",  "",  // 0xd0-0xd7
  "CLD", "CMP", "PHX", "NOP", "NOP", "CMP", "DEC",  "",  // 0xd8-0xdf
  "CPX", "SBC", "NOP", "NOP", "CPX", "SBC", "INC",  "",  // 0xe0-0xe7
  "INX", "SBC", "NOP", "NOP", "CPX", "SBC", "INC",  "",  // 0xe8-0xef
  "BEQ", "SBC", "SBC", "NOP", "NOP", "SBC", "INC",  "",  // 0xf0-0xf7
  "SED", "SBC", "PLX", "NOP", "NOP", "SBC", "INC",  ""   // 0xf8-0xff
};

static byte const instructionModes[] = {
  IMP, XIN, NUL, NUL, // 0x00-0x03
  ZPG, ZPG, ZPG, ZPG, // 0x04-0x07
  IMP, IMM, ACC, NUL, // 0x08-0x0b
  ABS, ABS, ABS, FR1, // 0x0c-0x0f
  REL, INY, ZPI, NUL, // 0x10-0x13
  ZPG, ZPX, ZPX, ZPG, // 0x14-0x17
  IMP, ABY, IMP, NUL, // 0x18-0x1b
  ABS, ABX, ABX, FR1, // 0x1c-0x1f
  ABS, XIN, NUL, NUL, // 0x20-0x23
  ZPG, ZPG, ZPG, ZPG, // 0x24-0x27
  IMP, IMM, ACC, NUL, // 0x28-0x2b
  ABS, ABS, ABS, FR1, // 0x2c-0x2f
  REL, INY, ZPI, NUL, // 0x30-0x33
  ZPX, ZPX, ZPX, ZPG, // 0x34-0x37
  IMP, ABY, IMP, NUL, // 0x38-0x3b
  NUL, ABX, ABX, FR1, // 0x3c-0x3f
  IMP, XIN, NUL, NUL, // 0x40-0x43
  NUL, ZPG, ZPG, ZPG, // 0x44-0x47
  IMP, IMM, ACC, NUL, // 0x48-0x4b
  ABS, ABS, ABS, FR1, // 0x4c-0x4f
  REL, INY, ZPI, NUL, // 0x50-0x53
  NUL, ZPX, ZPX, ZPG, // 0x54-0x57
  IMP, ABY, IMP, NUL, // 0x58-0x5b
  NUL, ABX, ABX, FR1, // 0x5c-0x5f
  IMP, XIN, NUL, NUL, // 0x60-0x63
  ZPG, ZPG, ZPG, ZPG, // 0x64-0x67
  IMP, IMM, ACC, NUL, // 0x68-0x6b
  IND, ABS, ABS, FR1, // 0x6c-0x6f
  REL, INY, ZPI, NUL, // 0x70-0x73
  ZPX, ZPX, ZPX, ZPG, // 0x74-0x77
  IMP, ABY, IMP, NUL, // 0x78-0x7b
  AIX, ABX, ABX, FR1, // 0x7c-0x7f
  REL, XIN, NUL, ABS, // 0x80-0x83
  ZPG, ZPG, ZPG, ZPG, // 0x84-0x87
  IMP, IMM, IMP, NUL, // 0x88-0x8b
  ABS, ABS, ABS, FR1, // 0x8c-0x8f
  REL, INY, ZPI, NUL, // 0x90-0x93
  ZPX, ZPX, ZPY, ZPG, // 0x94-0x97
  IMP, ABY, IMP, NUL, // 0x98-0x9b
  ABS, ABX, ABX, FR1, // 0x9c-0x9f
  IMM, XIN, IMM, ABS, // 0xa0-0xa3
  ZPG, ZPG, ZPG, ZPG, // 0xa4-0xa7
  IMP, IMM, IMP, NUL, // 0xa8-0xab
  ABS, ABS, ABS, FR1, // 0xac-0xaf
  REL, INY, ZPI, NUL, // 0xb0-0xb3
  ZPX, ZPX, ZPY, ZPG, // 0xb4-0xb7
  IMP, ABY, IMP, NUL, // 0xb8-0xbb
  ABX, ABX, ABY, FR1, // 0xbc-0xbf
  IMM, XIN, NUL, NUL, // 0xc0-0xc3
  ZPG, ZPG, ZPG, ZPG, // 0xc4-0xc7
  IMP, IMM, IMP, NUL, // 0xc8-0xcb
  ABS, ABS, ABS, FR1, // 0xcc-0xcf
  REL, INY, ZPI, NUL, // 0xd0-0xd3
  NUL, ZPX, ZPX, ZPG, // 0xd4-0xd7
  IMP, ABY, IMP, NUL, // 0xd8-0xdb
  NUL, ABX, ABX, FR1, // 0xdc-0xdf
  IMM, XIN, NUL, NUL, // 0xe0-0xe3
  ZPG, ZPG, ZPG, ZPG, // 0xe4-0xe7
  IMP, IMM, IMP, NUL, // 0xe8-0xeb
  ABS, ABS, ABS, FR1, // 0xec-0xef
  REL, INY, ZPI, NUL, // 0xf0-0xf3
  NUL, ZPX, ZPX, ZPG, // 0xf4-0xf7
  IMP, ABY, IMP, NUL, // 0xf8-0xfb
  NUL, ABX, ABX, FR1 // 0xfc-0xff
};

static const byte instructionSizes[] = {
  1, 2, 2, 1, 2, 2, 2, 2, 1, 2, 1, 1, 3, 3, 3, 3,   // 0x00-0x0f
  2, 2, 2, 1, 2, 2, 2, 2, 1, 3, 1, 1, 3, 3, 3, 3,   // 0x10-0x1f
  3, 2, 2, 1, 2, 2, 2, 2, 1, 2, 1, 1, 3, 3, 3, 3,   // 0x20-0x2f
  2, 2, 2, 1, 2, 2, 2, 2, 1, 3, 1, 1, 3, 3, 3, 3,   // 0x30-0x3f
  1, 2, 2, 1, 2, 2, 2, 2, 1, 2, 1, 1, 3, 3, 3, 3,   // 0x40-0x4f
  2, 2, 2, 1, 2, 2, 2, 2, 1, 3, 1, 1, 3, 3, 3, 3,   // 0x50-0x5f
  1, 2, 2, 1, 2, 2, 2, 2, 1, 2, 1, 1, 3, 3, 3, 3,   // 0x60-0x6f
  2, 2, 2, 1, 2, 2, 2, 2, 1, 3, 1, 1, 3, 3, 3, 3,   // 0x70-0x7f
  2, 2, 2, 3, 2, 2, 2, 2, 1, 2, 1, 1, 3, 3, 3, 3,   // 0x80-0x8f
  2, 2, 2, 1, 2, 2, 2, 2, 1, 3, 1, 1, 3, 3, 3, 3,   // 0x90-0x9f
  2, 2, 2, 3, 2, 2, 2, 2, 1, 2, 1, 1, 3, 3, 3, 3,   // 0xa0-0xaf
  2, 2, 2, 1, 2, 2, 2, 2, 1, 3, 1, 1, 3, 3, 3, 3,   // 0xb0-0xbf
  2, 2, 2, 1, 2, 2, 2, 2, 1, 2, 1, 1, 3, 3, 3, 3,   // 0xc0-0xcf
  2, 2, 2, 1, 2, 2, 2, 2, 1, 3, 1, 1, 3, 3, 3, 3,   // 0xd0-0xdf
  2, 2, 2, 1, 2, 2, 2, 2, 1, 2, 1, 1, 3, 3, 3, 3,   // 0xe0-0xef
  2, 2, 2, 1, 2, 2, 2, 2, 1, 3, 1, 1, 3, 3, 3, 3    // 0xf0-0xff
};


CPU6510::CPU6510(){
	isready=false;
	reset();
	
}
BUS *CPU6510::getBus(){
	return &bus;
}
void CPU6510::reset(){

 	//SET PC 
 	PC=this->bus.read16(RST_VECTOR);
 	
 	//REGISTER
 	this->A=0;
 	X=0;
 	Y=0;

 	//Stack
 	STACK=0xff;

 	//clear state bits
  	clrState(P_CARRY);
  	clrState(P_NEGATIVE);
  	clrState(P_DECIMAL);
  	clrState(P_OVERFLOW);
  	clrState(P_ZERO);
  	clrState(P_IRQ_DISABLE);
  	clrState(P_BREAK);

	bus.write8(0x00,0x2F);
  	bus.write8(0x01,0x37);

}
boolean CPU6510::exec(){
	isready=false;
	//INTERRUPTS
	if ( stateNMIRequest ){	//NMI INTERRUPT
		handleNMI();
	} else if (!getState(P_IRQ_DISABLE) && stateIRQRequest) {	//IRQ INTERRUPT
		handleIRQ();
	} 

	//read opcode
	byte inst = this->bus.read8(PC);



	byte inst_mode = instructionModes[inst];
	byte inst_size = instructionSizes[inst];

		
	incrementPC();
	for (int i = 0; i < inst_size -1; i++) {
        data_inst[i] = bus.read8(PC);
        //Serial.println("DATA["+String(i)+"]="+ data_inst[i]);
        // Increment PC after reading
        incrementPC();
	 }

	//debug code 

	/*if ( inst == 0x60 || inst ==0x00 ){
		
		if ( STACK==0xff) {
			return false;
		}

	} */

	/*Serial.print("#> "+opcodeNames[inst]);
	if ( inst_size >=2 ){
		Serial.print(" 0x"+String(this->data_inst[0],HEX));
	}
	if ( inst_size == 3){
		Serial.print(" 0x"+String(this->data_inst[1],HEX));
	}
	Serial.println();*/

	//end debug code 

	switch (inst_mode){
		case IMP:
			instImplicite(inst);
			break;
		case IMM:
			instImmediat(inst);
			break;
		case ZPG:	
			case ABS:
			case ABX:
			case ABY:
			case ZPX:
			case ZPY:
			case XIN:
			case INY:
			case IND:
			instAbsoluAndZero(inst);
			break;
		case ACC:
			instAccumulator(inst);
			break;
		case REL:
			instRelative(inst);
			break;
	}
 	isready=true;
	return true;
	
	

}

//DEBUG TOOLS
String CPU6510::showMem(unsigned short memstart, unsigned short memend){

	String data="";
    String datachar="";
    char my_str[2];
    my_str[1]=0;

	for(int i=0;i<=(memend-memstart);i++){
		
	
		if (i%8==0){
			data+=" "+datachar;
			data+="\n"+String(memstart+i,HEX)+" ";
			datachar=String("");
		}
		if (bus.read8(memstart+i)<0x10){
			data+="0";	
		}
		byte c=(bus.read8(memstart+i));
		data+=String(c,HEX)+" ";
		if ( c>0 && c<26){
			my_str[0]=c+64;
		} else {
			my_str[0]=c;
		}		
		
		if (my_str[0]>32 && my_str[0]<128){

			datachar+=String(my_str)+" ";
		} else {
			datachar+=". ";	
		}
	}

	return data+"\n";

}
/*void CPU6510::zeroMem(unsigned short memstart, unsigned short memend){
	for(int i=0;i<=(memend-memstart);i++){
		bus.write8(memstart+i,0x00);
	}	

}*/
//****************
// MODES
//****************	
String CPU6510::showRegisters(){
	String data="";

	data+="A:"+String(A,HEX)+" X:"+String(X,HEX)+" Y:"+String(Y,HEX)+" SP:"+String(STACK,HEX)+"\n";
	data+="PC:"+String(PC,HEX)+"\n\n";
	
	data+="NV-BDIZC\n";
	byte p=state;
	for(int i=0;i<8;i++){
		if ( p&0x80 ){
			data+="1";
		} else {
			data+="0";
		}
		p<<=1;
	}
	return data+"\n";	

}
void CPU6510::instImplicite(byte inst){

	switch(inst){
		case 0x68: //PLA
			A=stackPop();
			setArithmeticFlags(A);
			break;
		case 0x28: //PLP
			state=stackPop();
			if ( getState(P_BREAK) && !getState(P_IRQ_DISABLE)){
				handleBREAK();
			}


			break;
		case 0x08: //PHP
			stackPush(state | 0x10);  //Set BRK Flag
			break;
		case 0x48: //PHA
			stackPush(A);
			break;
		case 0xEA: //NOP
			break;
		case 0x00: //BRK
			incrementPC();
			handleBREAK();
			break;
		case 0x18: //CLC
			clrState(P_CARRY);
			break;
		case 0xD8: //CLD
			clrState(P_DECIMAL);
			break;
		case 0x58: //CLI
			clrState(P_IRQ_DISABLE);
			break;
		case 0xB8: //CLV
			clrState(P_OVERFLOW);
			break;
		case 0xCA: //DEX
			X=--X & 0xff;
			setArithmeticFlags(X);
			break;
		case 0x88: //DEY
			Y=--Y & 0xff;
			setArithmeticFlags(Y);
			break;
		case 0xE8: //INX
			X=++X & 0xff;
			setArithmeticFlags(X);
			break;
		case 0xC8: //INY
			Y=++Y & 0xff;
			setArithmeticFlags(Y);
			break;
		case 0x40: //RTI
			{
				state=stackPop();
				short tmp=stackPop();
				tmp|=(stackPop()<<8);
				PC=tmp;
			}
			break;
		case 0x60: //RTS
			{
				unsigned short tmp=stackPop();
				tmp&=0xff;
				tmp|=((stackPop()<<8)&0xff00);
				PC=tmp&0xffff;
			}
			break;
		case 0x38: //SEC
			setState(P_CARRY);
			break;
		case 0xF8: //SED
			setState(P_DECIMAL);
			break;
		case 0x78: //SEI
			setState(P_IRQ_DISABLE);
			break;
		case 0xAA: //TAX
			X=A;
			setArithmeticFlags(X);
			break;
		case 0xA8: //TAY
			Y=A;
			setArithmeticFlags(Y);
			break;
		case 0xBA: //TSX
		  	X = getStackPointer();
            setArithmeticFlags(X);
			break;
		case 0x8A: //TXA
			A=X;
			setArithmeticFlags(A);
			break;
		case 0x9A: //TXS
			setStackPointer(X);
			break;
		case 0x98: //TYA
			A=Y;
			setArithmeticFlags(A);
			break;	
	
		}
}
void CPU6510::instImmediat(byte inst){
	switch(inst){
		case 0xA9: //LDA
			A=this->data_inst[0];
			setArithmeticFlags(A);
			break;
		case 0xA2: //LDX
			X=this->data_inst[0];
			break;
		case 0xA0: //LDX
			Y=this->data_inst[0];	
			break;
		case 0x69: //ADC
			if ( getState(P_DECIMAL)==0 ){
				A=adc(this->data_inst[0]);			
			} else {
				A=adcDecimal(this->data_inst[0]);
			}
			break;	
		case 0x29: //AND
			A&=this->data_inst[0];
			setArithmeticFlags(A);
			break;
		case 0xC9: //CMP
			cmp(A,this->data_inst[0]);
			break;
		case 0xE0: //CPX
			cmp(X,this->data_inst[0]);
			break;
		case 0xC0: //CPY
			cmp(Y,this->data_inst[0]);
			break;
		case 0x49: //EOR
			A^=this->data_inst[0];
			setArithmeticFlags(A);
			break;
		case 0x09: //ORA
			A|=this->data_inst[0];
			setArithmeticFlags(A);
			break;
		case 0xE9: //SBC
			if ( getState(P_DECIMAL)==0 ){
				A=sbc(this->data_inst[0]);			
			} else {
				A=sbcDecimal(this->data_inst[0]);	
			}
			break;
	}	
}
//****************
// INSTRUCTIONS 
//****************
void CPU6510::instAbsoluAndZero(byte inst){
	unsigned short addr=0;
	switch(instructionModes[inst]){
		case ABS:
			addr=(data_inst[1]<<8)|(data_inst[0]);
			break;
		case ZPG:
			addr=(data_inst[0]);
			break;
		case ABX:
			addr=((data_inst[1]<<8)|(data_inst[0]))+X;
			break;
		case ABY:
			addr=((data_inst[1]<<8)|(data_inst[0]))+Y;
			break;
		case ZPX:
			addr=((data_inst[0])+X)&0xFF ;
			break;
		case ZPY:
			addr=((data_inst[0])+Y)&0xFF ;
			break;
		case XIN:
			{
				byte tmp= (data_inst[0] + X) ;
				addr=(bus.read8(tmp+1)<<8)|(bus.read8(tmp));	
			}
			break;
		case INY:
			{
				byte tmp=data_inst[0] & 0xff;
				addr=(((bus.read8(tmp+1)<<8)|(bus.read8(tmp)))+Y) ;
			}
			break;
		case IND: 
			{
				short tmp=((data_inst[1]<<8)|(data_inst[0]));
				addr=((bus.read8(tmp+1)<<8)|(bus.read8(tmp)));
			}
			break;
	}

	addr&=0xffff;

	switch(inst){
		case 0xAD: //LDA   ABS
		case 0xA5: //      Zero page
		case 0xBD: //      ABS,X
		case 0xB9: //      ABS,Y
		case 0xB5: //      ZP,X
		case 0xA1: //      (IND,X)
		case 0xB1: //      (IND),Y
			A=this->bus.read8(addr);
			setArithmeticFlags(A);
			break;
		case 0xAE: //LDX   ABS
		case 0xA6: //      Zero page
		case 0xBE: // 	   ABS,Y
		case 0xB6: // 	   ZP,Y
			X=this->bus.read8(addr);
			setArithmeticFlags(X);
			break;
		case 0xAC: //LDY   ABS
		case 0xA4: //      Zero Page 
		case 0xBC: //      ABS,X
		case 0xB4: //      ZP,X
			Y=this->bus.read8(addr);	
			setArithmeticFlags(Y);
			break;
		case 0x8C: //STY   ABS
		case 0x84: //      Zero Page     
		case 0x94: //      ZP,X
			this->bus.write8(addr,Y);	
			break;	
		case 0x8E: //STX   ABS
		case 0x86: //      Zero Page     
		case 0x96: //      ZP,Y
			this->bus.write8(addr,X);	
			break;	
		case 0x8D: //STA   ABS
 		case 0x85: //      Zero page
 		case 0x9D: //      ABS,X
 		case 0x99: //      ABS,Y
 		case 0x95: //      ZP,X
 		case 0x81: //      (IND,X)
 		case 0x91: //      (IND),Y
			this->bus.write8(addr,A);	
			break;
		case 0x6D: //ADC   ABS
		case 0x65: //      Zero page
		case 0x7D: //      ABS,X
		case 0x79: //      ABS,Y
		case 0x75: //      ZP,X
		case 0x61: //      (IND,X)
		case 0x71: //      (IND),Y
			if ( getState(P_DECIMAL)==0 ){
				A=adc(this->bus.read8(addr));
			} else {
				A=adcDecimal(this->bus.read8(addr));
			}
			break;
		case 0x2D: //AND   ABS
		case 0x25: //      Zero page
		case 0x3D: //      ABS,X
		case 0x39: //      ABS,Y
		case 0x35: //      ZP,X 
		case 0x21: //      (IND,X)
		case 0x31: //      (IND),Y
			A&=this->bus.read8(addr);
			setArithmeticFlags(A);
			break;
		case 0xCD: //CMP   ABS
		case 0xC5: //      Zero page
		case 0xDD: //      ABS,X
		case 0xD9: //      ABS,Y
		case 0xD5: //      ZP,X
		case 0xC1: //      (IND,X)
		case 0xD1: //      (IND),Y
			cmp(A,this->bus.read8(addr));
			break;
		case 0xEC: //CPX   ABS
		case 0xE4: //      Zero page
			cmp(X,this->bus.read8(addr));
			break;
		case 0xCC: //CPY   ABS 
		case 0xC4: //      Zero page
			cmp(Y,this->bus.read8(addr));
			break;
		case 0xCE: //DEC   ABS
		case 0xC6: //      Zero page
		case 0xDE: //      ABS,X
		case 0xD6: //      ZP,X
			this->bus.write8(addr,this->bus.read8(addr)-1);
        	setArithmeticFlags(this->bus.read8(addr)); 
			break;
		case 0xEE: //INC   ABS
		case 0xE6: //      ZeroPage
		case 0xFE: //      ABS,X
		case 0xF6: //      ZP,X 
			this->bus.write8(addr,this->bus.read8(addr)+1);
        	setArithmeticFlags(this->bus.read8(addr)); 
			break;
		case 0x4D: //EOR   ABS
		case 0x45: //      Zero page
		case 0x5D: //      ABS,X
		case 0x59: //      ABS,Y 
		case 0x55: //      ZP,X
		case 0x41: //      (IND,X)
		case 0x51: //      (IND),Y
			A^=this->bus.read8(addr);
			setArithmeticFlags(A);
			break;
		case 0x0D: //ORA   ABS
		case 0x05: //      Zero page
		case 0x1D: //      ABS,X
		case 0x19: //      ABS,Y
		case 0x15: //      ZP,X
		case 0x01: //      (IND,X)
		case 0x11: //      (IND),Y
			A|=this->bus.read8(addr);
			setArithmeticFlags(A);
			break;			
		case 0xED: //SBC   ABS
		case 0xE5: //      Zero page
		case 0xFD: //      ABS,X
		case 0xF9: //      ABS,Y
		case 0xF5: //      ZP,X		
		case 0xE1: //      (IND,X)
		case 0xF1: //      (IND),Y
			if ( getState(P_DECIMAL)==0 ){
				A=sbc(this->bus.read8(addr));			
			} else {
				A=sbcDecimal(this->bus.read8(addr));	
			}
			break;
		case 0x4E: //LSR   ABS
		case 0x46: //      Zero page
		case 0x5E: //      ABS,X
		case 0x56: //      ZP,X
			this->bus.write8(addr,lsr(this->bus.read8(addr)));
        	setArithmeticFlags(this->bus.read8(addr));        
			break;	
		case 0x0E: //ASL   ABS
		case 0x06: //      Zero page
		case 0x1E: //      ABS,X
		case 0x16: //      ZP,X		
        	this->bus.write8(addr,asl(this->bus.read8(addr)));
        	setArithmeticFlags(this->bus.read8(addr));
			break;
		case 0x6E: //ROR   ABS
		case 0x66: //      Zero page
		case 0x7E: //      ABS,X 
		case 0x76: //      ZP,X	
			this->bus.write8(addr,ror(this->bus.read8(addr)));
        	setArithmeticFlags(this->bus.read8(addr));  
			break;
		case 0x2E: //ROL   ABS
		case 0x26: //      Zero page
		case 0x3E: //      ABS,X
		case 0x36: //      ZP,X
			this->bus.write8(addr,rol(this->bus.read8(addr)));
        	setArithmeticFlags(this->bus.read8(addr));  	
			break;
		case 0x2C: //BIT   ABS
		case 0x24: //      Zero page
			{
				byte tmp=this->bus.read8(addr);
				clrState(P_ZERO);
				clrState(P_NEGATIVE);
				clrState(P_OVERFLOW);
				if ( (A & tmp)==0 ){
					setState(P_ZERO);
				}
				if ( (tmp & 0x80) !=0){
					setState(P_NEGATIVE);
				}
				if ( (tmp & 0x40) !=0){
					setState(P_OVERFLOW);
				}
			}
			break;
		case 0x4C: //JMP   ABS
		case 0x6C: //	   IND
 			PC=addr;
			break; 
		case 0x20: //JSR   ABS
			stackPush((PC>>8)); //hight
			stackPush((PC&0xFF)); //low
			PC=addr;
			break;
		case 0xA3: //LDB   ABS
 			B=bus.read16(addr);			
			break;
		case 0x83: //STB   ABS 
			bus.write16(addr,B);	
			break;
	}
}
void CPU6510::instAccumulator(byte inst){
	switch(inst){
		case 0x0A: //ASL
			A=asl(A);
			setArithmeticFlags(A);  
			break;
		case 0x4A: //LSR
			A=lsr(A);
			setArithmeticFlags(A);  
			break;
		case 0x2A: //ROL
			A=rol(A);
			setArithmeticFlags(A);  
			break;
		case 0x6A: //ROR
			A=ror(A);
			setArithmeticFlags(A);  
			break;
	}

}
void CPU6510::instRelative(byte inst){
	switch(inst){
		case 0x90: //BCC 
			if ( !getState(P_CARRY)){
				PC=getAddrRelativ(data_inst[0]);
			}
			break; //BCS
		case 0xB0: 
			if ( getState(P_CARRY)){
				PC=getAddrRelativ(data_inst[0]);
			}
			break;
		case 0xF0: //BEQ
			if ( getState(P_ZERO)){
				PC=getAddrRelativ(data_inst[0]);
			}
			break;
		case 0x30: //BMI 
			if ( getState(P_NEGATIVE)){
				PC=getAddrRelativ(data_inst[0]);
			}
			break;
		case 0xD0: //BNE
			if ( !getState(P_ZERO)){
				PC=getAddrRelativ(data_inst[0]);
			}
			break;
		case 0x10: //BPL
			if ( !getState(P_NEGATIVE)){
				PC=getAddrRelativ(data_inst[0]);
			}
			break;
		case 0x50: //BCV
			if ( !getState(P_OVERFLOW)){
				PC=getAddrRelativ(data_inst[0]);
			}
			break;
		case 0x70: //BVS
			if ( getState(P_OVERFLOW)){
				PC=getAddrRelativ(data_inst[0]);
			}
			break;
		}



}
void CPU6510::setArithmeticFlags(int reg) {
 	clrState(P_ZERO); //Set to ZERO   
    if (reg == 0){
    	setState(P_ZERO);
    }

    clrState(P_NEGATIVE); //Set to Zero
    if ((reg & 0x80) != 0){
    	setState(P_NEGATIVE);
    }
}
byte CPU6510::rol(byte m) {
        byte result = ((m << 1) | getState(P_CARRY) );
    	if ((m & 0x80) != 0 ){
    		setState(P_CARRY);
    	} else {
    		clrState(P_CARRY);
    	}
        return result;
    }
byte CPU6510::ror(byte m) {
        int result = (m >> 1) | (getState(P_CARRY) << 7) ;
    	if ((m & 0x01) != 0 ){
    		setState(P_CARRY);
    	} else {
    		clrState(P_CARRY);
    	}
        return result;
    }

byte CPU6510::asl(byte m){
	if ((m  & 0x80) != 0){
		setState(P_CARRY);
	} else {
        clrState(P_CARRY);
     }
	m<<=1;
	return m;
}
byte CPU6510::lsr(byte m){
	if ((m  & 0x01) != 0){
		setState(P_CARRY);
	} else {
        clrState(P_CARRY);
    }
	m>>=1;
	return m;
}
int CPU6510::adc(int operand) {
	int acc=A;
    int result = (operand & 0xff) + (acc & 0xff) + getState(P_CARRY);
    int carry6 = (operand & 0x7f) + (acc & 0x7f) + getState(P_CARRY);
    
    if ( (result & 0x100) != 0 ){
    	setState(P_CARRY);
    	//setState(P_OVERFLOW); 
	} else {
		clrState(P_CARRY);
	}

	if ( getState(P_CARRY) ^ ((carry6 & 0x80) != 0) ) {
		setState(P_OVERFLOW); 
	} else {
		clrState(P_OVERFLOW); 
	}
    
    result &= 0xff;
    setArithmeticFlags(result);
    return result;
}
int CPU6510::adcDecimal( int operand) {
	int acc=A;
    int l, h, result;
    l = (acc & 0x0f) + (operand & 0x0f) + getState(P_CARRY);
    if ((l & 0xff) > 9) l += 6;
    h = (acc >> 4) + (operand >> 4) + (l > 15 ? 1 : 0);
    if ((h & 0xff) > 9) h += 6;
    result = (l & 0x0f) | (h << 4);
    result &= 0xff;
    
    if (h > 15){
    	setState(P_CARRY); // N Flag is valid on CMOS 6502/65816
    } else {
    	clrState(P_CARRY);
    }
  
    if (result == 0){
    	setState(P_ZERO); // N Flag is valid on CMOS 6502/65816
    } else {
    	clrState(P_ZERO);
    }

    clrState(P_OVERFLOW); // BCD never sets overflow flag

    /*if (behavior == CpuBehavior.NMOS_6502||
        behavior == CpuBehavior.NMOS_WITH_ROR_BUG) {
            setNegativeFlag(false); // BCD is never negative on NMOS 6502
    }
    else {*/
	//clrState(P_NEGATIVE);
    if ((result & 0x80) != 0){
    	setState(P_NEGATIVE); // N Flag is valid on CMOS 6502/65816
    } else {
    	clrState(P_NEGATIVE);
    }
    //}
    return result;
}
int CPU6510::sbc(int operand) {
	int acc=A;
    int result;
    result = adc(~operand);
    setArithmeticFlags(result);
    return result;
}
int CPU6510::sbcDecimal(int operand) {
	int acc=A;
    int l, h, result;
    l = (acc & 0x0f) - (operand & 0x0f) - (getState(P_CARRY)? 0 : 1);
    if ((l & 0x10) != 0) l -= 6;
    h = (acc >> 4) - (operand >> 4) - ((l & 0x10) != 0 ? 1 : 0);
    if ((h & 0x10) != 0) h -= 6;
    result = (l & 0x0f) | (h << 4) & 0xff;

    if ((h & 0xff) < 15){
    	setState(P_CARRY); // N Flag is valid on CMOS 6502/65816
    } else {
    	clrState(P_CARRY);
    }
  
    if (result == 0){
    	setState(P_ZERO); // N Flag is valid on CMOS 6502/65816
    } else {
    	clrState(P_ZERO);
    }

    clrState(P_OVERFLOW); // BCD never sets overflow flag

    /*if (behavior == CpuBehavior.NMOS_6502||
        behavior == CpuBehavior.NMOS_WITH_ROR_BUG) {
            setNegativeFlag(false); // BCD is never negative on NMOS 6502
    }
    else {*/
    if ((result & 0x80) != 0){
		setState(P_NEGATIVE); // N Flag is valid on CMOS 6502/65816
    } else {
    	clrState(P_NEGATIVE);
    }
    //}
    return (result & 0xff);
}
void CPU6510::cmp(int reg, int operand) {
    int tmp = (reg - operand) & 0xff;
    if ( reg >= operand) {
		setState(P_CARRY);
	} else {
		clrState(P_CARRY);
	}
    if ( tmp==0){
    	setState(P_ZERO);
    } else {
    	clrState(P_ZERO);
    }
    if ( (tmp & 0x80) != 0){
   		setState(P_NEGATIVE);
   } else {
   		clrState(P_NEGATIVE);
   }
}
void CPU6510::incrementPC() {
    if (PC == 0xffff) {
        PC = 0;
    } else {
        ++PC;
    }
}
short CPU6510::getAddrRelativ(signed char offset){
	return ((PC+ offset) & 0xffff);

}
//****************
// STACK FUNCTIONS
//****************
void CPU6510::stackPush(byte data)  {
   
    bus.write8(0x100 + STACK,data);
    if ( STACK == 0) {
        STACK = 0xff;
    } else {
        STACK-=1;
    }
   
}
byte CPU6510::stackPop()  {
	if (STACK == 0xff) {
        STACK = 0x00;
    } else {
        STACK+=1;
    }
    return this->bus.read8(0x100 + STACK);
}
int CPU6510::stackPeek() {
    return this->bus.read8(0x100 + STACK + 1 );
}
int CPU6510::getStackPointer() {
    return STACK;
}
void CPU6510::setStackPointer(int offset) {
    STACK = offset;
}
//****************
// STATE FUCNTIONS
//****************
void CPU6510::setState(byte bit){
	state|= (1<<bit);
}
void CPU6510::clrState(byte bit){
	state&= ~(1 << bit); 
}
boolean CPU6510::getState(byte bit){
	byte res=state&(1<<bit);
	if (res==0){
		return 0;
	} 
	return 1;
}
//********************
// INTERRUPT FUCNTIONS
//********************
void CPU6510::requestNMI(){
	stateNMIRequest=true;
}
void CPU6510::requestIRQ(){
	stateIRQRequest=true;
}
void CPU6510::handleNMI(){
	clrState(P_BREAK);
	stateNMIRequest=false;
	interrupt(NMI_VECTOR);
}
void CPU6510::handleIRQ(){
	clrState(P_BREAK);
	stateIRQRequest=false;
	interrupt(IRQ_VECTOR);
}
void CPU6510::handleBREAK(){
	Serial.println("************ BRK INTERRUPT ************");
	setState(P_BREAK);
	stateIRQRequest=false;
	interrupt(IRQ_VECTOR);
}
void CPU6510::interrupt(short  addIntVector){

	//Save PC in stack
	stackPush((PC>>8)&0xff);
	stackPush((PC)&0xff);
	//Save cpu state in stack
	stackPush(state);
	//Disable IRQ
	setState(P_IRQ_DISABLE);
	//clear decimal mode
	clrState(P_DECIMAL);
	//execute interrupt code
	PC=this->bus.read16(addIntVector);

}
boolean CPU6510::isReady(){
	return isready;
}
short CPU6510::getPC(){
	return PC;
}
void CPU6510::start(){
	isready=true;
}
void CPU6510::stop(){
	isready=false;
}