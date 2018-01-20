#ifndef BUS_h
#define BUS_h

#define BLOCK_SIZE 8192
#define BLOCK_NUM 8

class BUS {

	private:

		  // byte **ram ;
		
	public:	



		BUS();
		//READ
		byte read8(unsigned short memAddr);
		unsigned short read16(unsigned short memAddr);
		//WRITE
		void write8(unsigned short memAddr, byte value);
		void write16(unsigned short memAddr, unsigned short value);

		byte readGD8(unsigned short memAddr);
		void writeGD8(unsigned short memAddr, byte value);
		/*
		//IRQ
		void assertIrq();  
		void clearIrq();  
		//MMI
		void assertNmi();
		void clearNmi();
		*/
};

#endif