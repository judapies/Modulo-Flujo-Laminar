//Programa para Cabina de Bioseguridad

#include <18F4685.h>
#device adc=10
#device HIGH_INTS=TRUE //Activamos niveles de prioridad
#fuses HS,WDT8192,NOPROTECT,NOLVP,NODEBUG,NOSTVREN,NOPUT,NOCPD,NOWRT,NOIESO,NOFCMEN,NOPBADEN,NOWRTC,NOWRTB,NOEBTR,NOEBTRB,NOCPB,NOXINST,NOLPT1OSC,NOMCLR,NOBROWNOUT
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, bits=8, parity=N,errors)
#use i2c(Master,slow,sda=PIN_D7,scl=PIN_D6)
#priority rda
#include <MATH.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <DS1302.c>
#include "FuncionesDisplay.h"

#define   ARRIBA           !input(PIN_B4)
#define   ABAJO            !input(PIN_B5)
#define   DERECHA          !input(PIN_B6)
#define   IZQUIERDA        !input(PIN_B7)
#define   VIDRIOUP         input(PIN_E1)
#define   VIDRIODN         input(PIN_E0)

#define   Alarma_on           output_bit(PIN_C0,1)
#define   Alarma_off          output_bit(PIN_C0,0)
#define   LuzBlanca_on        output_bit(PIN_D0,0)
#define   LuzBlanca_off       output_bit(PIN_D0,1)
#define   LuzUV_on            output_bit(PIN_D1,0)
#define   LuzUV_off           output_bit(PIN_D1,1)
#define   Aux_on              output_bit(PIN_D2,0)
#define   Aux_off             output_bit(PIN_D2,1)
#define   Motor_on            output_bit(PIN_C3,1)
#define   Motor_off           output_bit(PIN_C3,0)
#define   Motor2_on           output_bit(PIN_C2,1)
#define   Motor2_off          output_bit(PIN_C2,0)
#define   Toma_on             output_bit(PIN_C1,1)
#define   Toma_off            output_bit(PIN_C1,0)
#define   Display_on          output_bit(PIN_D5,1)
#define   Display_off         output_bit(PIN_D5,0)

#define OFF 0
#define ON  1
#define RX_BUFFER_SIZE  10
char Rx_Buffer[RX_BUFFER_SIZE+1];
char Rx_Buffer2[RX_BUFFER_SIZE+1];
char RX_Wr_Index=0;
char RX_Rd_Index=0;
char RX_Counter=0;

#define TX_BUFFER_SIZE  24
char Tx_Buffer[TX_BUFFER_SIZE+1];
char TX_Wr_Index=0;
char TX_Counter=0;

#define RS485 

/* Tabla de Modulos, Primera Fila A?o Regular y el siguiente a?o Bisiesto
E   F   M   A   M   J   J   A   S   O   N   D
0   3   3   6   1   4   6   2   5   0   3   5
0   3   4   0   2   5   0   3   6   1   4   6
*/
int8 Regular[12]={0,3,3,6,1,4,6,2,5,0,3,5};
int8 Bisiesto[12]={0,3,4,0,2,5,0,3,6,1,4,6};

short imprimir=0,flag_blower=0,flag_toma=0,flag_luz=0,w=0,flag_uv=0,flag_alarma2=0,Alarmas=OFF,flagError=OFF;
short tiempo_purga=0,tiempo_trabajo=0,flag_latencia=0,entro=0,GuardaEEPROM=0,Minutos=OFF,Segundos=OFF,Latencia=0,tiempo_uv=0,guardauv=0;
short flagClave=OFF,Encendio=OFF,UP=OFF,DOWN=OFF,RIGHT=OFF,LEFT=OFF;
short guardatrabajo=0,Lectura=0,tiempo_postpurga=0;
int8 Menu=240,Opcion=1,negativo=10,G_l=0,G_h=0;
int8 q=0,r=0,l=0,h=0,Entero=0,Decimal1=0,Alarma=10,Alarma2=10,LuzUV=0,Vel1=0,Vel2=0;
int8 Year=18,Mes=9,Dia=13,Hora=0,Minuto=0,Segundo=0,dow=0,Dato_Exitoso=0,MenuAntt=8,OpcionMenu=0,instruccion=0,address=0,regl=0,regh=0,pwm1l=0,pwm1h=0,pwm2l=0,pwm2h=0;
int8 YearTx=0,MesTx=0,DiaTx=0,HoraTx=0,MinutoTx=0,dowTx=0,z=0,Dato2=0,Inicio=0,TipoClave=0,PantallaPrincipal=0,Digito=0,conteoError=0;
int8 sPurga=0,sPurgap=0,mPurga=0,mPurgap=0,sPPurga=0,sPPurgap=0,mPPurga=0,mPPurgap=0,sUV=0,mUV=0,sUVp=0,mUVp=0,UV2=10,Modulo=0,Opcion2=0;

unsigned int16 Temporal1=0,segundos_uv=0;

int16 t_latencia=0,minutos_uv=0,minutos_trabajo=0,G16=0,tClave=0,tup=0,tdown=0,tright=0,tleft=0,reg=0;
signed int  Tuv[4]={0,0,0,0};          // Tiempo de UV transcurrido(en Horas)
signed int  Ttrabajo[4]={0,0,0,0};        // Tiempo de trabajo transcurrido (en Horas)
signed int  Password[4]={0,0,0,0};        // Contrasena Almacenada de 4 digitos
signed int  Contrasena[4]={0,0,0,0};      // Contrasena de 4 digitos
float Inflow=0.0,Downflow=0.0,Filtro_Downflow=0.0,Filtro_Inflow=0.0,Diferencia=0.0,VInflow=0.0;
float Velocidad=0.0,x_uno=0.0;
float Temporal=0.0,Temporal2=0.0,UVTime=0.0,TrabajoTime=0.0,barraDn=0.0,barraIn=0.0,pInDn=0,pInIn=0,Qexhaust=0.0,Qdownflow=0.0;
//float t=378.463968402560,f=-18275.0402762787;//Sensor 2 Posible bueno

//float p1=8.0175,p2=-52.32,p3=135.13,p4=-65.076,p5=-24.203;
//%0 280 218 063
//float p1=1.4591,p2=-2.4919,p3=6.5292,p4=13.865,p5=-18.740;

//%0 281 002 537
float p1=2.2585,p2=-6.0214,p3=16.151,p4=12.094,p5=-23.765;

float promedio[20]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float promedio2[20]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float PromPresion[8]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float prom[10];
float zero_fabrica=199.0,zero_actual=0.0,Relacion=0.57,tempo=0.0;
float V1=0.0,G=2.0,Pa=71,Temp=20,TempK=0,HR=60,Psv=0,f=0,Xv=0,densidad=1,Z1=0,Pr=0;

int16 tiempos=0;
int8 i=0,l2=0,h2=0,Temperatura=0,tEncendido=0;
short Purga=OFF;
int8 VelMotor=0,VelMotor2=0,Prendio=10;

//Comunicacion//------------------------
byte SlaveA0Tx[0X20];
byte txbuf[100];
byte dato=0;            //Contendr? la informaci?n a enviar o recibir
byte direccion=0;       //Contendr? la direcci?n del esclavo en el bus I2C
byte posicion=0; 

typedef struct{
   float zero_actual;
   float Diferencia;
   int16 adc;
   int8 negativo ;
   float Velocidad;
   int8 l;
   int8 h;
   float Flujo;   
   float Ajuste;
   float promedio[20];   
}SensorFlujo;

typedef struct{
   int16 adc;
   float V1;
   float G;
   float Presion;
   float RPT100;
   int8 r;
   int8 h;   
   float Ajuste;   
   float PromPresion[10];
   float promediopresion;
}SensorPresion;

SensorFlujo Flujo[5];
SensorPresion Presion[5];
#include "Funciones.h"

#int_TIMER1 HIGH
void temp1s(void){
   set_timer1(62411);//5ms
   
   if(ARRIBA)
      tup++;
   else
      tup=0;
       
   if(ABAJO)
      tdown++;
   else
      tdown=0;
      
   if(DERECHA)
      tright++;
   else
      tright=0;
    
   if(IZQUIERDA)
      tleft++;
   else
      tleft=0;
      
   tiempos++;
   //if(pulsos>=120){ 
   if(tiempos>=200){ 
      tiempos=0;
      //pulsos=0;
      imprimir=1;Lectura=1;
      if(Menu==0 && TipoClave==0)
         tEncendido++;
      
      if(flag_blower==1)
         minutos_trabajo++; 
  
      if(tiempo_purga==1 || Purga){                  
         sPurga++;
         if(sPurga>59){
            sPurga=0;
            mPurga++;
         }
      }
      
      if(tiempo_postpurga){
         sPPurga++;
         if(sPPurga>59){
            sPPurga=0;
            mPPurga++;
         }
      }
   
      if(tiempo_uv==1){
         segundos_uv++;
         sUV++;
         if(sUV>59){
            sUV=0;
            mUV++;
         }
      }
   }
}

#int_rda 
void serial_rx_isr(){
   Dato2=fgetc();
   if(Dato2==0x65){//Inicio Comunicacion
      Inicio=1;
      RX_Wr_Index=0;
   }
   if(Inicio==1){
      Rx_Buffer2[RX_Wr_Index] = Dato2;
      RX_Wr_Index++;
   }
   if(RX_Wr_Index >= RX_BUFFER_SIZE){
      RX_Wr_Index =0;
      Inicio=0;
   }
   if(RX_Wr_Index==0){
      if(Rx_Buffer2[0]==0x65 && Rx_Buffer2[1]==0xff && Rx_Buffer2[2]==0xff && Rx_Buffer2[8]==0x00 && Rx_Buffer2[9]==0xff )
         Dato_Exitoso=5;
      else
         Dato_Exitoso=10;
   }
   //if(Menu==20)
   //  Alarma_on;
     
   //output_toggle(PIN_C0);  
}

void main(){
   Display_on;
   enable_interrupts(int_rda);
   enable_interrupts(global);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);
   enable_interrupts(INT_TIMER1);
   setup_wdt(WDT_ON);
   setup_adc_ports(AN0_TO_AN4);
   setup_adc(ADC_CLOCK_DIV_32);
   //port_b_pullups(true);
   setup_wdt(WDT_ON);
   VelMotor=0;
   VelMotor2=0;
   VelocidadMotor(0);
   Motor_off;  
   delay_ms(1);
   rtc_init();  
   printf("page Bienvenida");
   SendDataDisplay();     
   Motor_off;
   LuzBlanca_off;Alarma_off;
