// Cuatro pulsadores de entrada (Derecha, Izquierda, arriba y abajo).
// Reloj con XT de 4 MHz.
// Programaci?n para Autoclave con Calderin
// Tiene Men?:Ciclo: Liquidos, Instrumental suave, Textil Instrumental y Caucho.
// Tiene 6 niveles programables para desfogue suave.
// Ing. Juan David Pi?eros.
// JP Inglobal. 2015

#include <18F2550.h>
#fuses XTPLL,WDT1024,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV4,VREGEN,NOSTVREN,NOPUT,NOCPD,NOWRT,NOIESO,NOFCMEN,NOPBADEN,NOWRTC,NOWRTB,NOEBTR,NOEBTRB,NOCPB,NOXINST,NOBROWNOUT,NOLPT1OSC,NOMCLR
//#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV1,VREGEN,NOMCLR
#use delay(clock=16000000)

#use i2c(slave, slow, sda=PIN_B0, scl=PIN_B1, address=0xB0)

#define   Buzzer_on           output_bit(PIN_A5,1)
#define   Buzzer_off          output_bit(PIN_A5,0)

#define MODBUS_TYPE MODBUS_TYPE_MASTER
#define MODBUS_SERIAL_TYPE MODBUS_RTU     //use MODBUS_ASCII for ASCII mode
#define MODBUS_SERIAL_RX_BUFFER_SIZE 1023
#define MODBUS_SERIAL_BAUD 9600

#use RS232(BAUD=9600, BITS=8, PARITY=N, XMIT=PIN_B4, RCV=PIN_B5,stream=PC, errors)
#define MODBUS_SERIAL_INT_SOURCE MODBUS_INT_RDA
#define MODBUS_SERIAL_TX_PIN PIN_C6   // Data transmit pin
#define MODBUS_SERIAL_RX_PIN PIN_C7   // Data receive pin
//The following should be defined for RS485 communication
#define MODBUS_SERIAL_ENABLE_PIN   PIN_B2   // Controls DE pin for RS485
#define MODBUS_SERIAL_RX_ENABLE    0   // Controls RE pin for RS485
#define DEBUG_MSG(msg) fprintf(PC, msg)
#define DEBUG_DATA(msg,data) fprintf(PC, msg, data)


#include <modbus.c>

#define MODBUS_SLAVE_ADDRESS_1 0x01
#define MODBUS_SLAVE_ADDRESS_2 0x02
#define MODBUS_SLAVE_ADDRESS_3 0x03

// Variables para Pt100
int8 instruccion=0,address=0,regl=0,regh=0,i=0,pwm1l=0,pwm1h=0,pwm2l=0,pwm2h=0,crc1=0,crc2=0,conteoError=0,conteoError2=0,conteoError3=0,temporal=0;
int16 reg=0,pwm1=0,pwm2=0;

int8 fstate;                     //Guardara el estado del bus I2C
int8 posicion, buffer[50], txbuf[90],rxbuf[50];     //Buffer de memoria

