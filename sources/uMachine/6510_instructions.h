#ifndef INSTRUCTIONS_6510_h
#define INSTRUCTIONS_6510_h

#include "bus.h"

class CPU6510 {

	private:
		BUS bus;
		//NMI request
		boolean stateNMIRequest=false;
		//IRQ request
		boolean stateIRQRequest=false;
		//Status CPU
		boolean isready=false;
		//Instruction pointer
		unsigned short PC;
		//Registers
		byte  A=0, X=0, Y=0;
		short B;
		//Stack
		byte  STACK;
		//STATE
		byte state=0x20; // NVBDIZC
		//DATA
		byte data_inst[2];
	

		/* Process status register mnemonics */
	    static const int P_CARRY       = 0x00;
	    static const int P_ZERO        = 0x01;
	    static const int P_IRQ_DISABLE = 0x02;
	    static const int P_DECIMAL     = 0x03;
	    static const int P_BREAK       = 0x04;
	    // Bit 5 is always '1'
	    static const int P_OVERFLOW    = 0x06;
	    static const int P_NEGATIVE    = 0x07;
	protected:
		//MODES
		void instImplicite(byte inst);
	  	void instImmediat(byte inst);
		void instAbsoluAndZero(byte inst);
		void instAccumulator(byte inst);
		void instRelative(byte inst);
		//INSTRCUTIONS	
		void setArithmeticFlags(int reg); 
		byte ror(byte m);
		byte rol(byte m);
		byte asl(byte m);
		byte lsr(byte m);
		int adc(int operand);
		int adcDecimal( int operand);
		int sbc(int operand);
		int sbcDecimal(int operand);
		void cmp(int reg, int operand);
		void incrementPC();
		short getAddrRelativ(signed char offset);
		//STACK
		void stackPush(byte data) ;
		byte stackPop() ;
		int stackPeek() ;
		int getStackPointer() ;
		void setStackPointer(int offset) ;
		//STATE
		void setState(byte bit);
		void clrState(byte bit);
		boolean getState(byte bit);
		//INTERRUPS
		
		void handleNMI();
		void handleIRQ();
		void handleBREAK();
		void interrupt(short addIntVector);	

	public:
		// MNI Vector
		static const unsigned short NMI_VECTOR = 0xfffa;
		// Reset vector
		static const unsigned short RST_VECTOR = 0xfffc;
		// IRQ vector
		static const unsigned short IRQ_VECTOR = 0xfffe;

		//Interrupt request
		void requestNMI();
		void requestIRQ();
	
		//CPU action
		void reset(); 
		boolean exec();
		boolean isReady();		
		void start();
		void stop();
		BUS *getBus();
		short getPC();
	
		//Constructor
		CPU6510();
	
		//DEBUG
		void zeroMem(unsigned short memstart, unsigned short memend);
		String showMem(unsigned short memstart, unsigned short memend);
		String showRegisters();
};


#endif