//-------Inicio de Cabina-------------------------------------------------------------------------//   
   Motor2_off;
   LeeEEPROM(); // Lectura de variables almacenadas en la EEPROM 
   LimitaValores();
   delay_ms(100);
   LeerBotones();
   UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
   VelocidadMotor(0);
   
   while(true){ 
      LeerBotones();
      restart_wdt();
      LeeDisplay();
   
//**Actualizaci?n de Lectura de Sensores**//   
   if(Lectura==1){
      Lectura=0;
      rtc_get_date(Dia,Mes,Year,dow);
      rtc_get_time(Hora,Minuto,Segundo);
      InFlow=LeerFlujo(10,1);
      DownFlow=InFlow*Relacion;
      //DownFlow=D6FW101(10,2);//LeerFlujo(8,2);
      //DownFlow=LeerFlujo(10,2);
      //Qdownflow=DownFlow*1709.41;//AreaFiltroDownflow=0.55742m^2*3600 48*18; Afiltro=0.47483776m^2*3600=1709.41
      //Qexhaust=InFlow*182.41;//m3/h, pi*r^2*Inflow*3600; 164.62 con 9"1/2 de Ducto
      //Qinflow=Qexhaust-Qdownflow;
      //VInflow=Qinflow/793.44;
      VInflow=InFlow;
      
      if(VInflow<0)
         VInflow=0;
      Filtro_InFlow=Leer_Sensor_Presion(8,3);
      //Filtro_InFlow=0;
      Filtro_DownFlow=Leer_Sensor_Presion(8,4);
      Temperatura=leeTemperatura(5);
      if(Alarma==5 || Alarma2==5 || Alarmas){
         if(flag_alarma2==0){
            Alarma_on;
            flag_alarma2=1;
         }else{
            Alarma_off;
            flag_alarma2=0;
         }
      }
      
      if(Alarma==10 && Alarma2==10){
         Alarma_off;
      }
      
      if(flag_latencia==1)   
         t_latencia++;         
   
      if(t_latencia>=15)
         Latencia=1;
   }
//----------------------------------------//

//**Contador de Tiempo de Trabajo del filtro**//
   if(minutos_trabajo>=3600){ // Tiempo de Trabajo
      minutos_trabajo=0; //Minutos Trabajo realmente son segundos.
      write_eeprom(37,make8(minutos_trabajo,0));
      delay_ms(20);
      write_eeprom(38,make8(minutos_trabajo,1));
      delay_ms(20);
      Ttrabajo[0]++;
      if(Ttrabajo[0]>9){
         Ttrabajo[0]=0;
         Ttrabajo[1]++;
      }
      if(Ttrabajo[1]>9){
         Ttrabajo[1]=0;
         Ttrabajo[2]++;
      }
      if(Ttrabajo[2]>9){
         Ttrabajo[2]=0;
         Ttrabajo[3]++;
      }
      if(Ttrabajo[3]>9){
         Ttrabajo[3]=0;
      }
      write_eeprom(16,Ttrabajo[0]);
      delay_ms(20);
      write_eeprom(17,Ttrabajo[1]);
      delay_ms(20);
      write_eeprom(18,Ttrabajo[2]);
      delay_ms(20);
      write_eeprom(19,Ttrabajo[3]);
      delay_ms(20);
   }else if(minutos_trabajo==900 || minutos_trabajo==1800 || minutos_trabajo==2700 ){
      write_eeprom(37,make8(minutos_trabajo,0));
      delay_ms(20);
      write_eeprom(38,make8(minutos_trabajo,1));
      delay_ms(20);
   }
//----------------------------------------//

//**Contador de Tiempo de Trabajo de UV**//
         if(segundos_uv>=60){
            segundos_uv=0;
            minutos_uv++;
         }
         
         if(minutos_uv>=60){
            minutos_uv=0;
            Tuv[0]++;
            if(Tuv[0]>9){
               Tuv[0]=0;
               Tuv[1]++;
            }
            if(Tuv[1]>9){
               Tuv[1]=0;
               Tuv[2]++;
            }
            if(Tuv[2]>9){
               Tuv[2]=0;
               Tuv[3]++;
            }
            if(Tuv[3]>9){
               Tuv[3]=0;
            }
            write_eeprom(39,minutos_uv);
            delay_ms(20);
            write_eeprom(12,Tuv[0]);
            delay_ms(20);
            write_eeprom(13,Tuv[1]);
            delay_ms(20);
            write_eeprom(14,Tuv[2]);
            delay_ms(20);
            write_eeprom(15,Tuv[3]);
            delay_ms(20);
         }else if(minutos_uv==15 || minutos_uv==30 || minutos_uv==45){
            write_eeprom(39,minutos_uv);
            delay_ms(20);
         }
//----------------------------------------//   
   if(Menu!=5){// Si no esta en el menu de estados apaga la alarma
      Alarma_off;
      Alarma=10;
      Alarma2=10;
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------
   if(Menu==240){ //Menu de Bienvenida.
      delay_ms(1500);
      printf("page Clave");
      SendDataDisplay();
      ApagaSalidas();
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
    if(Menu==0){ //Menu de Contrase?a de acceso.      
      
      entro=OFF;
      
      if(tEncendido>30){
         Encendio=OFF;
         Prendio=10;
         VelMotor=0;VelMotor2=0;LuzBlanca_off;LuzUV_off;Toma_off;Alarma_off;Motor_off;Motor2_off;Aux_off;
         printf("page Off");
         SendDataDisplay();
         UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         delay_ms(150);
      }
      
      if(RX_Buffer[4]==0x11 || LEFT){//11
         UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         if(TipoClave!=0){
            printf("page Ajustes");
            SendDataDisplay();
            PantallaPrincipal=0;
         }
      }
      
      if(Digito==1){
         printf("t0.txt=\"%u\"",Contrasena[0]);
         SendDataDisplay();
         printf("va0.val=1");
         SendDataDisplay();
      }else if(Digito==2){
         printf("t0.txt=\"%u%u\"",Contrasena[0],Contrasena[1]);
         SendDataDisplay();
         printf("va0.val=2");
         SendDataDisplay();
      }else if(Digito==3){
         printf("t0.txt=\"%u%u%u\"",Contrasena[0],Contrasena[1],Contrasena[2]);
         SendDataDisplay();
         printf("va0.val=3");
         SendDataDisplay();
      }else if(Digito==4){
         printf("t0.txt=\"%u%u%u%u\"",Contrasena[0],Contrasena[1],Contrasena[2],Contrasena[3]);
         SendDataDisplay();
         printf("va0.val=4");
         SendDataDisplay();
      }
      
      if(UP){
         delay_ms(200);
         if(UP){
         UP=OFF;
         tEncendido=0;
         if(Digito==0)
            Digito=1;
            if(Contrasena[Digito-1]<9)
               Contrasena[Digito-1]++;
         }
      }
      
      if(DOWN){
         delay_ms(200);
         if(DOWN){
            if(Digito==0)
               Digito=1;
            tEncendido=0;
            DOWN=OFF;
            if(Contrasena[Digito-1]>0)
               Contrasena[Digito-1]--;
         }
      }
      
      if(RIGHT){
         delay_ms(200);
         if(RIGHT){
            tEncendido=0;
            RIGHT=OFF;
            Digito++;
         }
      }
      
      if(Digito>4){
      Digito=0;
      tEncendido=0;
      printf("t0.txt=\"\"");
      SendDataDisplay();
         if(TipoClave==0){
            if(Contrasena[0]==3&&Contrasena[1]==8&&Contrasena[2]==9&&Contrasena[3]==2){ // Si Ingresa clave para reset general del sistema.
               write_eeprom(0,0);delay_ms(20);write_eeprom(1,0);delay_ms(20);// Reestablece a contrase?a de Fabrica y reinicia Programa.
               write_eeprom(2,0);delay_ms(20);write_eeprom(3,0);delay_ms(20);
               Encendio=OFF;
               Prendio=10;
               Password[0]=0;Password[1]=0;Password[2]=0;Password[3]=0;
               VelMotor=0;VelMotor2=0;LuzBlanca_off;LuzUV_off;Toma_off;Alarma_off;Motor_off;Motor2_off;Aux_off;
               printf("page Off");
               SendDataDisplay();
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
               delay_ms(150);
            }
            if((Contrasena[0]==Password[0])&&(Contrasena[1]==Password[1])&&(Contrasena[2]==Password[2])&&(Contrasena[3]==Password[3])){
               printf("page ClaveCorrecta");
               SendDataDisplay();
               calculaDensidad();
               Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }else{
               printf("page ClaveBad");
               SendDataDisplay();
               Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            } 
         }else if(TipoClave==1){
            if(Contrasena[0]==4&&Contrasena[1]==4&&Contrasena[2]==4&&Contrasena[3]==4){ // Si Ingresa clave de Servicio Tecnico
               printf("page Config");
               SendDataDisplay();
               Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }else{
               printf("page Ajustes");
               SendDataDisplay();
               Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }
         }else if(TipoClave==2){
            if((Contrasena[0]==Password[0])&&(Contrasena[1]==Password[1])&&(Contrasena[2]==Password[2])&&(Contrasena[3]==Password[3])){
               printf("page Clave");
               SendDataDisplay();
               printf("titulo.txt=\"Clave Nueva\"");
               SendDataDisplay();
               TipoClave=3;
               Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }else{
               printf("page Ajustes");
               SendDataDisplay();
               Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            } 
         }else if(TipoClave==3){
            printf("titulo.txt=\"Clave Nueva\"");
            SendDataDisplay();
            printf("page Ajustes");
            SendDataDisplay();
            if(!GuardaEEPROM){
               write_eeprom(0,Contrasena[0]);delay_ms(20);write_eeprom(1,Contrasena[1]);delay_ms(20);
               write_eeprom(2,Contrasena[2]);delay_ms(20);write_eeprom(3,Contrasena[3]);delay_ms(20);
               Password[0]=Contrasena[0];
               Password[1]=Contrasena[1];
               Password[2]=Contrasena[2];
               Password[3]=Contrasena[3];
               GuardaEEPROM=ON;
               Alarma_on;delay_ms(1000);Alarma_off;
               Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }
         }
      }
      
      if(TipoClave==3){
         printf("titulo.txt=\"Clave Nueva\"");
         SendDataDisplay();
         printf("doevents");
         SendDataDisplay();
      }else if(TipoClave==2){
         printf("titulo.txt=\"Clave Actual\"");
         SendDataDisplay();
         printf("doevents");
         SendDataDisplay();
      }else if(TipoClave==1){
         printf("titulo.txt=\"Clave Tecnico\"");
         SendDataDisplay();
         printf("doevents");
         SendDataDisplay();
      }else if(TipoClave==0){
         printf("titulo.txt=\"Ingrese Contrase?a\"");
         SendDataDisplay();
      }
      
      if(RX_Buffer[3]==0x0f){//0f, recibe caracteres ingresados desde el Display
         tEncendido=0;
         if(TipoClave==0){
            if(RX_Buffer[4]==0x33&&RX_Buffer[5]==0x38&&RX_Buffer[6]==0x39&&RX_Buffer[7]==0x32){ // Si Ingresa clave para reset general del sistema.
               write_eeprom(0,0);delay_ms(20);write_eeprom(1,0);delay_ms(20);// Reestablece a contrase?a de Fabrica y reinicia Programa.
               write_eeprom(2,0);delay_ms(20);write_eeprom(3,0);delay_ms(20);
               Encendio=OFF;
               Prendio=10;
               Password[0]=0;Password[1]=0;Password[2]=0;Password[3]=0;
               VelMotor=0;VelMotor2=0;LuzBlanca_off;LuzUV_off;Toma_off;Alarma_off;Motor_off;Motor2_off;Aux_off;
               printf("page Off");
               SendDataDisplay();
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
               delay_ms(150);
            }
            if(RX_Buffer[4]>=0x30 && RX_Buffer[5]>=0x30 && RX_Buffer[6]>=0x30 && RX_Buffer[7]>=0x30
            && RX_Buffer[4]<=0x39 && RX_Buffer[5]<=0x39 && RX_Buffer[6]<=0x39 && RX_Buffer[7]<=0x39){
            if((RX_Buffer[4]==Password[0]+0x30)&&(RX_Buffer[5]==Password[1]+0x30)&&(RX_Buffer[6]==Password[2]+0x30)&&(RX_Buffer[7]==Password[3]+0x30)){
                  printf("page ClaveCorrecta");
                  SendDataDisplay();
                  calculaDensidad();
                  UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
               }else{
                  printf("page ClaveBad");
                  SendDataDisplay();
                  UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
               } 
            }
         }else if(TipoClave==1){
            if(RX_Buffer[4]==0x34&&RX_Buffer[5]==0x34&&RX_Buffer[6]==0x34&&RX_Buffer[7]==0x34){ // Si Ingresa clave de Servicio Tecnico
               printf("page Config");
               SendDataDisplay();
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }else{
               printf("page Ajustes");
               SendDataDisplay();
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }
         }else if(TipoClave==2){
            if(RX_Buffer[4]>=0x30 && RX_Buffer[5]>=0x30 && RX_Buffer[6]>=0x30 && RX_Buffer[7]>=0x30
            && RX_Buffer[4]<=0x39 && RX_Buffer[5]<=0x39 && RX_Buffer[6]<=0x39 && RX_Buffer[7]<=0x39){
            if((RX_Buffer[4]==Password[0]+0x30)&&(RX_Buffer[5]==Password[1]+0x30)&&(RX_Buffer[6]==Password[2]+0x30)&&(RX_Buffer[7]==Password[3]+0x30)){
               printf("page Clave");
               SendDataDisplay();               
               printf("titulo.txt=\"Clave Nueva\"");
               SendDataDisplay();
               TipoClave=3;               
               GuardaEEPROM=0;
               RX_Buffer[3]=0x00;RX_Buffer2[3]=0x00;  
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }else{
               printf("page Ajustes");
               SendDataDisplay();              
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
               //RX_Buffer[3]=0x00;RX_Buffer2[3]=0x00;  
            } 
            }
         }else if(TipoClave==3){
            printf("titulo.txt=\"Clave Nueva\"");
            SendDataDisplay(); 
            if(RX_Buffer[4]>=0x30 && RX_Buffer[5]>=0x30 && RX_Buffer[6]>=0x30 && RX_Buffer[7]>=0x30
            && RX_Buffer[4]<=0x39 && RX_Buffer[5]<=0x39 && RX_Buffer[6]<=0x39 && RX_Buffer[7]<=0x39){
               if(!GuardaEEPROM){
                  write_eeprom(0,RX_Buffer[4]-0x30);delay_ms(20);write_eeprom(1,RX_Buffer[5]-0x30);delay_ms(20);
                  write_eeprom(2,RX_Buffer[6]-0x30);delay_ms(20);write_eeprom(3,RX_Buffer[7]-0x30);delay_ms(20);
                  Password[0]=RX_Buffer[4]-0x30;
                  Password[1]=RX_Buffer[5]-0x30;
                  Password[2]=RX_Buffer[6]-0x30;
                  Password[3]=RX_Buffer[7]-0x30;
                  GuardaEEPROM=ON;
                  Alarma_on;delay_ms(1000);Alarma_off;
               }
               printf("page Ajustes");
               SendDataDisplay();
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }else{
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
               printf("page ClaveBad");
               SendDataDisplay();
               for(i=0;i<10;i++){
                  Alarma_on;delay_ms(150);Alarma_off;delay_ms(150);
               }
            }
         }
      }
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==1){ //Menu Principal
         
         printf("fecha.txt=\"Fecha:%02u/%02u/20%02u    Hora: %02u:%02u:%02u  T:%02u?C\"",Dia,Mes,Year,Hora,Minuto,Segundo,Temperatura);
         SendDataDisplay();
         SendDataDisplay();
         GuardaEEPROM=OFF;
         LuzBlanca_off;LuzUV_off;
         
         if(RIGHT){
            RIGHT=OFF;
            OpcionMenu++;
         }
         
         if(OpcionMenu>6)
            OpcionMenu=0;
         
         if(OpcionMenu==0){// No selecciono ninguno
            printf("b02.pic=74");
            SendDataDisplay();
            printf("b12.pic=78");
            SendDataDisplay();
            printf("b22.pic=79");
            SendDataDisplay();
            printf("b32.pic=77");
            SendDataDisplay();
            printf("b42.pic=76");
            SendDataDisplay();
            printf("b52.pic=75");
            SendDataDisplay();
            if(UP){
               printf("page Clave");
               SendDataDisplay();            
               TipoClave=1;
               UP=OFF;
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }
         }else if(OpcionMenu==1){// Selecciono Tiempo de Purga
            printf("b02.pic=72");
            SendDataDisplay();
            printf("b12.pic=78");
            SendDataDisplay();
            printf("b22.pic=79");
            SendDataDisplay();
            printf("b32.pic=77");
            SendDataDisplay();
            printf("b42.pic=76");
            SendDataDisplay();
            printf("b52.pic=75");
            SendDataDisplay();
            if(UP){
               printf("page TiempoPurga");
               SendDataDisplay();
               UP=OFF;
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }
         }else if(OpcionMenu==4){ //Selecciono Tiempo UV
            printf("b02.pic=74");
            SendDataDisplay();
            printf("b12.pic=73");
            SendDataDisplay();
            printf("b22.pic=79");
            SendDataDisplay();
            printf("b32.pic=77");
            SendDataDisplay();
            printf("b42.pic=76");
            SendDataDisplay();
            printf("b52.pic=75");
            SendDataDisplay();
            if(UP){
               printf("page TiempoUV");
               SendDataDisplay();
               UP=OFF;
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }
         }else if(OpcionMenu==2){//Selecciono Post purga
            printf("b02.pic=74");
            SendDataDisplay();
            printf("b12.pic=78");
            SendDataDisplay();
            printf("b22.pic=71");
            SendDataDisplay();
            printf("b32.pic=77");
            SendDataDisplay();
            printf("b42.pic=76");
            SendDataDisplay();
            printf("b52.pic=75");
            SendDataDisplay();
            if(UP){
               printf("page TiempoPostP");
               SendDataDisplay();
               UP=OFF;
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }
         }else if(OpcionMenu==5){//Selecciono Mantenimiento
            printf("b02.pic=74");
            SendDataDisplay();
            printf("b12.pic=78");
            SendDataDisplay();
            printf("b22.pic=79");
            SendDataDisplay();
            printf("b32.pic=70");
            SendDataDisplay();
            printf("b42.pic=76");
            SendDataDisplay();
            printf("b52.pic=75");
            SendDataDisplay();
            if(UP){
               printf("page Mto");
               SendDataDisplay();
               UP=OFF;
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }
         }else if(OpcionMenu==3){// Selecciono Fecha
            printf("b02.pic=74");
            SendDataDisplay();
            printf("b12.pic=78");
            SendDataDisplay();
            printf("b22.pic=79");
            SendDataDisplay();
            printf("b32.pic=77");
            SendDataDisplay();
            printf("b42.pic=69");
            SendDataDisplay();
            printf("b52.pic=75");
            SendDataDisplay();
            if(UP){
               printf("page Fecha");
               SendDataDisplay();
               UP=OFF;
               DiaTx=Dia;
               MesTx=Mes;
               YearTx=Year;
               HoraTx=Hora;
               MinutoTx=Minuto;
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }
         }else if(OpcionMenu==6){
            printf("b02.pic=74");
            SendDataDisplay();
            printf("b12.pic=78");
            SendDataDisplay();
            printf("b22.pic=79");
            SendDataDisplay();
            printf("b32.pic=77");
            SendDataDisplay();
            printf("b42.pic=76");
            SendDataDisplay();
            printf("b52.pic=68");
            SendDataDisplay();
            if(UP){
               printf("page Clave");
               SendDataDisplay();
               UP=OFF;
               TipoClave=2;
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }
         }
         
         if(RX_Buffer[4]==0x01){//Selecciono Tiempo Purga
            printf("page TiempoPurga");
            SendDataDisplay();
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         }
         
         if(RX_Buffer[4]==0x02){//Selecciono Tiempo UV
            printf("page TiempoUV");
            SendDataDisplay();  
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         }
         
         if(RX_Buffer[4]==0x03){//Selecciono Tiempo Post Purga
            printf("page TiempoPostP");
            SendDataDisplay();  
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         }
         
         if(RX_Buffer[4]==0x04){//Selecciono Mantenimiento
            printf("page Mto");
            SendDataDisplay();  
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         }
         
         if(RX_Buffer[4]==0x05){//Selecciono Hora y Fecha
            printf("page Fecha");
            SendDataDisplay();
            DiaTx=Dia;
            MesTx=Mes;
            YearTx=Year;
            HoraTx=Hora;
            MinutoTx=Minuto;
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         }
         
         if(RX_Buffer[4]==0x06){//Selecciono Cambio de Clave
            printf("page Clave");
            SendDataDisplay();            
            TipoClave=2;
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         }
         
         if(RX_Buffer[4]==0xa0){//Selecciono Servicio
            printf("page Clave");
            SendDataDisplay();            
            TipoClave=1;
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         }
         
         if(RX_Buffer[4]==0xb0){//Selecciono Regresar
            printf("page Principal");
            SendDataDisplay();     
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         }
         
         if(LEFT){
            delay_ms(200);
            if(LEFT){
               LEFT=OFF;
               printf("page Principal");
               SendDataDisplay(); 
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }
         }
      }
 //_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==2){ //Menu de Tiempo de Purga
         
         printf("fecha.txt=\"Fecha:%02u/%02u/20%02u    Hora: %02u:%02u:%02u  T:%02u?C\"",Dia,Mes,Year,Hora,Minuto,Segundo,Temperatura);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a){//Selecciono Minutos
            Minutos=ON;
            Segundos=OFF;
         }
         
         if(RX_Buffer[4]==0x0b){//Selecciono Segundos
            Segundos=ON;
            Minutos=OFF;
         }           
         
         if(RIGHT){
            delay_ms(200);
            if(RIGHT){
               RIGHT=OFF;
               if(!Segundos){
                  Segundos=ON;
                  Minutos=OFF;
                  printf("tsecest.pco=65535");
                  SendDataDisplay();  
                  printf("tminest.pco=0");
                  SendDataDisplay();  
               }else if(!Minutos){
                  Segundos=OFF;
                  Minutos=ON;
                  printf("tsecest.pco=0");
                  SendDataDisplay();  
                  printf("tminest.pco=65535");
                  SendDataDisplay();  
               }
            }
         }
         
         if(RX_Buffer[4]==0x11 || LEFT){//Selecciono Regresar
            delay_ms(200);
            if(RX_Buffer[4]==0x11 || LEFT){
               LEFT=OFF;
               Segundos=OFF;
               Minutos=OFF;
               printf("page Ajustes");
               SendDataDisplay();  
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
               if(!GuardaEEPROM){
                  write_eeprom(4,mPurgap);
                  delay_ms(10);
                  write_eeprom(5,sPurgap);
                  delay_ms(10);
                  //mPurga=mPurgap;
                  //sPurga=sPurgap;
                  GuardaEEPROM=ON;
               }
            }
         }
         
         if(RX_Buffer[4]==0x0c || RX_Buffer[4]==0x0d || UP){//Tecla arriba Oprimida
            delay_ms(10);
            if(RX_Buffer[4]==0x0c || RX_Buffer[4]==0x0d || UP){//Tecla arriba Oprimida
               UP=OFF;
               if(Segundos){
                  sPurgap++;
               }
               if(Minutos){
                  mPurgap++;
               }  
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;
            }
         }
         
         if(RX_Buffer[4]==0x0f || RX_Buffer[4]==0x0e || DOWN){//Tecla abajo oprimida
            delay_ms(10);
            if(RX_Buffer[4]==0x0f || RX_Buffer[4]==0x0e || DOWN){//Tecla abajo oprimida
               DOWN=OFF;
               if(Segundos && sPurgap>0){
                  sPurgap--;
               }
               if(Minutos){
                  mPurgap--;
               }
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;
            }
         }
         
         if(mPurgap<1)mPurgap=99;
         if(mPurgap>99)mPurgap=1;
         if(sPurgap>59)sPurgap=0;
            
         printf("tminest.txt=\"%02u\"",mPurgap);
         SendDataDisplay();
         printf("tsecest.txt=\"%02u\"",sPurgap);
         SendDataDisplay();
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==3){ //Menu de Tiempo de Post-Purga
         
         printf("fecha.txt=\"Fecha:%02u/%02u/20%02u    Hora: %02u:%02u:%02u  T:%02u?C\"",Dia,Mes,Year,Hora,Minuto,Segundo,Temperatura);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a){//Selecciono Minutos
            Minutos=ON;
            Segundos=OFF;
         }
         
         if(RX_Buffer[4]==0x0b){//Selecciono Segundos
            Segundos=ON;
            Minutos=OFF;
         }           
         
         if(RIGHT){
            delay_ms(200);
            if(RIGHT){
               RIGHT=OFF;
               if(!Segundos){
                  Segundos=ON;
                  Minutos=OFF;
                  printf("tsecest.pco=65535");
                  SendDataDisplay();  
                  printf("tminest.pco=0");
                  SendDataDisplay();  
               }else if(!Minutos){
                  Segundos=OFF;
                  Minutos=ON;
                  printf("tsecest.pco=0");
                  SendDataDisplay();  
                  printf("tminest.pco=65535");
                  SendDataDisplay();  
               }
            }
         }
         
         if(RX_Buffer[4]==0x11 || LEFT){//Selecciono Regresar
            delay_ms(200);
            if(RX_Buffer[4]==0x11 || LEFT){
               LEFT=OFF;
               Segundos=OFF;
               Minutos=OFF;
               printf("page Ajustes");
               SendDataDisplay();  
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
               if(!GuardaEEPROM){
                  write_eeprom(6,mPPurgap);
                  delay_ms(10);
                  write_eeprom(7,sPPurgap);
                  delay_ms(10);
                  //mPPurga=mPPurgap;
                  //sPPurga=sPPurgap;
                  GuardaEEPROM=ON;
               }
            }
         }        
         
         if(RX_Buffer[4]==0x0c || RX_Buffer[4]==0x0d || UP){//Tecla arriba Oprimida
            delay_ms(10);
            if(RX_Buffer[4]==0x0c || RX_Buffer[4]==0x0d || UP){//Tecla arriba Oprimida
               UP=OFF;
               if(Segundos){
                  sPPurgap++;
               }
               if(Minutos){
                  mPPurgap++;
               }  
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;
            }
         }
         
         if(RX_Buffer[4]==0x0f || RX_Buffer[4]==0x0e || DOWN){//Tecla abajo oprimida
            delay_ms(10);
            if(RX_Buffer[4]==0x0f || RX_Buffer[4]==0x0e || DOWN){//Tecla abajo oprimida
               DOWN=OFF;
               if(Segundos && sPPurgap>0){
                  sPPurgap--;
               }
               if(Minutos){
                  mPPurgap--;
               }
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;
            }
         }
         
         if(mPPurgap<1)mPPurgap=99;
         if(mPPurgap>99)mPPurgap=1;
         if(sPPurgap>59)sPPurgap=0;
            
         printf("tminest.txt=\"%02u\"",mPPurgap);
         SendDataDisplay();
         printf("tsecest.txt=\"%02u\"",sPPurgap);
         SendDataDisplay();
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==4){//Menu de Purga.
         
         tempo=((float)Vel1/(1.6666666))*10.0;
         reg=(int16)tempo;
         writeCommand(5,1,reg);
         
         Purga=ON;
         tiempo_purga=1;
         printf("titulo.txt=\"Purga\"");
         SendDataDisplay();
         
         LuzBlanca_off;//Alarma_off;
         printf("minPurga.txt=\"%02u\"",mPurga);
         SendDataDisplay();
         printf("secPurga.txt=\"%02u\"",sPurga);
         SendDataDisplay();
         printf("fecha.txt=\"Fecha:%02u/%02u/20%02u    Hora: %02u:%02u:%02u  T:%02u?C\"",Dia,Mes,Year,Hora,Minuto,Segundo,Temperatura);
         SendDataDisplay();
         
         VelocidadMotor(Vel1);
         
         if(VIDRIODN)
            Aux_on;
         else
            Aux_off;
         
         if(RX_Buffer[4]==0x11 || LEFT){
            delay_ms(1000);
            if(RX_Buffer[4]==0x11 || LEFT){
               LEFT=OFF;
               tiempo_purga=0;
               printf("page Principal");
               SendDataDisplay();
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
               flag_luz=1;flag_blower=1;flag_toma=1;
            }
         }
       
         if((sPurga>=sPurgap)&&(mPurga>=mPurgap)){
            tiempo_purga=0;flag_luz=1;flag_blower=1;flag_toma=1;
            printf("page Principal");
            SendDataDisplay();
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         }      
      }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==5){ //Menu General.
         tiempo_postpurga=0;
         sPPurga=0;
         mPPurga=0;
         Purga=OFF;
         w=0;
         UVTime=Tuv[0]+(Tuv[1]*10)+(Tuv[2]*100)+(Tuv[3]*1000);         
         VelMotor2=Vel2;
         
         if(imprimir==1){
            printf("fecha.txt=\"Fecha:%02u/%02u/20%02u    Hora: %02u:%02u:%02u  T:%02u?C\"",Dia,Mes,Year,Hora,Minuto,Segundo,Temperatura);
            SendDataDisplay();
            printf("fdown.txt=\"Presi?n Filtro DownFlow:%2.2finH20\"",Filtro_Downflow);
            SendDataDisplay();
            printf("fex.txt=\"Presi?n Pre-Filtro:%2.2finH20\"",Filtro_Inflow);
            SendDataDisplay();
            imprimir=0;
         }
      
// Dibujo de barra de estado de filtro                  
      barraDn=100.0*((Filtro_Downflow-pInDn)/(Presion[4].Ajuste));
      if(barraDn>100.0)
         barraDn=100.0;
      
      if(barraDn<0.0)
         barraDn=0.0;
      
      printf("pdown.txt=\"%03u%%\"",(int8)barraDn);
      SendDataDisplay();
      dibujaBarraDn();
      
      barraIn=100.0*((Filtro_Inflow-pInIn)/(Presion[3].Ajuste));
      if(barraIn>100.0)
         barraIn=100.0;
         
      if(barraIn<0.0)
         barraIn=0.0;
         
      printf("pex.txt=\"%03u%%\"",(int8)barraIn);
      SendDataDisplay();
      dibujaBarraIn();
      
      if(flag_luz==1){
         printf("b0.pic=67");
         SendDataDisplay();    
         LuzBlanca_on;
      }else{
         printf("b0.pic=9");
         SendDataDisplay(); 
         LuzBlanca_off;
      }
         
      if(flag_uv){
         flag_Luz=0;LuzBlanca_off;
         tiempo_uv=1;
         LuzUV_on;
         printf("b3.pic=14");
         SendDataDisplay(); 
         if(guardauv==1){
            guardauv=0;
            write_eeprom(39,minutos_uv);
            delay_ms(20);
            write_eeprom(12,Tuv[0]);
            delay_ms(20);
            write_eeprom(13,Tuv[1]);
            delay_ms(20);
            write_eeprom(14,Tuv[2]);
            delay_ms(20);
            write_eeprom(15,Tuv[3]);
            delay_ms(20);
         }               
      }else{
         LuzUV_off;tiempo_uv=0;
         printf("b3.pic=15");
         SendDataDisplay(); 
         if(guardauv==0){
            guardauv=1;
            write_eeprom(39,minutos_uv);
            delay_ms(20);
            write_eeprom(12,Tuv[0]);
            delay_ms(20);
            write_eeprom(13,Tuv[1]);
            delay_ms(20);
            write_eeprom(14,Tuv[2]);
            delay_ms(20);
            write_eeprom(15,Tuv[3]);
            delay_ms(20);
         }
      }
    
      if(Alarmas){
         printf("estado.txt=\"Revise Variadores\"");
         SendDataDisplay();    
         printf("icon.pic=64");
         SendDataDisplay();
      }else{
         if((barraDn>=90 && barraDn<=95) || (barraIn>=90 && barraIn<=95)){
            printf("estado.txt=\"Programe Cambio Filtros\"");
            SendDataDisplay();    
            printf("icon.pic=64");
            SendDataDisplay(); 
            Alarma2=10;
         }else if (barraDn>95 || barraIn>95){
            printf("estado.txt=\"Cambie los Filtros\"");
            SendDataDisplay();    
            printf("icon.pic=65");
            SendDataDisplay();    
            Alarma2=5;
         }else{
            printf("estado.txt=\"Operaci?n Segura\"");
            SendDataDisplay();    
            printf("icon.pic=63");
            SendDataDisplay(); 
            cambiaColor(0);
            Alarma2=10;
         } 
      }
                  
      if(flag_blower==1){
         printf("b2.pic=12");
         SendDataDisplay(); 
         tiempo_trabajo=1;
         VelocidadMotor(Vel1);
         if(guardatrabajo==1){
            guardatrabajo=0;
            write_eeprom(37,make8(minutos_trabajo,0));
            delay_ms(10);
            write_eeprom(38,make8(minutos_trabajo,1));
            delay_ms(10);
            write_eeprom(16,Ttrabajo[0]);
            delay_ms(10);
            write_eeprom(17,Ttrabajo[1]);
            delay_ms(10);
            write_eeprom(18,Ttrabajo[2]);
            delay_ms(10);
            write_eeprom(19,Ttrabajo[3]);
            delay_ms(10);            
         }
      }else{
         //Filtro_Downflow=0;
         printf("b2.pic=13");
         SendDataDisplay(); 
         tiempo_trabajo=0;
         VelocidadMotor(0);
         if(guardatrabajo==0){
            guardatrabajo=1;
            write_eeprom(37,make8(minutos_trabajo,0));
            delay_ms(10);
            write_eeprom(38,make8(minutos_trabajo,1));
            delay_ms(10);
            write_eeprom(16,Ttrabajo[0]);
            delay_ms(10);
            write_eeprom(17,Ttrabajo[1]);
            delay_ms(10);
            write_eeprom(18,Ttrabajo[2]);
            delay_ms(10);
            write_eeprom(19,Ttrabajo[3]);
            delay_ms(10);
         }
      }
      
      if(flag_toma==1){ // Si activo Toma
         printf("b1.pic=10");
         SendDataDisplay(); 
         Toma_on;
      }else{// Si desactivo Toma        
         printf("b1.pic=11");
         SendDataDisplay(); 
         Toma_off;
      }
      
      if(DOWN){
         delay_ms(10);
         if(DOWN){
            flag_luz=!flag_luz;
         }
      }
      
      if(RIGHT && LEFT){
         delay_ms(10);
         if(RIGHT && LEFT){
            RIGHT=OFF;LEFT=OFF;
            printf("page PostPurga");
            SendDataDisplay();
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         }
      }else{
         if(RIGHT){
            delay_ms(10);
            if(RIGHT){
               RIGHT=OFF;
               flag_blower=!flag_blower;
            }
         }
         
         if(LEFT){
            delay_ms(10);
            if(LEFT){
               LEFT=OFF;
               flag_uv=!flag_uv;
            }
         }
      }
      
      if(UP){//Si oprime boton de Toma.         
         delay_ms(500);
         if(UP){//Si oprime boton de Toma.
            UP=OFF;
            delay_ms(30);
            printf("page Ajustes");
            SendDataDisplay();
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         }
      }
      
      if(RX_Buffer[4]==0x01){         
         flag_luz=!flag_luz;
         RX_Buffer[4]=0x00;  
         RX_Buffer2[4]=0x00;
      }
      
      if(RX_Buffer[4]==0x02){
         flag_blower=!flag_blower;
         RX_Buffer[4]=0x00;  
         RX_Buffer2[4]=0x00;
      }
      
      if(RX_Buffer[4]==0x03){
         flag_toma=!flag_toma;
         RX_Buffer[4]=0x00;  
         RX_Buffer2[4]=0x00;
      }
      
      if(RX_Buffer[4]==0x04){
         flag_uv=!flag_uv;
         RX_Buffer[4]=0x00;  
         RX_Buffer2[4]=0x00;
      }
      
      if(RX_Buffer[4]==0x05){         
         printf("page Ajustes");
         SendDataDisplay();
         UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
      }
      
      if(RX_Buffer[4]==0x06){         
         printf("page PostPurga");
         SendDataDisplay();
         UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
      }
      
      if((sUV>=sUVp)&&(mUV>=mUVp)){
         if((sUVp==0)&&(mUVp==0)){
         }else{
            tiempo_uv=0;LuzUV=10;flag_uv=!flag_uv;
            sUV=0;mUV=0;UV2=0;
         }
      }
            
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==6){//Menu de Post-Purga.
         tiempo_postpurga=1;
         LuzBlanca_off;LuzUV_off;Toma_off;Alarma_off;
         printf("minPurga.txt=\"%02u\"",mPPurga);
         SendDataDisplay();
         printf("secPurga.txt=\"%02u\"",sPPurga);
         SendDataDisplay();
         
         VelMotor=Vel1;
         
         if(VIDRIODN && Prendio==20)
            Aux_on;
         else
            Aux_off;
         
         //if(Encendio)
            //Aux_on;
         
         printf("fecha.txt=\"Fecha:%02u/%02u/20%02u    Hora: %02u:%02u:%02u  T:%02u?C\"",Dia,Mes,Year,Hora,Minuto,Segundo,Temperatura);
         SendDataDisplay();
         
         if(RX_Buffer[4]==0x11 || LEFT){
            tiempo_postpurga=0;
            printf("page Principal");
            SendDataDisplay();
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            RX_Buffer[4]=0x00;  
            RX_Buffer2[4]=0x00;
            LEFT=OFF;
         }
       
         if((sPPurga>=sPPurgap)&&(mPPurga>=mPPurgap)){
            tiempo_postpurga=0;
            ApagaSalidas();
            //Display_off;
            Encendio=OFF;
            Prendio=10;
            VelMotor=0;VelMotor2=0;LuzBlanca_off;LuzUV_off;Toma_off;Alarma_off;Motor_off;Motor2_off;Aux_off;
            printf("page Off");
            SendDataDisplay();
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            delay_ms(150);
            //reset_cpu();
         }      
      }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==7){ //Menu de Configuraci?n de Fecha y Hora
         
         if(RX_Buffer[4]==0x0a){//Selecciono Hora
            Opcion=4;
         }
         
         if(RX_Buffer[4]==0x0b){//Selecciono Minuto
            Opcion=5;
         }
         
         if(RX_Buffer[4]==0x0c){//Selecciono Dia
            Opcion=1;
         }
         
         if(RX_Buffer[4]==0x0d){//Selecciono Hora
            Opcion=2;
         }
         
         if(RX_Buffer[4]==0x0e){//Selecciono Year
            Opcion=3;
         }
         
         if(RIGHT){
            delay_ms(200);
            if(RIGHT){
               RIGHT=OFF;
               Opcion++;
            }
         }
         if(Opcion>5)
            Opcion=1;
         if(Opcion<1)
            Opcion=5;
            
         if(Opcion==1){
            printf("tdia.pco=2450");
            SendDataDisplay();  
            printf("tmes.pco=0");
            SendDataDisplay(); 
            printf("tyear.pco=0");
            SendDataDisplay(); 
            printf("thora.pco=0");
            SendDataDisplay(); 
            printf("tminutos.pco=0");
            SendDataDisplay(); 
         }else if(Opcion==2){
            printf("tdia.pco=0");
            SendDataDisplay();  
            printf("tmes.pco=2450");
            SendDataDisplay(); 
            printf("tyear.pco=0");
            SendDataDisplay(); 
            printf("thora.pco=0");
            SendDataDisplay(); 
            printf("tminutos.pco=0");
            SendDataDisplay(); 
         }else if(Opcion==3){
            printf("tdia.pco=0");
            SendDataDisplay();  
            printf("tmes.pco=0");
            SendDataDisplay(); 
            printf("tyear.pco=2450");
            SendDataDisplay(); 
            printf("thora.pco=0");
            SendDataDisplay(); 
            printf("tminutos.pco=0");
            SendDataDisplay();
         }else if(Opcion==4){
            printf("tdia.pco=0");
            SendDataDisplay();  
            printf("tmes.pco=0");
            SendDataDisplay(); 
            printf("tyear.pco=0");
            SendDataDisplay(); 
            printf("thora.pco=2450");
            SendDataDisplay(); 
            printf("tminutos.pco=0");
            SendDataDisplay();
         }else if(Opcion==5){
            printf("tdia.pco=0");
            SendDataDisplay();  
            printf("tmes.pco=0");
            SendDataDisplay(); 
            printf("tyear.pco=0");
            SendDataDisplay(); 
            printf("thora.pco=0");
            SendDataDisplay(); 
            printf("tminutos.pco=2450");
            SendDataDisplay();
         }
            
         if(RX_Buffer[4]==0x11 || LEFT){//Selecciono Regresar
            LEFT=OFF;
            printf("page Ajustes");
            SendDataDisplay();  
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            PantallaPrincipal=0;
            
            if(esBisiesto(YearTx))
               Modulo=Bisiesto[MesTx];
            else
               Modulo=Regular[MesTx];
         
            dowTx=((YearTx-1)%7+((YearTx-1)/4-3*((YearTx-1)/100+1)/4)%7+Modulo+DiaTx%7)%7;
            rtc_set_datetime(DiaTx,MesTx,YearTx,dowTx,HoraTx,MinutoTx);
         }
         
         if((RX_Buffer[4]==0x2a)|| UP){//Tecla Arriba Oprimida
            delay_ms(200);
            if((RX_Buffer[4]==0x2a)|| UP){   
               UP=OFF;
               if(Opcion==1){
                  if(MesTx==2){
                     if(esBisiesto(YearTx)){
                        if(DiaTx<29)
                           DiaTx++;
                        else
                           DiaTx=1;   
                     }else{
                        if(DiaTx<28)
                           DiaTx++;
                        else
                           DiaTx=1;   
                     }
                  }else{
                     if(MesTx<=7){
                        if(MesTx % 2 ==0){
                           if(DiaTx<30)
                              DiaTx++;    
                           else
                              DiaTx=1;   
                        }else{
                           if(DiaTx<31)
                              DiaTx++;    
                           else
                              DiaTx=1;   
                        }    
                     }else{
                        if(MesTx % 2 ==0){
                           if(DiaTx<31)
                              DiaTx++;  
                           else
                              DiaTx=1;
                        }else{
                           if(DiaTx<30)
                              DiaTx++;    
                           else
                              DiaTx=1;
                        }    
                     }
                  }
               }else if(Opcion==2){
                  if(MesTx<12)
                     MesTx++;
                  else
                     MesTx=1;
               }else if(Opcion==3){
                  if(YearTx<99)
                     YearTx++;
                  else 
                     YearTx=0;
               }else if(Opcion==4){
                  if(HoraTx<24)
                     HoraTx++;
                  else
                     HoraTx=0;
               }else if(Opcion==5){
                  if(MinutoTx<59)
                     MinutoTx++;
                  else
                     MinutoTx=0;
               }
               RX_Buffer[4]=0x00;
               RX_Buffer2[4]=0x00;
            }
         }
         
         if(RX_Buffer[4]==0x2b || DOWN){//Tecla Abajo Oprimida
            delay_ms(200);
            if(RX_Buffer[4]==0x2b || DOWN){   
               DOWN=OFF;
               if(Opcion==1){
                  if(DiaTx>0)
                     DiaTx--;
               }else if(Opcion==2){
                  if(MesTx>1)
                     MesTx--;
                  else
                     MesTx=12;
               }else if(Opcion==3){
                  if(YearTx>0)
                     YearTx--;
                  else
                     YearTx=99;
               }else if(Opcion==4){
                  if(HoraTx>0)
                     HoraTx--;
                  else
                     HoraTx=23;
               }else if(Opcion==5){
                  if(MinutoTx>0)
                     MinutoTx--;
                  else
                     MinutoTx=59;
               }
               RX_Buffer[4]=0x00;
               RX_Buffer2[4]=0x00;
            }
         }
      
      printf("thora.txt=\"%02u\"",HoraTx);
      SendDataDisplay();
      printf("tminutos.txt=\"%02u\"",MinutoTx);
      SendDataDisplay();
      printf("tdia.txt=\"%02u\"",DiaTx);
      SendDataDisplay();
      printf("tmes.txt=\"%02u\"",MesTx);
      SendDataDisplay();
      printf("tyear.txt=\"%02u\"",YearTx);
      SendDataDisplay();
      LimitaDia();  
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
    if(Menu==8){//Menu de off
      VelMotor=0;VelMotor2=0;LuzBlanca_off;LuzUV_off;Toma_off;Alarma_off;Motor_off;Motor2_off;Aux_off;
      mPurga=0;sPurga=0;mPPurga=0;sPPurga=0;mUV=0;sUV=0;flagClave=0;tClave=0;
      tiempo_purga=0;tiempo_postpurga=0;TipoClave=0;tEncendido=0;
      
      if(!Encendio)
         Display_off;
      if(RIGHT || UP || DOWN){                  
            RIGHT=OFF;UP=OFF;DOWN=OFF;
            Display_on;
            Encendio=ON;
            Prendio=20;
            delay_ms(1000);
            printf("page Bienvenida");
            SendDataDisplay();   
      }
      
      if(RX_Buffer[4]==0x11){
         printf("page Bienvenida");
         SendDataDisplay();                  
      }
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==9){ //Menu de Tiempo de UV
         
         printf("fecha.txt=\"Fecha:%02u/%02u/20%02u    Hora: %02u:%02u:%02u  T:%02u?C\"",Dia,Mes,Year,Hora,Minuto,Segundo,Temperatura);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a){//Selecciono Minutos
            Minutos=ON;
            Segundos=OFF;
         }
         
         if(RX_Buffer[4]==0x0b){//Selecciono Segundos
            Segundos=ON;
            Minutos=OFF;
         }    
         
         if(RIGHT){
            delay_ms(200);
            if(RIGHT){
               RIGHT=OFF;
               if(!Segundos){
                  Segundos=ON;
                  Minutos=OFF;
                  printf("tsecest.pco=65535");
                  SendDataDisplay();  
                  printf("tminest.pco=0");
                  SendDataDisplay();  
               }else if(!Minutos){
                  Segundos=OFF;
                  Minutos=ON;
                  printf("tsecest.pco=0");
                  SendDataDisplay();  
                  printf("tminest.pco=65535");
                  SendDataDisplay();  
               }
            }
         }
         
         if(RX_Buffer[4]==0x11 || LEFT){//Selecciono Regresar
            delay_ms(200);
            if(RX_Buffer[4]==0x11 || LEFT){
               LEFT=OFF;
               Segundos=OFF;
               Minutos=OFF;
               printf("page Ajustes");
               SendDataDisplay();
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
               if(!GuardaEEPROM){
                  write_eeprom(8,mUVp);
                  delay_ms(10);
                  write_eeprom(9,sUVp);
                  delay_ms(10);
                  mUV=mUVp;
                  sUV=sUVp;
                  GuardaEEPROM=ON;
               }
            }
         }
         
         if(RX_Buffer[4]==0x0c || RX_Buffer[4]==0x0d || UP){//Tecla arriba Oprimida
            delay_ms(10);
            if(RX_Buffer[4]==0x0c || RX_Buffer[4]==0x0d || UP){//Tecla arriba Oprimida
               UP=OFF;
               if(Segundos){
                  sUVp++;
               }
               if(Minutos){
                  mUVp++;
               }  
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;
            }
         }
         
         if(RX_Buffer[4]==0x0f || RX_Buffer[4]==0x0e || DOWN){//Tecla abajo oprimida
            delay_ms(10);
            if(RX_Buffer[4]==0x0f || RX_Buffer[4]==0x0e || DOWN){//Tecla abajo oprimida
               DOWN=OFF;
               if(Segundos && sUVp>0){
                  sUVp--;
               }
               if(Minutos){
                  mUVp--;
               }
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;
            }
         }
         
         if(mUVp<1)mUVp=99;
         if(mUVp>99)mUVp=1;
         if(sUVp>59)sUVp=0;
            
         printf("tminest.txt=\"%02u\"",mUVp);
         SendDataDisplay();
         printf("tsecest.txt=\"%02u\"",sUVp);
         SendDataDisplay();
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
    if(Menu==10){ // Menu de Mantenimiento
         
         LuzBlanca_on;
         UVTime=Tuv[0]+(Tuv[1]*10)+(Tuv[2]*100)+(Tuv[3]*1000);   
         TrabajoTime=Ttrabajo[0]+(Ttrabajo[1]*10)+(Ttrabajo[2]*100)+(Ttrabajo[3]*1000);
         /*
         printf("b00.txt=\"UV(horas):%05Lu\"",(int16)UVTime);
         SendDataDisplay();
         printf("b11.txt=\"Filtro(horas):%05Lu\"",(int16)TrabajoTime);
         SendDataDisplay();
         
         printf("ceroIn.txt=\"Cero InFlow: %2.0f \"",Flujo[1].zero_actual);
         SendDataDisplay();
         printf("ceroDn.txt=\"Cero DnFlow: %2.0f \"",Flujo[2].zero_actual);
         SendDataDisplay();
         printf("ajusteIn.txt=\"Ajuste InFlow:%2.2f Qi:%3.0f\"",Flujo[1].ajuste,Qexhaust);
         SendDataDisplay();
         printf("ajusteDn.txt=\"Ajuste DnFlow:%2.2f Qd:%3.0f\"",Flujo[2].ajuste,Qdownflow);
         SendDataDisplay();
         printf("inflow.txt=\"InFlow: %2.2f Pi:%2.2f \"",Inflow,Filtro_Inflow);
         SendDataDisplay();
         printf("downflow.txt=\"DownFlow: %2.2f Pd:%2.2f\"",DownFlow,Filtro_Downflow);
         SendDataDisplay();
         printf("adcin.txt=\"ADCIn: %04Lu \"",(int16)Flujo[1].adc);
         SendDataDisplay();
         printf("adcdn.txt=\"ADCDn: %04Lu d:%2.2f\"",sensores(2),densidad);
         SendDataDisplay();
         */
         Tuv[0]++;
         readHolding();
         writeCommand(5,2,Tuv[0]);
         printf("b00.txt=\"R0:%05Lu  R1:%05Lu\"",make16(txbuf[1],txbuf[2]),make16(txbuf[3],txbuf[4]));
         SendDataDisplay();
         printf("b11.txt=\"R2:%05Lu  R3:%05Lu\"",make16(txbuf[5],txbuf[6]),make16(txbuf[7],txbuf[8]));
         SendDataDisplay();
         printf("ceroIn.txt=\"R4:%05Lu  R5:%05Lu\"",make16(txbuf[9],txbuf[10]),make16(txbuf[11],txbuf[12]));
         SendDataDisplay();
         printf("ceroDn.txt=\"R6:%05Lu  R7:%05Lu\"",make16(txbuf[13],txbuf[14]),make16(txbuf[15],txbuf[16]));
         SendDataDisplay();
         printf("ajusteIn.txt=\"R7:%05Lu  R8:%05Lu\"",make16(txbuf[17],txbuf[18]),make16(txbuf[19],txbuf[20]));
         SendDataDisplay();
         printf("ajusteDn.txt=\"R9:%05Lu  R10:%05Lu\"",make16(txbuf[21],txbuf[22]),make16(txbuf[23],txbuf[24]));
         SendDataDisplay();
         printf("inflow.txt=\"tx[0]:%03u tx[49]:%03u\"",txbuf[0],txbuf[49]);
         SendDataDisplay();
         printf("downflow.txt=\"DownFlow: %2.2f Pd:%2.2f\"",DownFlow,Filtro_Downflow);
         SendDataDisplay();
         printf("adcin.txt=\"ADCIn: %04Lu \"",(int16)Flujo[1].adc);
         SendDataDisplay();
         printf("adcdn.txt=\"ADCDn: %04Lu d:%2.2f\"",sensores(2),densidad);
         SendDataDisplay();
         
         if(RX_Buffer[4]==0x01 || (UP && RIGHT)){ // Reinicia Tiempo de Luz UV         
            delay_ms(2000); 
            if(RX_Buffer[4]==0x01 || (UP && RIGHT)){ // Reinicia Tiempo de Luz UV
               UP=OFF;RIGHT=OFF;
               write_eeprom(12,0);
               delay_ms(20);
               write_eeprom(13,0);
               delay_ms(20);
               write_eeprom(14,0);
               delay_ms(20);
               write_eeprom(15,0);
               delay_ms(20);
               Tuv[3]=0;Tuv[2]=0;Tuv[1]=0;Tuv[0]=0;
               LuzBlanca_off;
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;
               printf("page Ajustes");
               SendDataDisplay();
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }
         }
      
         if(RX_Buffer[4]==0x02 || (DOWN && RIGHT)){ // Reinicia Tiempo de Trabajo        
            delay_ms(2000);
            if(RX_Buffer[4]==0x02 || (DOWN && RIGHT)){ // Reinicia Tiempo de Trabajo        
               DOWN=OFF;RIGHT=OFF;
               write_eeprom(16,0);
               delay_ms(20);
               write_eeprom(17,0);
               delay_ms(20);
               write_eeprom(18,0);
               delay_ms(20);
               write_eeprom(19,0);
               delay_ms(20);
               Ttrabajo[3]=0;Ttrabajo[2]=0;Ttrabajo[1]=0;Ttrabajo[0]=0;
               LuzBlanca_off;
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;
               printf("page Ajustes");
               SendDataDisplay();
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            }
         }
         
         if(RX_Buffer[4]==0x21 || LEFT){//Oprimio salir
            delay_ms(50);
            if(RX_Buffer[4]==0x21 || LEFT){
               LEFT=OFF;
               printf("page Ajustes");
               SendDataDisplay();
               LuzBlanca_off;
               UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
               // Falta apagar Todo
            }
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------    

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
    if(Menu==15){//Menu de Clave Correcta
      if(!entro){
         delay_ms(2000);
         entro=ON;
      }
      printf("page Purga");
      SendDataDisplay();
      UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------      

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
    if(Menu==16){//Menu de Clave InCorrecta
      if(!entro){
         delay_ms(2000);
         entro=ON;
      }
      printf("page Clave");
      SendDataDisplay();
      UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------      

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==100){ //Menu de Configuraci?n de Parametros de Control
      if(Opcion2>15)
         Opcion2=1;
      if(Opcion2<1)
         Opcion2=15;
      
      if(Opcion2==1){
         printf("Config.t2c.txt=\"Velocidad\"");
         SendDataDisplay();      
         printf("t3c.txt=\"%u\"",Vel1);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a || UP){//Tecla Arriba Oprimida
               UP=OFF;
               Vel1+=1.0;
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
            
         if(RX_Buffer[4]==0x0b || DOWN){//Tecla Abajo Oprimida
               DOWN=OFF;
               Vel1-=1.0;
               RX_Buffer[4]=0x00;
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
         VelocidadMotor(Vel1);
         if(Vel1>100)
            Vel1=1;
         if(Vel1<1)
            Vel1=100;
      }else if(Opcion2==2){
         VelMotor=0;
         VelMotor2=0;
         printf("Config.t2c.txt=\"Cero InFlow\"");
         SendDataDisplay();      
         printf("t3c.txt=\"%2.0f\"",Flujo[1].zero_actual);
         SendDataDisplay();
         
         if(RX_Buffer[4]==0x0a || UP){//Tecla Arriba Oprimida
            UP=OFF;
            Flujo[1].zero_actual=sensores(1);
            Flujo[1].adc=sensores(1);
            Flujo[1].Diferencia=zero_fabrica-Flujo[1].adc;
               
            if(Flujo[1].Diferencia>=0){
               Flujo[1].negativo=10;write_eeprom(50,Flujo[1].negativo);delay_ms(20);
            }
            if(Flujo[1].Diferencia<0){
               Flujo[1].negativo=20;write_eeprom(50,Flujo[1].negativo);delay_ms(20);
            }
            Flujo[1].Diferencia=abs(Flujo[1].Diferencia);
            Temporal1=(int16)Flujo[1].Diferencia;
            write_eeprom(42,make8(Temporal1,0));
            delay_ms(20);
            write_eeprom(43,make8(Temporal1,1));//Guardar valor de Setpoint en eeprom
            delay_ms(20);
   
            Temporal1=(int16)Flujo[1].zero_actual;
            write_eeprom(33,make8(Temporal1,0));
            delay_ms(20);
            write_eeprom(34,make8(Temporal1,1));//Guardar valor de Setpoint en eeprom
            delay_ms(20);
            RX_Buffer[4]=0x00;  
            RX_Buffer2[4]=0x00;                           
         }
            
         if(RX_Buffer[4]==0x0b || DOWN){//Tecla Abajo Oprimida            
               DOWN=OFF;
               RX_Buffer[4]=0x00;
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
      }else if(Opcion2==3){
         VelMotor=0;
         VelMotor2=0;
         printf("Config.t2c.txt=\"Cero DnFlow\"");
         SendDataDisplay();      
         printf("t3c.txt=\"%2.0f\"",Flujo[2].zero_actual);
         SendDataDisplay();
         
         if(RX_Buffer[4]==0x0a || UP){//Tecla Arriba Oprimida
            UP=OFF;
            Flujo[2].zero_actual=sensores(2);
            Flujo[2].adc=sensores(2);
            Flujo[2].Diferencia=zero_fabrica-Flujo[2].adc;
               
            if(Flujo[2].Diferencia>=0){
               Flujo[2].negativo=10;write_eeprom(51,Flujo[2].negativo);delay_ms(20);
            }
            if(Flujo[2].Diferencia<0){
               Flujo[2].negativo=20;write_eeprom(51,Flujo[2].negativo);delay_ms(20);
            }
            Flujo[2].Diferencia=abs(Flujo[2].Diferencia);
            Temporal1=(int16)Flujo[2].Diferencia;
            write_eeprom(44,make8(Temporal1,0));
            delay_ms(20);
            write_eeprom(45,make8(Temporal1,1));//Guardar valor de Setpoint en eeprom
            delay_ms(20);
   
            Temporal1=(int16)Flujo[2].zero_actual;
            write_eeprom(35,make8(Temporal1,0));
            delay_ms(20);
            write_eeprom(36,make8(Temporal1,1));//Guardar valor de Setpoint en eeprom
            delay_ms(20);
            RX_Buffer[4]=0x00;  
            RX_Buffer2[4]=0x00;                           
         }
            
         if(RX_Buffer[4]==0x0b || DOWN){//Tecla Abajo Oprimida            
               DOWN=OFF;
               RX_Buffer[4]=0x00;
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
      }else if(Opcion2==4){
         printf("Config.t2c.txt=\"Ajuste InFlow\"");
         SendDataDisplay();      
         printf("t3c.txt=\"%2.2f\"",Flujo[1].Ajuste);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a || UP){//Tecla Arriba Oprimida            
               UP=OFF;
               if(Flujo[1].Ajuste<10)
                  Flujo[1].Ajuste+=0.01;
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;                           
         }
            
         if(RX_Buffer[4]==0x0b || DOWN){//Tecla Abajo Oprimida            
               DOWN=OFF;
               if(Flujo[1].Ajuste>0)
                  Flujo[1].Ajuste-=0.01;
               RX_Buffer[4]=0x00;
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
      }else if(Opcion2==5){
         printf("Config.t2c.txt=\"Ajuste DnFlow\"");
         SendDataDisplay();      
         printf("t3c.txt=\"%2.2f\"",Flujo[2].Ajuste);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a || UP){//Tecla Arriba Oprimida
               UP=OFF;
               if(Flujo[2].Ajuste<10)
                  Flujo[2].Ajuste+=0.01;
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
            
         if(RX_Buffer[4]==0x0b || DOWN){//Tecla Abajo Oprimida
               DOWN=OFF;
               if(Flujo[2].Ajuste>0)
                  Flujo[2].Ajuste-=0.01;
               RX_Buffer[4]=0x00;
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();            
         }
      }else if(Opcion2==6){
         VelMotor=0;
         VelMotor2=0;
         printf("Config.t2c.txt=\"Cero PDownFlow\"");
         SendDataDisplay();      
         printf("t3c.txt=\"%2.0f\"",Presion[4].G);
         //printf("t3c.txt=\"%2.0f\"",Gdn);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a || UP){//Tecla Arriba Oprimida            
            UP=OFF;
            Presion[4].G=sensores(4);//Presion[4].adc;
            G16=(int16)Presion[4].G;
            //Gdn=sensores(4);//Presion[4].adc;
            //G16=(int16)Gdn;
            G_l=G16; G_h=(G16>>8);
               
            write_eeprom(46,G_l);
            delay_ms(20);
            write_eeprom(47,G_h);//Guardar valor de Setpoint en eeprom
            delay_ms(20);
            RX_Buffer[4]=0x00;  
            RX_Buffer2[4]=0x00;
            Alarma_on;delay_ms(500);Alarma_off;
         }
            
         if(RX_Buffer[4]==0x0b){//Tecla Abajo Oprimida            
               
         }
      }else if(Opcion2==7){
         VelMotor=0;
         VelMotor2=0;
         printf("Config.t2c.txt=\"Cero PInFlow\"");
         SendDataDisplay();      
         printf("t3c.txt=\"%2.0f\"",Presion[3].G);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a || UP){//Tecla Arriba Oprimida            
            UP=OFF;
            Presion[3].G=sensores(3);//Presion[3].adc;
            G16=(int16)Presion[3].G;
            G_l=G16; G_h=(G16>>8);
               
            write_eeprom(48,G_l);
            delay_ms(20);
            write_eeprom(49,G_h);//Guardar valor de Setpoint en eeprom
            delay_ms(20);
            RX_Buffer[4]=0x00;  
            RX_Buffer2[4]=0x00;
            Alarma_on;delay_ms(500);Alarma_off;
         }
         
         if(RX_Buffer[4]==0x0b){//Tecla Abajo Oprimida            
               
         }
      }else if(Opcion2==8){
         printf("Config.t2c.txt=\"Limite PDownFlow\"");
         SendDataDisplay();      
         printf("t3c.txt=\"%2.1f\"",Presion[4].Ajuste);
         //printf("t3c.txt=\"%2.1f\"",Ajustedn);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a || UP){//Tecla Arriba Oprimida            
               UP=OFF;
               if(Presion[4].Ajuste<2.0)
                  Presion[4].Ajuste+=0.1;
               //if(Ajustedn<5.0)
               //   Ajustedn+=0.1;
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;            
         }
            
         if(RX_Buffer[4]==0x0b || DOWN){//Tecla Abajo Oprimida
               DOWN=OFF;
               if(Presion[4].Ajuste>0.1)
                  Presion[4].Ajuste-=0.1;
               //if(Ajustedn>0.0)
               //   Ajustedn-=0.1;
               RX_Buffer[4]=0x00;
               RX_Buffer2[4]=0x00;            
         }
         if(Presion[4].Ajuste>2.0)
            Presion[4].Ajuste=0.1;
         if(Presion[4].Ajuste<0.1)
            Presion[4].Ajuste=2.0;   
      }else if(Opcion2==9){
         printf("Config.t2c.txt=\"Limite PInFlow\"");
         SendDataDisplay();      
         printf("t3c.txt=\"%2.1f\"",Presion[3].Ajuste);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a || UP){//Tecla Arriba Oprimida            
               UP=OFF;
               if(Presion[3].Ajuste<3.0)
                  Presion[3].Ajuste+=0.1;
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;            
         }
            
         if(RX_Buffer[4]==0x0b || DOWN){//Tecla Abajo Oprimida
               DOWN=OFF;
               if(Presion[3].Ajuste>0.3)
                  Presion[3].Ajuste-=0.1;
               RX_Buffer[4]=0x00;
               RX_Buffer2[4]=0x00;            
         }
         if(Presion[3].Ajuste>3.0)
            Presion[3].Ajuste=0.1;
         if(Presion[3].Ajuste<0.1)
            Presion[3].Ajuste=3.0;
      }else if(Opcion2==10){
         printf("Config.t2c.txt=\"Velocidad 2\"");
         SendDataDisplay();      
         printf("t3c.txt=\"%u\"",Vel2);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a || UP){//Tecla Arriba Oprimida
               UP=OFF;
               Vel2+=1.0;
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
            
         if(RX_Buffer[4]==0x0b || DOWN){//Tecla Abajo Oprimida
               DOWN=OFF;
               Vel2-=1.0;
               RX_Buffer[4]=0x00;
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
         VelMotor2=Vel2;
         if(Vel2>6)
            Vel2=1;
         if(Vel2<1)
            Vel2=6;
      }else if(Opcion2==11){
         printf("Config.t2c.txt=\"Alarma InFlow\"");
         SendDataDisplay();      
         printf("t3c.txt=\"%2.2f\"",Relacion);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a || UP){//Tecla Arriba Oprimida
               UP=OFF;
               Relacion+=0.01;
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
            
         if(RX_Buffer[4]==0x0b || DOWN){//Tecla Abajo Oprimida
               DOWN=OFF;
               Relacion-=0.01;
               RX_Buffer[4]=0x00;
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
         if(Relacion>4.0)Relacion=0.4;
         if(Relacion<0.4)Relacion=4.0;
      }else if(Opcion2==12){
         printf("Config.t2c.txt=\"P Inicial DownFlow\"");
         SendDataDisplay();      
         printf("t3c.txt=\"%2.2f\"",pInDn);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a || UP){//Tecla Arriba Oprimida
               UP=OFF;
               pInDn+=0.01;
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
            
         if(RX_Buffer[4]==0x0b || DOWN){//Tecla Abajo Oprimida
               DOWN=OFF;
               pInDn-=0.01;
               RX_Buffer[4]=0x00;
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
         if(pInDn>0.6)pInDn=0.1;
         if(pInDn<0.1)pInDn=0.6;
      }else if(Opcion2==13){
         printf("Config.t2c.txt=\"P Inicial InFlow\"");
         SendDataDisplay();      
         printf("t3c.txt=\"%2.2f\"",pInIn);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a || UP){//Tecla Arriba Oprimida
               UP=OFF;
               pInIn+=0.01;
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
            
         if(RX_Buffer[4]==0x0b || DOWN){//Tecla Abajo Oprimida
               DOWN=OFF;
               pInIn-=0.01;
               RX_Buffer[4]=0x00;
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
         if(pInIn>1.8)pInIn=0.8;
         if(pInIn<0.8)pInIn=1.8;
      }else if(Opcion2==14){
         printf("Config.t2c.txt=\"P atmosferica\"");
         SendDataDisplay();      
         printf("t3c.txt=\"%2.0f\"",Pr);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a || UP){//Tecla Arriba Oprimida
               UP=OFF;
               Pr+=1;
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
            
         if(RX_Buffer[4]==0x0b || DOWN){//Tecla Abajo Oprimida
               DOWN=OFF;
               Pr-=1;
               RX_Buffer[4]=0x00;
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
      }else if(Opcion2==15){
         printf("Config.t2c.txt=\"Humedad\"");
         SendDataDisplay();      
         printf("t3c.txt=\"%2.0f\"",HR);
         SendDataDisplay();
         if(RX_Buffer[4]==0x0a || UP){//Tecla Arriba Oprimida
               UP=OFF;
               HR+=1.0;
               RX_Buffer[4]=0x00;  
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
            
         if(RX_Buffer[4]==0x0b || DOWN){//Tecla Abajo Oprimida
               DOWN=OFF;
               HR-=1.0;
               RX_Buffer[4]=0x00;
               RX_Buffer2[4]=0x00;            
               //BorraBuffer();
         }
      }
      
      if(RX_Buffer[4]==0x0c){//Tecla Izquierda Oprimida
            Opcion2++;
            RX_Buffer[4]=0x00;  
            RX_Buffer2[4]=0x00;            
            //BorraBuffer();   
      }
         
      if(RX_Buffer[4]==0x0d || RIGHT){//Tecla Derecha Oprimida         
            Opcion2--;
            RX_Buffer[4]=0x00;
            RX_Buffer2[4]=0x00;            
            RIGHT=OFF;
            //BorraBuffer();
      }
      
      if(RX_Buffer[4]==0x11 || LEFT){//Tecla Regresar
         delay_ms(20);
         if(RX_Buffer[4]==0x11 || LEFT){
            LEFT=OFF;
            if(!GuardaEEPROM){
               Temporal=Flujo[1].Ajuste;
               Entero=(int)Flujo[1].Ajuste;
               Temporal=Temporal-Entero;
               Temporal2=Temporal*100.0;
               Decimal1=(int8)Temporal2;
               write_eeprom(25,Entero);
               delay_ms(20);
               write_eeprom(26,Decimal1);
               delay_ms(20);
               
               Temporal=Flujo[2].Ajuste;
               Entero=(int)Flujo[2].Ajuste;
               Temporal=Temporal-Entero;
               Temporal2=Temporal*100.0;
               Decimal1=(int8)Temporal2;
               write_eeprom(27,Entero);
               delay_ms(20);
               write_eeprom(28,Decimal1);
               delay_ms(20);
               
               Temporal=Presion[4].Ajuste;
               Entero=(int)Presion[4].Ajuste;
               //Temporal=Ajustedn;
               //Entero=(int)Ajustedn;
               Temporal=Temporal-Entero;
               Temporal2=Temporal*10.0;
               Decimal1=(int8)Temporal2;
               write_eeprom(29,Entero);
               delay_ms(20);
               write_eeprom(30,Decimal1);
               delay_ms(20);
               
               Temporal=Presion[3].Ajuste;
               Entero=(int)Presion[3].Ajuste;
               Temporal=Temporal-Entero;
               Temporal2=Temporal*10.0;
               Decimal1=(int8)Temporal2;
               write_eeprom(31,Entero);
               delay_ms(20);
               write_eeprom(32,Decimal1);
               delay_ms(20);
               write_eeprom(40,Vel1);
               delay_ms(20);
               write_eeprom(41,Vel2);
               delay_ms(20);
               
               Temporal=Relacion;
               Entero=(int)Relacion;
               Temporal=Temporal-Entero;
               Temporal2=Temporal*100.0;
               Decimal1=(int8)Temporal2;
               write_eeprom(52,Entero);
               delay_ms(20);
               write_eeprom(53,Decimal1);
               delay_ms(20);
               
               Temporal=pInDn;
               Entero=(int)pInDn;
               Temporal=Temporal-Entero;
               Temporal2=Temporal*100.0;
               Decimal1=(int8)Temporal2;
               write_eeprom(54,Entero);
               delay_ms(20);
               write_eeprom(55,Decimal1);
               delay_ms(20);
               
               Temporal=pInIn;
               Entero=(int)pInIn;
               Temporal=Temporal-Entero;
               Temporal2=Temporal*100.0;
               Decimal1=(int8)Temporal2;
               write_eeprom(56,Entero);
               delay_ms(20);
               write_eeprom(57,Decimal1);
               delay_ms(20);
               GuardaEEPROM=ON;
               
               write_eeprom(58,(int8)Pr);
               delay_ms(20);
               write_eeprom(59,(int8)HR);
               delay_ms(20);
            }
            printf("page Ajustes");
            SendDataDisplay();
            
            calculaDensidad();
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         }
      }
      printf("fecha.txt=\"Fecha:%02u/%02u/20%02u    Hora: %02u:%02u\"",Dia,Mes,Year,Hora,Minuto);
      SendDataDisplay(); 
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

   }

}
