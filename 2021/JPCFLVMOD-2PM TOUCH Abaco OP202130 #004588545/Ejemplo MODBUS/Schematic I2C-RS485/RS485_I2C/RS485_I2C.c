// Cuatro pulsadores de entrada (Derecha, Izquierda, arriba y abajo).
// Reloj con XT de 4 MHz.
// Programación para Autoclave con Calderin
// Tiene Menú:Ciclo: Liquidos, Instrumental suave, Textil Instrumental y Caucho.
// Tiene 6 niveles programables para desfogue suave.
// Ing. Juan David Piñeros.
// JP Inglobal. 2015

#include <18F2550.h>
//#include <18F4620.h>
#device adc=10
#fuses XTPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,NOSTVREN,NOPUT,NOCPD,NOWRT,NOIESO,NOFCMEN,NOPBADEN,NOWRTC,NOWRTB,NOEBTR,NOEBTRB,NOCPB,NOXINST,NOBROWNOUT,NOLPT1OSC,NOMCLR,NOUSBDIV,NOVREGEN,PLL1
//#fuses HS,NOWDT,NOPROTECT,NOLVP,NODEBUG,NOSTVREN,NOPUT,NOCPD,NOWRT,NOIESO,NOFCMEN,NOPBADEN,NOWRTC,NOWRTB,NOEBTR,NOEBTRB,NOCPB,NOXINST,NOLPT1OSC,NOMCLR
#use delay(clock=4M)
#use i2c(slave, slow, sda=PIN_B0, scl=PIN_B1, address=0xB0)
//#use i2c(slave, slow, sda=PIN_C4, scl=PIN_C3, address=0xB0)

#define MODBUS_TYPE MODBUS_TYPE_MASTER
#define MODBUS_SERIAL_TYPE MODBUS_RTU     //use MODBUS_ASCII for ASCII mode
#define MODBUS_SERIAL_RX_BUFFER_SIZE 1023
#define MODBUS_SERIAL_BAUD 9600

//#ifndef USE_WITH_PC
//#use rs232(baud=9600, UART1, stream=PC, errors)
#use RS232(BAUD=9600, BITS=8, PARITY=N, XMIT=PIN_B4, RCV=PIN_B5,stream=PC, errors)
//#define MODBUS_SERIAL_INT_SOURCE MODBUS_INT_EXT
#define MODBUS_SERIAL_INT_SOURCE MODBUS_INT_RDA
#define MODBUS_SERIAL_TX_PIN PIN_C6   // Data transmit pin
#define MODBUS_SERIAL_RX_PIN PIN_C7   // Data receive pin
//The following should be defined for RS485 communication
#define MODBUS_SERIAL_ENABLE_PIN   PIN_C1   // Controls DE pin for RS485
#define MODBUS_SERIAL_RX_ENABLE    0   // Controls RE pin for RS485
#define DEBUG_MSG(msg) fprintf(PC, msg)
#define DEBUG_DATA(msg,data) fprintf(PC, msg, data)
//#else
//#define MODBUS_SERIAL_INT_SOURCE MODBUS_INT_EXT
//#define DEBUG_MSG(msg) if(0)
//#define DEBUG_DATA(msg,data) if(0)
//#endif

#include <modbus.c>

#define MODBUS_SLAVE_ADDRESS 0x01

// Variables para Pt100
int8 instruccion=0,address=0,regl=0,regh=0,i=0;
int16 reg=0;

int8 fstate;                     //Guardara el estado del bus I2C
int8 posicion, buffer[50], txbuf[50],rxbuf[50];     //Buffer de memoria

#INT_SSP
void ssp_interupt (){
   
   int incoming;                //Variable donde se recibe el byte que manda el maestro
   
   fstate = i2c_isr_state();    //Lectura del estado del bus I2c la interrupción

   /* Solicitud de lectura del esclavo por el master */
   if(fstate == 0x80) {         
       //Manda al maestro la información contenida en la posición de memoria que le ha solicitado
      i2c_write (txbuf[posicion]);
   }
/* Sino está solicitando lectura es que está enviando algo */
   else {                              //Sino es que hay dato en el bus I2C...
      incoming = i2c_read();           //... lo lee
      if (fState == 1) {          //Información recibida corresponde a la posicion
         posicion = incoming;          //Se guarda posición
      }
      else if (fState == 2) {          //Información recibida corresponde al dato
         rxbuf[posicion] = incoming;
      }
  }
}

void Lee_Vector(void){
   instruccion=rxbuf[0];
   address=rxbuf[1];
   regl=rxbuf[2];
   regh=rxbuf[3];
   reg=(regh*256)+regl;
}

/*This function may come in handy for you since MODBUS uses MSB first.*/
int8 swap_bits(int8 c)
{
   return ((c&1)?128:0)|((c&2)?64:0)|((c&4)?32:0)|((c&8)?16:0)|((c&16)?8:0)
          |((c&32)?4:0)|((c&64)?2:0)|((c&128)?1:0);
}

void print_menu()
{
   DEBUG_MSG("\r\nPick command to send\r\n1. Read all coils.\r\n");
   DEBUG_MSG("2. Read all inputs.\r\n3. Read all holding registers.\r\n");
   DEBUG_MSG("4. Read all input registers.\r\n5. Turn coil 6 on.\r\n6. ");
   DEBUG_MSG("Write 0x4444 to register 0x03\r\n7. Set 8 coils using 0x50 as mask\r\n");
   DEBUG_MSG("8. Set 2 registers to 0x1111, 0x2222\r\n9. Send unknown command\r\n");
}

