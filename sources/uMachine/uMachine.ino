#include "Arduino.h"
#include "6510_instructions.h"

CPU6510 cpu;
BUS *bus2 ;
static boolean top = false;
static boolean debugpnt = false;
static boolean debugpnt2 = true;

// Initialize USB Controller
//USBHost usb;
// Attach keyboard controller to USB
//KeyboardController keyboard(usb);

/*void TC0_Handler()
  {

  // otherwise you get infinite interrupts
  if ( cpu.isReady()) {
    cpu.exec();
    Serial.println(cpu.showMem(0x1020 , 0x102F));
    Serial.println(cpu.showRegisters());
  } else {
   Serial.println("CPU BUSY");
  }
  long dummy = REG_TC0_SR0; // vital - reading this clears some flag
  }
*/
void setup() {
  // put your setup code here, to run once:
  //unsigned short addrprog = 0;


  Serial.begin(115200);
  Serial.println("*************** uMachine ***************");
  delay(1000);

  cpu.reset();
  bus2 = cpu.getBus();

  //test read ROM/RAM
  //bus2->write8(0x01, 0x30); //disable ROM
  Serial.println("0xA000: 0x" + String(bus2->read8(0xA000), HEX)); //Check FIRST ROM BASIC DATA
  Serial.println("0x0001: 0x" + String(bus2->read16(0x0001), HEX)); //Show register 0x0001
  bus2->write8(0xd018, 0x15); //set $D018
  Serial.println(cpu.showMem(0xfffA, 0xffff)); //show vectors for CPU NMI/RST/IRQ

  Serial.println(cpu.showRegisters());
  Serial.println("D012:" + String(bus2->read8( 0xD012), HEX));
  Serial.println("VIC II registers: \n" + cpu.showMem(0xD000 , 0xD030));
  Serial.println("CPU running:" + String(cpu.isReady(), BIN));
  Serial.println("Start");

  cpu.start();

  top = true;
  /* pinMode(2, OUTPUT);   // port B pin 25
    analogWrite(2, 255);  // sets up some other registers I haven't worked out yet
    REG_PIOB_PDR = 1 << 25; // disable PIO, enable peripheral
    REG_PIOB_ABSR = 1 << 25; // select peripheral B
    REG_TC0_WPMR = 0x54494D00; // enable write to registers
    REG_TC0_CMR0 = 0b00000000000010011100010000000000; // set channel mode register (see datasheet)
    REG_TC0_RC0 = 34000; // counter period
    REG_TC0_RA0 = 10; // PWM value
    REG_TC0_CCR0 = 0b101;  // start counter
    REG_TC0_IER0 = 0b00010000; // enable interrupt on counter=rc
    REG_TC0_IDR0 = 0b11101111; // disable other interrupts

    NVIC_EnableIRQ(TC0_IRQn); // enable TC0 interrupts
  */


}


void keyPressed() {
  Serial.print("Pressed:  ");
  cpu.requestIRQ();
}
// This function intercepts key release
void keyReleased() {
  Serial.print("Released: ");

}

void loop() {

  //usb.Task();
  if ( top) {
    if ( cpu.isReady()) {
      //bus2->write8( 0xD012, 0x00);
      //Serial.println("[@PC:" + String(cpu.getPC() & 0xffff, HEX)+"]");
      if (debugpnt ) {
        Serial.println("--------------------\\");
        Serial.println(cpu.showRegisters());

        //Serial.println("0288:" + String(bus2->read8( 0x0288), HEX));
      }

      if ( (cpu.getPC() & 0xffff) == 0xE5B3) { //force D012 default value to 00
        bus2->write8( 0xD012, 0x00);
      }


      if ( (cpu.getPC() & 0xffff) == 0xE430) {   //<<-----------------
        debugpnt = true;
        Serial.println(cpu.showRegisters());
        Serial.println(cpu.showMem(0x002B , 0x0038));
      } else if ( (cpu.getPC() & 0xffff) == 0xE438) { //<<-----------------
        debugpnt = false;
        Serial.println("Screen registers: \n" + cpu.showMem(0x0062 , 0x0070));
      }



      if ( (cpu.getPC() & 0xffff) == 0x0073) { //0xA67A) {0xA68D

        if (debugpnt2 ) {
          Serial.println("[@PC:" + String(cpu.getPC() & 0xffff, HEX) + "]");
          Serial.println("D018:" + String(bus2->read8( 0xD018), HEX));
          Serial.println("0001:" + String(bus2->read8( 0x0001), HEX));
          Serial.println("Screen registers: \n" + cpu.showMem(0x0428 , 0x04F0));
          Serial.println("VIC II registers: \n" + cpu.showMem(0xD000 , 0xD030));
          Serial.println("CHRGOT: \n" + cpu.showMem(0x0073 , 0x008A));
          Serial.println("VECTORS: \n" + cpu.showMem(0x0314 , 0x032B));
          Serial.flush();
          debugpnt2 = false;
        } 
      }
      if (!debugpnt2) {
        Serial.println("[@PC:" + String(cpu.getPC() & 0xffff, HEX) + "]");
        Serial.println(cpu.showRegisters());
      }
      if (debugpnt ) {
        Serial.println("-------------------/\n ||");

      }
      cpu.exec();
    } else {
      Serial.println("CPU Busy");
    }
  }
}

