/////////////////////////////////////////////////////////////////////////
////                        ex_modbus_master.c                       ////
////                                                                 ////
////    This is a simple test program for a modbus master device.    ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// Notes:                                                          ////
////                                                                 ////
//// Testing Example:                                                ////
//// To test the master code you will need a slave device loaded     ////
//// with the ex_modbus_slave.c program (Note that you can test it   ////
//// with PC software as well.  See Communicating with PC Software.) ////
//// After making the proper connections (refer to wiring diagram)   ////
//// you should power on the slave device and then power on the      ////
//// master device.  The master device should begin sending commands ////
//// out and showing the replies from the slave through the PC       ////
//// connection.                                                     ////
////                                                                 ////
//// Hardware UART:                                                  ////
//// If you use a hardware UART, make sure MODBUS_SERIAL_INT_SOURCE  ////
//// is set to MODBUS_INT_RDA or MODBUS_INT_RDA2 depending on which  ////
//// UART you are using.  Also, when using a hardware UART you do    //// 
//// not need to specify the TX and RX pins.  The driver figures     //// 
//// them out for you.                                               ////
////                                                                 ////
//// Communicating with PC software:                                 ////
//// To communicate with a PC, connect the board to your computer    ////
//// using the serial cable provided with your board.  Then          ////
//// uncomment the #define USE_WITH_PC 1 line.  In this              ////
//// configuration the PIC is the master and the PC is the slave.    ////
////                                                                 ////
//// Troubleshooting:                                                ////
//// If the device seems unresponsive, power cycle the board and     ////
//// wait 5 seconds.                                                 ////
////                                                                 ////
//// Wiring Diagram:                                                 ////
//// This is the diagram for the default configuration.  Note that   //// 
//// the external interrupt (INT), PIN B0, is the only pin that can  ////
//// be used for receiving with software RS232 for PCM and PCH, and  ////
//// for PCD the exteranl interrupt (INT0), usually PIN F6, is the   ////
//// only pin that can be used for receiving with software RS232.    ////
////                                                                 ////
////            PCH and PCM                        PCD               ////
////    ----------     ----------       ----------     ----------    ////
////    |        |     |        |       |        |     |        |    ////
////    | Master |     | Slave  |       | Master |     | Slave  |    ////
////    |        |     |        |       |        |     |        |    ////
////    |      B1|---->|B0      |       |      D8|---->|F6      |    ////
////    |      B0|<----|B1      |       |      F6|<----|D8      |    ////
////    ----------     ----------       ----------     ----------    ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2006 Custom Computer Services        ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

//#define USE_WITH_PC 1

//#include <18f4620.h>
#include <18f2550.h>
//#fuses HS,NOWDT,NOPROTECT,NOLVP,NODEBUG,NOSTVREN,NOPUT,NOCPD,NOWRT,NOIESO,NOFCMEN,NOPBADEN,NOWRTC,NOWRTB,NOEBTR,NOEBTRB,NOCPB,NOXINST,,NOLPT1OSC,NOMCLR
//#fuses XT,NOWDT,NOPROTECT,NOLVP,NODEBUG,NOSTVREN,NOPUT,NOCPD,NOWRT,NOIESO,NOFCMEN,NOPBADEN,NOWRTC,NOWRTB,NOEBTR,NOEBTRB,NOCPB,NOXINST,NOBROWNOUT,NOLPT1OSC,NOMCLR,NOUSBDIV,NOVREGEN,PLL1
//#fuses HS,NOWDT
//#use delay(clock=2M)
#fuses XTPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV4,VREGEN,NOSTVREN,NOPUT,NOCPD,NOWRT,NOIESO,NOFCMEN,NOPBADEN,NOWRTC,NOWRTB,NOEBTR,NOEBTRB,NOCPB,NOXINST,NOBROWNOUT,NOLPT1OSC,NOMCLR
#use delay(clock=16000000)
#use i2c(slave, slow, sda=PIN_B0, scl=PIN_B1, address=0xB0,force_HW, stream = i2cS)

#define   UP            !input(PIN_A5)
#define   DOWN          !input(PIN_E0)
#define   RIGHT         !input(PIN_E1)
#define   LEFT          !input(PIN_E2)

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
//#include "lcd.c"

#define MODBUS_SLAVE_ADDRESS 0x01

// Variables para Pt100
int8 instruccion=0,address=0,regl=0,regh=0,i=0;
int16 reg=0;

int8 fstate;                     //Guardara el estado del bus I2C
int8 posicion, buffer[50], txbuf[50],rxbuf[50];     //Buffer de memoria