#INT_SSP
void ssp_interupt (){
   int8 incoming;                //Variable donde se recibe el byte que manda el maestro
   
   fstate = i2c_isr_state();    //Lectura del estado del bus I2c la interrupci?n

   if(fstate == 0x80) {         
       //Manda al maestro la informaci?n contenida en la posici?n de memoria que le ha solicitado
      i2c_write (txbuf[posicion]);
   }else {                              //Sino es que hay dato en el bus I2C...
      incoming = i2c_read();           //... lo lee
      if (fState == 1) {          //Informaci?n recibida corresponde a la posicion
         posicion = incoming;          //Se guarda posici?n
      }
      else if (fState == 2) {          //Informaci?n recibida corresponde al dato
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
   pwm1l=rxbuf[4];
   pwm1h=rxbuf[5];
   pwm2l=rxbuf[6];
   pwm2h=rxbuf[7];
   pwm1=(pwm1h*256)+pwm1l;
   pwm2=(pwm2h*256)+pwm2l;
   crc1=rxbuf[8];
   crc2=rxbuf[9];
}

int8 swap_bits(int8 c)
{
   return ((c&1)?128:0)|((c&2)?64:0)|((c&4)?32:0)|((c&8)?16:0)|((c&16)?8:0)
          |((c&32)?4:0)|((c&64)?2:0)|((c&128)?1:0);
}

void read_all_holding(int8 start,int8 size)
{   
   if(!(modbus_read_holding_registers(1,start,size))){ //Lee Variador #1
      txbuf[1]=(int8)modbus_rx.data[1];//Registro 1
      txbuf[2]=(int8)modbus_rx.data[2];//Registro 1
      txbuf[3]=(int8)modbus_rx.data[3];//Registro 2
      txbuf[4]=(int8)modbus_rx.data[4];//Registro 2
      txbuf[5]=(int8)modbus_rx.data[7];//Registro 4
      txbuf[6]=(int8)modbus_rx.data[8];//Registro 4
      txbuf[7]=(int8)modbus_rx.data[11];//Registro 6
      txbuf[8]=(int8)modbus_rx.data[12];//Registro 6
      txbuf[9]=(int8)modbus_rx.data[13];//Registro 7
      txbuf[10]=(int8)modbus_rx.data[14];//Registro 7
      txbuf[11]=(int8)modbus_rx.data[15];//Registro 8
      txbuf[12]=(int8)modbus_rx.data[16];//Registro 8
      txbuf[13]=(int8)modbus_rx.data[21];//Registro 11
      txbuf[14]=(int8)modbus_rx.data[22];//Registro 11
      txbuf[15]=(int8)modbus_rx.data[39];//Registro 20
      txbuf[16]=(int8)modbus_rx.data[40];//Registro 20
      txbuf[17]=(int8)modbus_rx.data[41];//Registro 21
      txbuf[18]=(int8)modbus_rx.data[42];//Registro 21
      txbuf[19]=(int8)modbus_rx.data[43];//Registro 22
      txbuf[20]=(int8)modbus_rx.data[44];//Registro 22
      txbuf[21]=(int8)modbus_rx.data[45];//Registro 23
      txbuf[22]=(int8)modbus_rx.data[46];//Registro 23
      txbuf[23]=(int8)modbus_rx.data[47];//Registro 24
      txbuf[24]=(int8)modbus_rx.data[48];//Registro 24
      conteoError=0;
   }else{
      conteoError++;
      if(conteoError>20){
         for(i=1; i < 25; ++i){         
            txbuf[i]=0;
         }
      }
   }
   
   if(!(modbus_read_holding_registers(2,start,size))){ //Lee Variador #2
      txbuf[25]=(int8)modbus_rx.data[1];//Registro 1
      txbuf[26]=(int8)modbus_rx.data[2];//Registro 1
      txbuf[27]=(int8)modbus_rx.data[3];//Registro 2
      txbuf[28]=(int8)modbus_rx.data[4];//Registro 2
      txbuf[29]=(int8)modbus_rx.data[7];//Registro 4
      txbuf[30]=(int8)modbus_rx.data[8];//Registro 4
      txbuf[31]=(int8)modbus_rx.data[11];//Registro 6
      txbuf[32]=(int8)modbus_rx.data[12];//Registro 6
      txbuf[33]=(int8)modbus_rx.data[13];//Registro 7
      txbuf[34]=(int8)modbus_rx.data[14];//Registro 7
      txbuf[35]=(int8)modbus_rx.data[15];//Registro 8
      txbuf[36]=(int8)modbus_rx.data[16];//Registro 8
      txbuf[37]=(int8)modbus_rx.data[21];//Registro 11
      txbuf[38]=(int8)modbus_rx.data[22];//Registro 11
      txbuf[39]=(int8)modbus_rx.data[39];//Registro 20
      txbuf[40]=(int8)modbus_rx.data[40];//Registro 20
      txbuf[41]=(int8)modbus_rx.data[41];//Registro 21
      txbuf[42]=(int8)modbus_rx.data[42];//Registro 21
      txbuf[43]=(int8)modbus_rx.data[43];//Registro 22
      txbuf[44]=(int8)modbus_rx.data[44];//Registro 22
      txbuf[45]=(int8)modbus_rx.data[45];//Registro 23
      txbuf[46]=(int8)modbus_rx.data[46];//Registro 23
      txbuf[47]=(int8)modbus_rx.data[47];//Registro 24
      txbuf[48]=(int8)modbus_rx.data[48];//Registro 24
      conteoError2=0;
   }else{
      conteoError2++;
      if(conteoError2>20){
         for(i=25; i < 49; ++i){         
            txbuf[i]=0;
         }
      }
   }
   
   if(!(modbus_read_holding_registers(3,start,size))){ //Lee Variador #3
      txbuf[50]=(int8)modbus_rx.data[1];//Registro 1
      txbuf[51]=(int8)modbus_rx.data[2];//Registro 1
      txbuf[52]=(int8)modbus_rx.data[3];//Registro 2
      txbuf[53]=(int8)modbus_rx.data[4];//Registro 2
      txbuf[54]=(int8)modbus_rx.data[7];//Registro 4
      txbuf[55]=(int8)modbus_rx.data[8];//Registro 4
      txbuf[56]=(int8)modbus_rx.data[11];//Registro 6
      txbuf[57]=(int8)modbus_rx.data[12];//Registro 6
      txbuf[58]=(int8)modbus_rx.data[13];//Registro 7
      txbuf[59]=(int8)modbus_rx.data[14];//Registro 7
      txbuf[60]=(int8)modbus_rx.data[15];//Registro 8
      txbuf[61]=(int8)modbus_rx.data[16];//Registro 8
      txbuf[62]=(int8)modbus_rx.data[21];//Registro 11
      txbuf[63]=(int8)modbus_rx.data[22];//Registro 11
      txbuf[64]=(int8)modbus_rx.data[39];//Registro 20
      txbuf[65]=(int8)modbus_rx.data[40];//Registro 20
      txbuf[66]=(int8)modbus_rx.data[41];//Registro 21
      txbuf[67]=(int8)modbus_rx.data[42];//Registro 21
      txbuf[68]=(int8)modbus_rx.data[43];//Registro 22
      txbuf[69]=(int8)modbus_rx.data[44];//Registro 22
      txbuf[70]=(int8)modbus_rx.data[45];//Registro 23
      txbuf[71]=(int8)modbus_rx.data[46];//Registro 23
      txbuf[72]=(int8)modbus_rx.data[47];//Registro 24
      txbuf[73]=(int8)modbus_rx.data[48];//Registro 24
      conteoError3=0;
   }else{
      conteoError3++;
      if(conteoError3>20){
         for(i=50; i < 74; ++i){         
            txbuf[i]=0;
         }
      }
   }
}

void write_reg(int16 address,int16 reg)
{
   /*
   DEBUG_MSG("Writing Single Register:\r\n");
   if(!(modbus_write_single_register(0x01,address,reg)))
   {
      DEBUG_MSG("Data: ");
      for(i=0; i < (modbus_rx.len); ++i)
         DEBUG_DATA("%X ", modbus_rx.data[i]);
      DEBUG_MSG("\r\n\r\n");
      Buzzer_off;
   }
   else
   {
      output_toggle(PIN_A5);
   }
   */
   modbus_write_single_register(1,address,reg);
   modbus_write_single_register(2,address,reg);
   modbus_write_single_register(3,address,reg);
}

void main()
{
   fState = 0;
   for (posicion=0;posicion<0x10;posicion++){
      buffer[posicion] = 0x00;
      txbuf[posicion] = 0x00;
      rxbuf[posicion] = 0x00;
   }
   enable_interrupts(INT_SSP);
   modbus_init();   
   setup_wdt(WDT_ON);
   setup_timer_2(T2_DIV_BY_16, 255, 16); 
   setup_ccp1(CCP_PWM);
   setup_ccp2(CCP_PWM);
   set_pwm1_duty (0);
   set_pwm2_duty (0);
   enable_interrupts(global);
   Lee_Vector();
   

while(true)
{
   restart_wdt();
   txbuf[0]=10;
   txbuf[49]=128;
   //temporal++;   
   set_pwm1_duty(pwm1);
   set_pwm2_duty(pwm2);
   //write_reg(1,temporal);
   if(crc1==txbuf[0] && crc2==txbuf[49]){
      if(instruccion==3){
         read_all_holding(0,25);
      }
      if(instruccion==5){
         write_reg(address,reg);
         /*
         if(address_ant!=address){
            address_ant=address;
            if(reg_ant!=reg){
               reg_ant=reg;
               write_reg(address,reg);
            }         
         }
         */
      }  
   }
   Lee_Vector();   
}
}
