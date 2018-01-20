
/*#include <avr/interrupt.h>
#include <avr/pgmspace.h>
*/
#include "Arduino.h"
#include "bus.h"
#include "rom_basic.h"
#include "rom_kernel.h"
#include "rom_char.h"
#include <malloc.h>

#define MEMBLOCK(addr) ((addr>>13)&0x07) 
#define MEMOFFSET(addr) (addr&0x1fff)


//static const byte ROM_KERNEL[8192];	
//static byte ram[BLOCK_NUM][BLOCK_SIZE];
static byte ram[65536];
BUS::BUS(){

	/*ram=(byte**)(malloc(BLOCK_NUM*sizeof(byte)));
	//alloc and clean memory
	for(int i=0;i<BLOCK_NUM;i++){
		//ram[i]=new byte[BLOCK_SIZE];
		ram[i]=(byte*)(malloc(sizeof(byte)*BLOCK_SIZE));
		for (int j=0;j<BLOCK_SIZE;j++){
			ram[i][j]=0;
		}
	}*/	

}
//****************
// RAM MEMORY
//****************
//READ RAM
byte BUS::read8(unsigned short memAddr){	
	
	//byte reg1=ram[0][1];
	byte reg1=ram[1];
	if ( (reg1 & 0x03) != 0){ //%x00: RAM visible in all three areas.
		if ( memAddr>=0xA000 && memAddr<=0xBFFF){ //BASIC ROM
			if ( (reg1 & 0x03) ==0x03 ) { //BASIC is visible
				return ROM_BASIC[(memAddr-0xA000)];
			}
		} else if (memAddr>=0xE000 && memAddr<=0xFFFF){//KERNEL ROM
			if ( (reg1 & 0x03) == 0x03 || (reg1 & 0x03) == 0x02) { //KERNEL is visible
				return ROM_KERNEL[(memAddr-0xE000)];
			}
		} /*else if ( memAddr>=0xD000 && memAddr<=0xDFFF ){ // CHAR ROM

		}*/
	}
	//return ram[MEMBLOCK(memAddr)][MEMOFFSET(memAddr)];
	return ram[memAddr];

}
unsigned short BUS::read16(unsigned short  memAddr){
	byte lval= read8(memAddr);
	memAddr++;
	byte hval= read8(memAddr);
	return (hval<<8)|(lval);
}
//WRITE RAM
void BUS::write8(unsigned short memAddr, byte value){
	
	if ( memAddr==1 ){
		// $0000 Processor port data direction register. Bits:
		// Bit #x: 0 = Bit #x in processor port can only be read; 1 = Bit #x in processor port can be read and written.
		byte result=(~ram[0] & ram[1]) | (value & ram[0] ); // B'C + AB   (A=value, B=RAM[0], C=RAM[1])
		ram[1]=result;
	} else {
		ram[memAddr]=value&0xff;	
	}
}
void BUS::write16(unsigned short memAddr, unsigned short value){ //low value is write on memaddr and high value on memaddr+1
	//ram[MEMBLOCK(memAddr)][MEMOFFSET(memAddr)]=value&0xff;
	ram[memAddr]=value&0xff;
	memAddr++;
	//ram[MEMBLOCK(memAddr)][MEMOFFSET(memAddr)]=value>>8;
	ram[memAddr]=value>>8;
}
//****************
// GD RAM MEMORY
//****************
//READ GAMEDUINO RAM
byte BUS::readGD8(unsigned short memAddr){
	return 0;
}
//WRITE GAMEDUINO RAM
void BUS::writeGD8(unsigned short memAddr, byte value){
		
}
//IRQ
/*void BUS::assertIrq(){
}  
void BUS::clearIrq(){
}  
//MMI
void BUS::assertNmi(){
}
void BUS::clearNmi(){
}*/