void read_all_coils()
{
   DEBUG_MSG("Coils:\r\n");
   if(!(modbus_read_coils(MODBUS_SLAVE_ADDRESS,0,8)))
   {
      DEBUG_MSG("Data: ");
      /*Started at 1 since 0 is quantity of coils*/
      for(i=1; i < (modbus_rx.len); ++i)
         DEBUG_DATA("%X ", modbus_rx.data[i]);
      DEBUG_MSG("\r\n\r\n");
   }
   else
   {
      DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
   }
}

void read_all_inputs()
{
   DEBUG_MSG("Inputs:\r\n");
   if(!(modbus_read_discrete_input(MODBUS_SLAVE_ADDRESS,0,8)))
   {
      DEBUG_MSG("Data: ");
      /*Started at 1 since 0 is quantity of coils*/
      for(i=1; i < (modbus_rx.len); ++i)
         DEBUG_DATA("%X ", modbus_rx.data[i]);
      DEBUG_MSG("\r\n\r\n");
   }
   else
   {
      DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
   }
}

void read_all_holding(int8 start,int8 size)
{
   DEBUG_MSG("Holding Registers:\r\n");
   if(!(modbus_read_holding_registers(MODBUS_SLAVE_ADDRESS,start,size)))
   {
      DEBUG_MSG("Data: ");
      /*Started at 1 since 0 is quantity of coils*/
      for(i=1; i < (modbus_rx.len); ++i){
         DEBUG_DATA("%X ", modbus_rx.data[i]);
         if(i<20)
            txbuf[i]=modbus_rx.data[i];
      }
      DEBUG_MSG("\r\n\r\n");
   } 
   else
   {
      DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
      output_bit(PIN_B7,1);
   }
}

void read_all_input_reg(int8 start,int8 size)
{
   DEBUG_MSG("Input Registers:\r\n");
   if(!(modbus_read_input_registers(MODBUS_SLAVE_ADDRESS,start,size)))
   {
      DEBUG_MSG("Data: ");
      /*Started at 1 since 0 is quantity of coils*/
      for(i=1; i < (modbus_rx.len); ++i)
         DEBUG_DATA("%X ", modbus_rx.data[i]);       
       
       DEBUG_MSG("\r\n\r\n");

   }
   else
   {
      DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
      output_bit(PIN_B7,1);
   }
}

void write_coil()
{
   DEBUG_MSG("Writing Single Coil:\r\n");
   if(!(modbus_write_single_coil(MODBUS_SLAVE_ADDRESS,6,TRUE)))
   {
      DEBUG_MSG("Data: ");
      for(i=0; i < (modbus_rx.len); ++i)
         DEBUG_DATA("%X ", modbus_rx.data[i]);
      DEBUG_MSG("\r\n\r\n");
   }
   else
   {
      DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
   }   
}

void write_reg(int16 address,int16 reg)
{
   DEBUG_MSG("Writing Single Register:\r\n");
   if(!(modbus_write_single_register(MODBUS_SLAVE_ADDRESS,address,reg)))
   {
      DEBUG_MSG("Data: ");
      for(i=0; i < (modbus_rx.len); ++i)
         DEBUG_DATA("%X ", modbus_rx.data[i]);
      DEBUG_MSG("\r\n\r\n");
   }
   else
   {
      DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
      output_bit(PIN_B7,1);
   }
}

void write_coils()
{
   int8 coils[1] = { 0x50 };
   DEBUG_MSG("Writing Multiple Coils:\r\n");
   if(!(modbus_write_multiple_coils(MODBUS_SLAVE_ADDRESS,0,8,coils)))
   {
      DEBUG_MSG("Data: ");
      for(i=0; i < (modbus_rx.len); ++i)
         DEBUG_DATA("%X ", modbus_rx.data[i]);
      DEBUG_MSG("\r\n\r\n");
   }
   else
   {
      DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
   }   
}

void write_regs()
{
   int16 reg_array[2] = {0x1111, 0x2222};
   DEBUG_MSG("Writing Multiple Registers:\r\n");
   if(!(modbus_write_multiple_registers(MODBUS_SLAVE_ADDRESS,0,2,reg_array)))
   {
      DEBUG_MSG("Data: ");
      for(i=0; i < (modbus_rx.len); ++i)
         DEBUG_DATA("%X ", modbus_rx.data[i]);
      DEBUG_MSG("\r\n\r\n");
   }
   else
   {
      DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
   }   
}

void unknown_func()
{
   DEBUG_MSG("Trying unknown function\r\n");
   DEBUG_MSG("Diagnostic:\r\n");
   if(!(modbus_diagnostics(MODBUS_SLAVE_ADDRESS,0,0)))
   {
      DEBUG_MSG("Data:");
      for(i=0; i < (modbus_rx.len); ++i)
         DEBUG_DATA("%X ", modbus_rx.data[i]);
      DEBUG_MSG("\r\n\r\n");
   }
   else
   {
      DEBUG_DATA("<-**Exception %X**->\r\n\r\n", modbus_rx.error);
   }
}


void main()
{
   fState = 0;
   for (posicion=0;posicion<0x10;posicion++){
      buffer[posicion] = 0x00;
      txbuf[posicion] = 0x00;
      rxbuf[posicion] = 0x00;
   }
   DEBUG_MSG("\r\nInitializing...");
   modbus_init();
   DEBUG_MSG("...ready\r\n");
   Lee_Vector();
   enable_interrupts(INT_SSP);
   enable_interrupts(global);
   

while(true)
{
   DEBUG_DATA("Address: %02u ", address);
   Lee_Vector();   
   if(instruccion==3){
      read_all_holding(0,8);
   }
   if(instruccion==5){
      write_reg(address,reg);
   }  
}
}