#INT_SSP
void ssp_interupt (){
   
   int incoming;                //Variable donde se recibe el byte que manda el maestro
   
   fstate = i2c_isr_state(i2cs);    //Lectura del estado del bus I2c la interrupción

   /* Solicitud de lectura del esclavo por el master */
   if(fstate == 0x80) {         
       //Manda al maestro la información contenida en la posición de memoria que le ha solicitado
      i2c_write (i2cs,txbuf[posicion]);
   }
/* Sino está solicitando lectura es que está enviando algo */
   else {                              //Sino es que hay dato en el bus I2C...
      incoming = i2c_read(i2cs);           //... lo lee
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
      for(i=1; i < (modbus_rx.len); ++i)
         DEBUG_DATA("%X ", modbus_rx.data[i]);
      DEBUG_MSG("\r\n\r\n");
      output_bit(PIN_B7,0);
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
      output_bit(PIN_B7,0);
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
   char c;
   short subop=0;
   int8 menu=0,Opcion=0,pagina=0,dir=0,reg=0;
   fState = 0;
   for (posicion=0;posicion<0x10;posicion++){
      buffer[posicion] = 0x00;
      txbuf[posicion] = 0x00;
      rxbuf[posicion] = 0x00;
   }
   //lcd_init();
   //enable_interrupts(global);
   delay_ms(6);
   setup_adc_ports(NO_ANALOGS);
   output_bit(PIN_B3,1);
   DEBUG_MSG("\r\nInitializing...");
   modbus_init();
   DEBUG_MSG("...ready\r\n");
   Lee_Vector();
   enable_interrupts(INT_SSP);
/*
   lcd_gotoxy(1,1);   
   printf(lcd_putc,"Master                ");
   lcd_gotoxy(1,2);   
   printf(lcd_putc,"PIC Modbus            ");
   delay_ms(5000);
   printf(lcd_putc,"\f");
*/
   while(true){
      delay_ms(100);
      pagina++;
      DEBUG_DATA("Address: %02u\r\n ", address);
      DEBUG_DATA("Inst: %02u\r\n ", instruccion);
      Lee_Vector();   
      txbuf[13]++;
      //if(instruccion==3)
         //read_all_holding(0,8);
      //if(instruccion==5)
         //write_reg(address,reg);
      /*
      lcd_gotoxy(1,1);
      printf(lcd_putc,"Dir:%03u",dir);
      lcd_gotoxy(1,2);   
      printf(lcd_putc,"Reg:%03u",reg);
      */
      //write_reg(1,pagina);
      /*
      if(menu==0){//Menu Inicial
         if(DOWN){
            delay_ms(100);
            if(Opcion<4)
               Opcion++;
         }
         if(UP){
            delay_ms(100);
            if(Opcion>0)
               Opcion--;
         }
         if(Opcion==0){
            lcd_gotoxy(1,1);   
            printf(lcd_putc,">Read COIL          ");
            lcd_gotoxy(1,2);   
            printf(lcd_putc," Read Inputs        ");      
            lcd_gotoxy(1,3);   
            printf(lcd_putc," Read Holding       ");      
            lcd_gotoxy(1,4);   
            printf(lcd_putc," Read Inputs Reg    ");      
         }else if(Opcion==1){
            lcd_gotoxy(1,1);   
            printf(lcd_putc," Read COIL          ");
            lcd_gotoxy(1,2);   
            printf(lcd_putc,">Read Inputs        ");      
            lcd_gotoxy(1,3);   
            printf(lcd_putc," Read Holding       ");      
            lcd_gotoxy(1,4);   
            printf(lcd_putc," Read Inputs Reg    ");      
         }else if(Opcion==2){
            lcd_gotoxy(1,1);
            printf(lcd_putc," Read COIL          ");
            lcd_gotoxy(1,2);   
            printf(lcd_putc," Read Inputs        ");      
            lcd_gotoxy(1,3);   
            printf(lcd_putc,">Read Holding       ");      
            lcd_gotoxy(1,4);   
            printf(lcd_putc," Read Inputs Reg    ");
         }else if(Opcion==3){
            lcd_gotoxy(1,1);
            printf(lcd_putc," Read COIL          ");
            lcd_gotoxy(1,2);   
            printf(lcd_putc," Read Inputs        ");      
            lcd_gotoxy(1,3);   
            printf(lcd_putc," Read Holding       ");      
            lcd_gotoxy(1,4);   
            printf(lcd_putc,">Read Inputs Reg    ");      
         }else if(Opcion==4){
            lcd_gotoxy(1,1);   
            printf(lcd_putc," Read Inputs        ");      
            lcd_gotoxy(1,2);   
            printf(lcd_putc," Read Holding       ");      
            lcd_gotoxy(1,3);   
            printf(lcd_putc," Read Inputs Reg    "); 
            lcd_gotoxy(1,4);   
            printf(lcd_putc,">Write Register     ");
         }
         if(RIGHT){
            delay_ms(100);
            menu=Opcion+1;
            printf(lcd_putc,"\f");
         }
      }else if(menu==1){//Read COIL
         read_all_coils();
      }else if(menu==2){//Read Inputs
         read_all_inputs();
      }else if(menu==3){//Read Holding
         read_all_holding(0,8);
         lcd_gotoxy(1,1);   
         lcd_gotoxy(1,1);   
         printf(lcd_putc,"R%u:%05Ld",pagina,make16((int8)modbus_rx.data[pagina+1],(int8)modbus_rx.data[pagina+2]));
         lcd_gotoxy(10,1);   
         printf(lcd_putc,"R%u:%05Ld",pagina+1,make16((int8)modbus_rx.data[pagina+3],(int8)modbus_rx.data[pagina+4]));
         lcd_gotoxy(1,2);   
         printf(lcd_putc,"R%u:%05Ld",pagina+2,make16((int8)modbus_rx.data[pagina+5],(int8)modbus_rx.data[pagina+6]));
         lcd_gotoxy(10,2);   
         printf(lcd_putc,"R%u:%05Ld",pagina+3,make16((int8)modbus_rx.data[pagina+7],(int8)modbus_rx.data[pagina+8]));
         lcd_gotoxy(1,3);   
         printf(lcd_putc,"R%u:%05Ld",pagina+3,make16((int8)modbus_rx.data[pagina+9],(int8)modbus_rx.data[pagina+10]));
         lcd_gotoxy(10,3);   
         printf(lcd_putc,"R%u:%05Ld",pagina+4,make16((int8)modbus_rx.data[pagina+11],(int8)modbus_rx.data[pagina+12]));
         lcd_gotoxy(1,4);   
         printf(lcd_putc,"R%u:%05Ld",pagina+5,make16((int8)modbus_rx.data[pagina+13],(int8)modbus_rx.data[pagina+14]));
         lcd_gotoxy(10,4);   
         printf(lcd_putc,"R%u:%05Ld",pagina+6,make16((int8)modbus_rx.data[pagina+15],(int8)modbus_rx.data[pagina+16]));
      }else if(menu==4){//Read Inputs Reg
         if(UP){
            delay_ms(100);
            if(pagina<4)
               pagina++;
         }
         if(DOWN){
            delay_ms(100);
            if(pagina>0)
               pagina--;
         }
         read_all_input_reg(pagina+0,8);
         lcd_gotoxy(1,1);   
         printf(lcd_putc,"R%u:%05Ld",pagina,make16((int8)modbus_rx.data[pagina+1],(int8)modbus_rx.data[pagina+2]));
         lcd_gotoxy(10,1);   
         printf(lcd_putc,"R%u:%05Ld",pagina+1,make16((int8)modbus_rx.data[pagina+3],(int8)modbus_rx.data[pagina+4]));
         lcd_gotoxy(1,2);   
         printf(lcd_putc,"R%u:%05Ld",pagina+2,make16((int8)modbus_rx.data[pagina+5],(int8)modbus_rx.data[pagina+6]));
         lcd_gotoxy(10,2);   
         printf(lcd_putc,"R%u:%05Ld",pagina+3,make16((int8)modbus_rx.data[pagina+7],(int8)modbus_rx.data[pagina+8]));
         lcd_gotoxy(1,3);   
         printf(lcd_putc,"R%u:%05Ld",pagina+3,make16((int8)modbus_rx.data[pagina+9],(int8)modbus_rx.data[pagina+10]));
         lcd_gotoxy(10,3);   
         printf(lcd_putc,"R%u:%05Ld",pagina+4,make16((int8)modbus_rx.data[pagina+11],(int8)modbus_rx.data[pagina+12]));
         lcd_gotoxy(1,4);   
         printf(lcd_putc,"R%u:%05Ld",pagina+5,make16((int8)modbus_rx.data[pagina+13],(int8)modbus_rx.data[pagina+14]));
         lcd_gotoxy(10,4);   
         printf(lcd_putc,"R%u:%05Ld",pagina+6,make16((int8)modbus_rx.data[pagina+15],(int8)modbus_rx.data[pagina+16]));
      }else if(menu==5){//Write Register
         if(LEFT){
            subop=!subop;
            output_bit(PIN_B7,1);
            delay_ms(1000);            
            output_bit(PIN_B7,0);
         }
         
         if(UP){
            delay_ms(100);            
               if(!subop)
                  dir++;
               if(subop)
                  reg++;
         }
         if(DOWN){
            delay_ms(100);
            if(!subop){
               if(dir>0)
                  dir--;
            }
            if(subop){
               if(reg>0)
                  reg--;
            }
         }
         lcd_gotoxy(1,1);   
         printf(lcd_putc,"Dir:%03u",dir);
         lcd_gotoxy(1,2);   
         printf(lcd_putc,"Reg:%03u",reg);
         if(RIGHT){
            delay_ms(100);
            write_reg(dir,reg);
         }
      }
      */
   }
}
