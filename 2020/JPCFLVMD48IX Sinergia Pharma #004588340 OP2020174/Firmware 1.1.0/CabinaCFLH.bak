// Cuatro pulsadores de entrada (Derecha, Izquierda, arriba y abajo).
// Reloj con XT de 4 MHz.
// Programación para Cabina de PCR.
// Tiene Contraseña de incio para permitir funcionamiento de Cabina.
// Tiene Menú:Luz UV, Luz Blanca, Motor Ventilador y Cambio de Contraseña.
// Ing. Juan David Piñeros.
// JP Inglobal.

#include <18F4550.h>
#device adc=10
#device HIGH_INTS=TRUE //Activamos niveles de prioridad
#priority TIMER2
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL5,CPUDIV4,VREGEN,NOMCLR// PLL1 para 4 MHz
#use delay(clock=16000000)
#include <MATH.h>
#include <LCD420.c>

// Definición de teclado - NC
#define   UP            !input(PIN_A5)
#define   DOWN          !input(PIN_E0)
#define   RIGHT         !input(PIN_E1)
#define   IZQU          !input(PIN_E2)
#define   VIDRIOUUP     !input(PIN_A1)
#define   VIDRIODN      !input(PIN_A2)

// Definición de otros puertos
#define   Luz_UV_on         output_bit(PIN_B3,0)
#define   Luz_UV_off        output_bit(PIN_B3,1)
#define   Luz_Blanca_on     output_bit(PIN_B4,0)
#define   Luz_Blanca_off    output_bit(PIN_B4,1)
#define   Motor_on          output_bit(PIN_C2,1)
#define   Motor_off         output_bit(PIN_C2,0)
#define   Motor2_on         output_bit(PIN_C1,1)
#define   Motor2_off        output_bit(PIN_C1,0)
#define   Buzzer_on         output_bit(PIN_C0,1)
#define   Buzzer_off        output_bit(PIN_C0,0)
#define   O1_on             output_bit(PIN_B6,1)
#define   O1_off            output_bit(PIN_B6,0)
#define   O2_on             output_bit(PIN_B7,1)
#define   O2_off            output_bit(PIN_B7,0)
#define   ON  1
#define   OFF 0

short estadouv=0,estadofl=0,estadomv=0,Flanco=0,Flanco1=0,Flanco2=0,Flanco3=0,tiempo_cumplido=0,Lectura=0,flaguv=0,flagfl=0,estadobuzzer=0,estadoalarma=0,pulsoSubir=OFF,pulsoBajar=OFF,flag_arriba=OFF,Entro=0;
short flagdn=0,flagup=0;
int8 Menu=0, n_opcionH=7,n_opcionL=2,unidad=11,Flecha=2,Flecha1=3,Flecha2=2, nMenuL=2, nMenuH=4,segundos=0,minutos=0,horasL=0,horasH=0,G_l=0,G_h=0;
int8 tempo_minutos=0,tempo_segundos=0,tempo_minutos_pro=0,tempo_segundos_pro=0,r=0,q=0,i=0,unidad2=1,tiemporeset=0,guardaruv=0;
signed int8   paso=0;
char t[3]={'>',' ','^'}; 
signed  int8 clave[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
signed  int8 contrasena[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
int8 ActivaBuzzer=0,codigoSubir=0,codigoBajar=0,codigoDetener=0,n=0,Vel1=4;
int16 tiempos,horas=0,G16=0;
float PromPresion[20]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float V1=0.0,Presion=0.0,Saturacion=0.0,CaidaPresion=0.0;
float G=2;
short Cambio=ON,flagmv=0;

int16 pulsos=0,retardo=0;
short Cruce=OFF,Purga=OFF;
int16 tiempocruce=0;
int8 Porcentaje=0,VelMotor=0;
float Tcruce=0.0,Tcruce2=0;

int1 BITS[32];
int16 Ta,Tb,TICK=0;
Int1 BIT_START, NUEVO_DATO;
int BYTE_IR[4];
int8 BIT_BYTE(int1 D7, int1 D6, int1 D5, int1 D4, int1 D3, int1 D2, int1 D1, int1 D0);

#include <Funciones.h>
/*
#int_EXT1 HIGH
void RB1(){
   Ta=get_timer1();
   TICK = Ta-tb;
   Tb = Ta;
   
   Entro=1;
   
   if(BIT_START==1){
       //BIT 1
       if(TICK>960 && TICK < 1280)  // 700 con Pres 4 y 20MHz 
      {
         BITS[n]=1; n++;
      }
      //BIT 0
       if(TICK>400 && TICK < 720)  // 350 con Pres 4 y 20MHz 
      {
         BITS[n]=0; n++;
      }
      if(n>=32){
         NUEVO_DATO=1;
         BIT_START=0;
      }
   
   }
   //BIT START
   if(TICK>2800 && TICK < 5800 && BIT_START==0)  //3500 con Pres 4 y 20MHz 
   {
      BIT_START=1;
      n=0; 
   }
}
*/
#int_EXT HIGH
void ext0() {  
   Cruce=ON;
   
   if(VelMotor==5){
      set_timer0(228); // Cada cuenta del timer0 es 51.2us y timer1 es 1.59us
      Motor_off; 
      Motor2_off; 
   }else if(VelMotor==4){
      set_timer0(201);      
      Motor_off; 
      Motor2_off; 
   }else if(VelMotor==3){
      set_timer0(196);
      Motor_off; 
      Motor2_off; 
   }else if(VelMotor==2){
      set_timer0(188);
      Motor_off; 
      Motor2_off; 
   }else if(VelMotor==1){
      set_timer0(180);
      Motor_off; 
      Motor2_off; 
   }
   pulsos++;   
}

#int_TIMER0 HIGH
void temp0s(void){
   if(VelMotor>0 && VelMotor<6){
   if(Cruce){         //If the triac pin is low we change the state and prepare the timer
      Motor_on;
      Motor2_on; 
      set_timer0(253);         // Next timer overflow will clear the triac pin
      Cruce=OFF;         // Flag now signals that we need to turn off the triac output pin
   } else {            //The triac has been triggered, cut the pulse
      Motor_off;
      Motor2_off; 
      set_timer0(0);         //Set timer period to maximum, timer should not overflow untill next zero cross interrupt
   }
   }
}

//#int_TIMER2
#int_TIMER1
void temp1s(void){
   //set_timer2(231);//100.25uS
   set_timer1(45536);//5ms
   tiempos++;
   //
    
   //if(tiempos>=10000){
   //if(tiempos>=9710){//1s
   if(tiempos>=200){//1s
   output_toggle(PIN_A3);
      tiempos=0;tiemporeset++;Lectura=1;
   
      if(ActivaBuzzer==1){
         if(estadobuzzer==0){
            Buzzer_off;
         }else{
            if(estadoalarma==1)
               Buzzer_on;
         }
         estadobuzzer=!estadobuzzer;
      }
   //----------------------------- Tiempo Total-----------------------------//
      if(estadouv==1){
         segundos++;
         if(tiempo_cumplido==0){
            if(tempo_segundos>0){
               tempo_segundos--;
            }else{
               if(tempo_minutos>0){
                  tempo_minutos--;
                  tempo_segundos=59;
               }else{
                  tiempo_cumplido=1;
                  estadouv=0;Luz_UV_off;
                  tempo_minutos=tempo_minutos_pro;tempo_segundos=tempo_segundos_pro;
               }
            }
         }
      }
   }
}

void main ()
{
   Motor_off;
   Motor2_off;
   ConfigInterrupciones();
   MensajeBienvenida();
   LeeEEPROM(); 
   LimitaValores();
   Motor_off;
   Motor2_off;
/*   
   if(UP && DOWN){
      delay_ms(500);
      if(UP && DOWN){
         lcd_gotoxy(1,1);
         printf(lcd_putc,"Cargando...");
         delay_ms(500);
         printf(lcd_putc,"\f");
         for(;;){
            leeControlRemoto();
            lcd_gotoxy(1,1);
            printf(lcd_putc,"Codigo Subir: %X   ",CodigoSubir);
            lcd_gotoxy(1,2);
            printf(lcd_putc,"Codigo Bajar: %X   ",CodigoBajar);
            lcd_gotoxy(1,3);
            printf(lcd_putc,"Codigo Detener: %X ",CodigoDetener);
            lcd_gotoxy(1,4);
            printf(lcd_putc,"Codigo:%X %X %X %X",BYTE_IR[0],BYTE_IR[1],BYTE_IR[2],BYTE_IR[3]);
            
            if(UP){
               delay_ms(500);
               if(UP){
                  codigoSubir=BYTE_IR[3];
                  write_eeprom(15,codigoSubir);
                  delay_ms(20);
                  Buzzer_on;delay_ms(500);Buzzer_off;   
               }
            }
            
            if(DOWN){
               delay_ms(500);
               if(DOWN){
                  codigoBajar=BYTE_IR[3];
                  write_eeprom(16,codigoBajar);
                  delay_ms(20);
                  Buzzer_on;delay_ms(500);Buzzer_off;   
               }
            }
            
            if(RIGHT){
               delay_ms(500);
               if(RIGHT){
                  codigoDetener=BYTE_IR[3];
                  write_eeprom(17,codigoDetener);
                  delay_ms(20);
                  Buzzer_on;delay_ms(500);Buzzer_off;   
               }
            }
            
            if(IZQU){
               delay_ms(500);
               if(IZQU){
                  reset_cpu();
               }
            }
         }
      }
   }
   */
   while(true){
   //leeControlRemoto();
   
   if(VelMotor==6){
      Motor_on;
      Motor2_on; 
      set_timer0(0);
   }
   
   if(VelMotor==0){
      Motor_off;
      Motor2_off; 
      set_timer0(0);
   }
//------------Menu0------------------------------------------------------------------   
   if(Menu == 0){ // Menu de Contraseña para Poder iniciar el equipo
      //if(Cambio){
         lcd_gotoxy(1,1);
         printf(lcd_putc,"     Ingrese        ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"    Contraseña      ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
         lcd_gotoxy(unidad,4);// Para mostrar cursor.
         lcd_putc(t[2]);
         
         if(unidad>11&&unidad<14){
            lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
            lcd_putc(t[1]);
         }
         
         Cambio=OFF;
      //}
      
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP){
            if(Flanco == 0) {
               clave[unidad-11]++;Flanco = 1;delay_ms(30);Cambio=ON;
            }
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN){
            if(Flanco2 == 0) {
               clave[unidad-11]--;Flanco2 = 1;delay_ms(30);Cambio=ON;
            }
         }
      }else{
         Flanco2 = 0;
      }
   
      if(RIGHT){// Si Oprime Derecha
         delay_ms(20);
         if(RIGHT){
            if(Flanco1 == 0) {
               Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");Cambio=ON;
            }
         }
      }else{
         Flanco1 = 0;
      }
            
      if(IZQU){// Si Oprime izquierda
         delay_ms(150);
         if(IZQU){
            if(Flanco3 == 0){
               Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");Cambio=ON;
            }
         }
      }else{
        Flanco3 = 0;
      }
            
      if(clave[unidad-11]<0)     // Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.
         clave[unidad-11]=9;
         
      if(clave[unidad-11]>9)     // Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.
         clave[unidad-11]=0;
       
      if(unidad<11)             // Si trata de correr mas a la izquierda de la primera unidad, deja el cursor en esa posicion.
         unidad=11;
            
      if(unidad>14){             // Si a Terminado de ingresar la clave, verifica si es correcta o no.
         if(clave[0]==3&&clave[1]==8&&clave[2]==9&&clave[3]==2){ // Si Ingresa clave para reset general del sistema.
            write_eeprom(0,0);delay_ms(20);write_eeprom(1,0);delay_ms(20);// Reestablece a contraseña de Fabrica y reinicia Programa.
             write_eeprom(2,0);delay_ms(20);write_eeprom(3,0);delay_ms(20);
             reset_cpu();
         }
            
         if(clave[0]==contrasena[0]&&clave[1]==contrasena[1]&&clave[2]==contrasena[2]&&clave[3]==contrasena[3]){ // Si las claves coinciden pasa a Menu Principal.
            lcd_gotoxy(1,1);
            printf(lcd_putc,"                   ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña    ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"      Correcta     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                   ");
            delay_ms(1000);Menu=50;unidad=11;printf(lcd_putc,"\f");Cambio=ON;
            clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;Cambio=ON;
         }
         else{                                         // Si la clave no coincide vuelve a mostrar el menu para ingresar la clave.
            lcd_gotoxy(1,1);
            printf(lcd_putc,"");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña    ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Incorrecta    ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                   ");
            delay_ms(1000);unidad=11;printf(lcd_putc,"\f");Cambio=ON;
            clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;Cambio=ON;
         }
      }
   }
//----------------Fin-Menu0---------------------------------------------------------------   

//------------Menu1------------------------------------------------------------------   
   if(Menu == 1){ // Menu de seleccion de lo que desea encender
      EliminaRuido();
      //if(Cambio){
         lcd_gotoxy(1,1);
         printf(lcd_putc,"---MENU PRINCIPAL---");
         mensajes(1+paso,2);
         mensajes(2+paso,3);
         mensajes(3+paso,4);

        lcd_gotoxy(1,Flecha);// Para mostrar la flecha de seleccion
        lcd_putc(t[0]);

        if(Flecha==2)
          {lcd_gotoxy(1,4);// Para mostrar la flecha de seleccion
            lcd_putc(t[1]);
            lcd_gotoxy(1,3);// Para mostrar la flecha de seleccion
            lcd_putc(t[1]);
          }
        
        if(Flecha==4)
          {lcd_gotoxy(1,2);// Para mostrar la flecha de seleccion
            lcd_putc(t[1]);
            lcd_gotoxy(1,3);// Para mostrar la flecha de seleccion
            lcd_putc(t[1]);
          }
        
        if(Flecha==3)
          { lcd_gotoxy(1,4);// Para mostrar la flecha de seleccion
            lcd_putc(t[1]);
            lcd_gotoxy(1,2);// Para mostrar la flecha de seleccion
            lcd_putc(t[1]);
          }
         Cambio=OFF;
     // }
        
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP){
            if(Flanco == 0){
               Flecha2--;Flecha--;Flecha1=Flecha+1;Flanco = 1;delay_ms(30);Cambio=ON;
            }
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN){
            if(Flanco2 == 0){
               Flecha2++;Flecha++;Flecha1=Flecha-1;Flanco2 = 1;delay_ms(30);Cambio=ON;
            }
         }
      }else{
         Flanco2 = 0;
      }
   
        
      if(Flecha2>nMenuH){
         paso++;nMenuH=Flecha2;nMenuL=nMenuH-2;Flecha=4;
      }
        
      if(Flecha2<nMenuL){
         paso--;nMenuL=Flecha2;nMenuH=nMenuL+2;Flecha=2;
      }
        
      if(Flecha2>n_opcionH){
         Flecha2=n_opcionL;Flecha=2;paso=0;nMenuL=Flecha2;nMenuH=nMenuL+2;
      }
        
      if(Flecha2<n_opcionL){
         Flecha2=n_opcionH;Flecha=4;paso=n_opcionH-4;nMenuH=Flecha2;nMenuL=nMenuH-2;
      }
        
      if(paso<0)
         paso=0;
         
      if(RIGHT){// Si oprime derecha
         delay_ms(20);
         if(RIGHT){
            if(Flanco1 == 0){
               Flanco1 = 1;Menu=Flecha2;Flecha=3;delay_ms(300);printf(lcd_putc,"\f");
               if(Menu==6){
                  clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
               }
            }
         }
         Cambio=ON;
      }else{
        Flanco1 = 0;
      }
            
      if(IZQU){
         delay_ms(20);
         if(IZQU){
            delay_ms(500);Menu=50;printf(lcd_putc,"\f");Cambio=ON;
         }
      }
   }
//----------------Fin-Menu1---------------------------------------------------------------      

//----------------Menu2---------------------------------------------------------------
   if(Menu == 2){ // Menu de tiempo de trabajo de Luz UV   
   EliminaRuido();
   //if(Cambio){
      lcd_gotoxy(1,1);
      printf(lcd_putc,"    Temporizador     ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"       Luz UV        ");
      lcd_gotoxy(6,3);
      printf(lcd_putc,"%03u:%02u (M:S)  ",tempo_minutos_pro,tempo_segundos_pro); 
      
      lcd_gotoxy(unidad2+6,4);// Para mostrar cursor.
      lcd_putc(t[2]);
      
      Cambio=OFF;
//   }
   
   if(UP)
   {
      delay_ms(20);
      if(UP){
         if(unidad2==1)
         {
            if(tempo_minutos_pro<240)
            {
               tempo_minutos_pro+=5;delay_ms(300);
            }
         }
         
         if(unidad2==4)
         {
            if(tempo_segundos_pro<59)
            {
               tempo_segundos_pro++;delay_ms(300);
            }
         }     
      Cambio=ON;
      }
   }
   
   if(DOWN)
   {
      delay_ms(20);
      if(DOWN){
         if(unidad2==1)
         {
            if(tempo_minutos_pro>0)
            {
               tempo_minutos_pro-=5;delay_ms(300);
            } 
         }
         
         if(unidad2==4)
         {
            if(tempo_segundos_pro>0)
            {
               tempo_segundos_pro--;delay_ms(300);
            }   
         }
      Cambio=ON;
      }
   }
   
   if(RIGHT)
   {
      delay_ms(20);
      if(RIGHT){
         if(unidad2==1)
         {
            unidad2=4;
         }
         else
         {
            if(unidad2==4)
            {
               unidad2=1;
            }
         }
      delay_ms(500);
      printf(lcd_putc,"\f");
      Cambio=ON;
      }
   }
   
   if(IZQU)
   {
      if(IZQU){
         delay_ms(200);
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Tiempo Almacenado");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"  Correctamente  ");
         write_eeprom(5,tempo_minutos_pro);
         delay_ms(20);
         write_eeprom(6,tempo_segundos_pro);
         delay_ms(20);
         tempo_minutos=tempo_minutos_pro;tempo_segundos=tempo_segundos_pro;
         delay_ms(700);
         delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
         Cambio=ON;
      }
   }  
  }
//----------------Fin-Menu2---------------------------------------------------------------

//----------------Menu3---------------------------------------------------------------
   if(Menu == 3){ // Menu para Cambio de Contraseña
      EliminaRuido();
      //if(Cambio){
         lcd_gotoxy(1,1);
         printf(lcd_putc,"     Ingrese        ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"    Contraseña      ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
         lcd_gotoxy(unidad,4);// Para mostrar cursor.
         lcd_putc(t[2]);
         
         if(unidad>11&&unidad<14){
            lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
            lcd_putc(t[1]);
         }
         
         Cambio=OFF;
      //}
      
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP){
            if(Flanco == 0) {
               clave[unidad-11]++;Flanco = 1;delay_ms(30);Cambio=ON;
            }
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN){
            if(Flanco2 == 0) {
               clave[unidad-11]--;Flanco2 = 1;delay_ms(30);Cambio=ON;
            }
         }
      }else{
         Flanco2 = 0;
      }
   
      if(RIGHT){// Si Oprime Derecha
         delay_ms(20);
         if(RIGHT){
            if(Flanco1 == 0) {
               Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");Cambio=ON;
            }
         }
      }else{
         Flanco1 = 0;
      }
            
      if(IZQU){// Si Oprime izquierda
         delay_ms(150);
         if(IZQU){
            if(Flanco3 == 0){
               Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");Cambio=ON;
            }
         }
      }else{
        Flanco3 = 0;
      }
            
      if(clave[unidad-11]<0)     // Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.
         clave[unidad-11]=9;
         
      if(clave[unidad-11]>9)     // Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.
         clave[unidad-11]=0;
       
      if(unidad<11){             // Si trata de correr mas a la izquierda de la primera unidad, deja el cursor en esa posicion.
         unidad=11;
         Menu=1;
         paso=0;
         clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;Cambio=ON;
         unidad=11;printf(lcd_putc,"\f");
      }
       
      if(unidad>14){// Si ya ingreso la contraseña muestra si es correcta o no, dependiendo si ingreso la clave correctamente.
         if(clave[0]==contrasena[0]&&clave[1]==contrasena[1]&&clave[2]==contrasena[2]&&clave[3]==contrasena[3]){
            lcd_gotoxy(1,1);
            printf(lcd_putc,"                    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña     ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"      Correcta      ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                    ");
            delay_ms(500);Menu=61;unidad=11;printf(lcd_putc,"\f");
            clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;Cambio=ON;
         }else{
            lcd_gotoxy(1,1);
            printf(lcd_putc,"                    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña     ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Incorrecta     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                    ");
            delay_ms(500);unidad=11;printf(lcd_putc,"\f");Cambio=ON;
            clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
         }
      }
   }      
//----------------Fin-Menu3---------------------------------------------------------------

//----------------Menu6.1---------------------------------------------------------------
   if(Menu == 61){ // Menu cuando ingresa correctamente la contraseña, permite que digite nueva contraseña.
      EliminaRuido();
//      if(Cambio){
         lcd_gotoxy(1,1);
         printf(lcd_putc,"   Ingrese Nueva    ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"     Contraseña     ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
         lcd_gotoxy(unidad,4);// Para mostrar cursor.
         lcd_putc(t[2]);
         
         if(unidad>11&&unidad<14){
            lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
            lcd_putc(t[1]);
         }
         
         Cambio=OFF;
//      }
      
      if(UP){//Si oprime hacia arriba
         delay_ms(20);
         if(UP){
            if(Flanco == 0) {
               clave[unidad-11]++;Flanco = 1;delay_ms(30);Cambio=ON;
            }
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){//Si oprime hacia abajo
         delay_ms(20);
         if(DOWN){
            if(Flanco2 == 0) {
               clave[unidad-11]--;Flanco2 = 1;delay_ms(30);Cambio=ON;
            }
         }
      }else{
         Flanco2 = 0;
      }
   
      if(RIGHT){// Si Oprime Derecha
         if(RIGHT){
            if(Flanco1 == 0) {
               Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");Cambio=ON;
            }
         }
      }else{
         Flanco1 = 0;
      }
            
      if(IZQU){// Si Oprime izquierda
         delay_ms(150);
         if(IZQU){
            if(Flanco3 == 0){
               Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");Cambio=ON;
            }
         }
      }else{
        Flanco3 = 0;
      }
            
      if(clave[unidad-11]<0)     // Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.
         clave[unidad-11]=9;
         
      if(clave[unidad-11]>9)     // Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.
         clave[unidad-11]=0;
       
      if(unidad<11){             // Si trata de correr mas a la izquierda de la primera unidad, deja el cursor en esa posicion.
         unidad=11;
         Menu=1;
         clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;Cambio=ON;
         unidad=11;printf(lcd_putc,"\f");
      }
       
      if(unidad>14){// Si ya ingreso la nueva contraseña.     
         lcd_gotoxy(1,1);
         printf(lcd_putc,"                    ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"     Contraseña     ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"     Almacenada     ");
         lcd_gotoxy(1,4);
         printf(lcd_putc,"                    ");
         write_eeprom(0,clave[0]);delay_ms(20);write_eeprom(1,clave[1]);delay_ms(20);
         write_eeprom(2,clave[2]);delay_ms(20);write_eeprom(3,clave[3]);delay_ms(20);
         delay_ms(500);Menu=1;paso=0;Flecha=2;Flecha2=2;
         contrasena[0]=clave[0];contrasena[1]=clave[1];contrasena[2]=clave[2];contrasena[3]=clave[3];
         clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;Cambio=ON;
         unidad=11;printf(lcd_putc,"\f");         
      }
   }      
//----------------Fin-Menu6.1---------------------------------------------------------------
   
//----------------Menu4---------------------------------------------------------------
   if(Menu == 4){ // Menu de tiempo de trabajo de Luz UV
   EliminaRuido();
//   if(Cambio){
      lcd_gotoxy(1,1);
      printf(lcd_putc,"  Duracion Actual   ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"   Tiempo= %05Lu   ",horas);
      lcd_gotoxy(1,4);
      printf(lcd_putc," RESET= Oprima ^ y > ");
      
      Cambio=OFF;
//   }
   
      if(UP && RIGHT){//Si oprime hacia arriba
         delay_ms(20);
         if(UP && RIGHT){
            delay_ms(200);
            printf(lcd_putc,"\f");
            lcd_gotoxy(1,2);
            printf(lcd_putc," Reset de tiempo ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Exitoso     ");
            write_eeprom(7,0);
            delay_ms(20);
            write_eeprom(8,0);
            delay_ms(20);
            write_eeprom(9,0);
            delay_ms(20);
            write_eeprom(10,0);
            delay_ms(700);
            segundos=0;minutos=0;horas=0;
            delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
            Cambio=ON;
         }
      }
            
      if(IZQU){// Si oprime Izquierda
         delay_ms(20);
         if(IZQU){
            if(Flanco3 == 0){
               Flanco3 = 1;delay_ms(500);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");Cambio=ON;
            }
         }
      }else{
         Flanco3 = 0;
      }
   }
//----------------Fin-Menu4---------------------------------------------------------------

//----------------Menu5---------------------------------------------------------------
   if(Menu == 5){ // Menu de Punto Cero
      EliminaRuido();
//      if(Cambio){
         lcd_gotoxy(1,1);
         printf(lcd_putc,"  Zero Point Config ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Presion= %3.2f inH2O ",(Leer_Sensor_Presion(5)*4.02));
         lcd_gotoxy(1,3);
         printf(lcd_putc,"    ADC=%2.0f",sensores(0));
         lcd_gotoxy(1,4);
         printf(lcd_putc,"    ZF=%2.0f",G);
         
         Cambio=OFF;
//      }
    
      if(RIGHT){// Si oprime derecha
         delay_ms(20);
         if(RIGHT){
            if(Flanco1 == 0){
               /*Flanco1 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
               G=sensores(0);
               G16=(int16)G;
               G_l=G16; G_h=(G16>>8);
               
               write_eeprom(11,G_l);
               delay_ms(20);
               write_eeprom(12,G_h);//Guardar valor de Setpoint en eeprom
               delay_ms(20);
               Cambio=ON;
            }
         }
      }else{
         Flanco1 = 0;
      }
            
      if(IZQU){// Si oprime Izquierda
         delay_ms(20);
         if(IZQU){
            if(Flanco3 == 0){
               /*Flanco3 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(30);printf(lcd_putc,"\f");
               Cambio=ON;
            }
         }
      }else{
         Flanco3 = 0;
      }
   }
//----------------Fin-Menu5---------------------------------------------------------------


//----------------Menu6---------------------------------------------------------------
   if(Menu == 6){ // Menu de seleccion de estado de Alarma
   estadoalarma=!estadoalarma;Flanco1 = 1;Menu=1; paso=0;
                      
            if(estadoalarma==1)
            {estadoalarma=1;
               lcd_gotoxy(1,1);
               printf(lcd_putc,"                    ");
               lcd_gotoxy(1,2);
               printf(lcd_putc,"        Activo      ");
               lcd_gotoxy(1,3);
               printf(lcd_putc,"        Alarma      ");
               lcd_gotoxy(1,4);
               printf(lcd_putc,"                    ");}
               
            if(estadoalarma==0)
            {estadoalarma=0;
               lcd_gotoxy(1,1);
               printf(lcd_putc,"                    ");
               lcd_gotoxy(1,2);
               printf(lcd_putc,"      Desactivo     ");
               lcd_gotoxy(1,3);
               printf(lcd_putc,"       Alarma       ");
               lcd_gotoxy(1,4);
               printf(lcd_putc,"                    ");}
             
            write_eeprom(4,estadoalarma);delay_ms(1000);Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
            Cambio=ON;
   }      
//----------------Fin-Menu6---------------------------------------------------------------

//----------------Menu7---------------------------------------------------------------
   if(Menu == 7){ // Menu de seleccion de Velocidad
      if(Vel1>6)
         Vel1=6;
      if(Vel1<1)
         Vel1=1;
      
      lcd_gotoxy(1,1);
      printf(lcd_putc,"Seleccione Velocidad");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"   del Ventilador   ");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"     Vel: %u       ",Vel1);
      lcd_gotoxy(1,4);
      printf(lcd_putc,"                    ");      
      
      if(UP && Vel1<6)//Si oprime hacia arriba
      {  
         delay_ms(200);Vel1++;
      }
            
      if(DOWN && Vel1>0)//Si oprime hacia abajo
      {  
         delay_ms(200);Vel1--;
      }
            
      if(RIGHT)//Si oprime SET
      {           
         VelMotor=Vel1;
      } 
            
      if(IZQU)//Si oprime boton de Toma.
      {                  
         delay_ms(100);Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
         write_eeprom(13,Vel1);//Guardar valor de Setpoint en eeprom
         delay_ms(20);
         lcd_gotoxy(1,1);
         printf(lcd_putc,"                    ");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"     Velocidad      ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"      Almacenada    ");
         lcd_gotoxy(1,4);
         printf(lcd_putc,"                    "); 
         delay_ms(1000);
         printf(lcd_putc,"\f");
         lcd_init();
         Menu=1; paso=0;
      }
   }      
//----------------Fin-Menu7---------------------------------------------------------------

//----------------Menu100---------------------------------------------------------------
   if(Menu == 50){ // Menu de seleccion de Estado de Luz UV   
   if(Lectura==1){
      Lectura=0;
      CaidaPresion=Leer_Sensor_Presion(8);
      lcd_gotoxy(1,1);
      printf(lcd_putc,"JPCR DP:%3.2f inH2O ",(CaidaPresion*4.02));      
      Saturacion=(CaidaPresion/0.24)*11;
      
      if(Saturacion>11)
      {
         Saturacion=11;
      }
      
      for(i=0;i<floor(Saturacion);i++)
      {
         lcd_gotoxy(i+8,2);
         printf(lcd_putc,"Ñ");
      }
      
      for(i=floor(Saturacion);i<11;i++)
      {
         lcd_gotoxy(i+8,2);
         printf(lcd_putc," ");
      }
      
      lcd_gotoxy(9,3);
      printf(lcd_putc,"Tempo:%03u:%02u",tempo_minutos,tempo_segundos);
      EliminaRuido();
   }
    
   if(tiempo_cumplido==1)
   {
      if(estadoalarma==1)
      {
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"  Temporizador  de  ");
         lcd_gotoxy(1,3);
         printf(lcd_putc," Luz UV  Finalizado ");
         lcd_gotoxy(1,4);
         printf(lcd_putc,"                    ");
         lcd_gotoxy(1,1);
         printf(lcd_putc,"                    ");
         for(i=0;i<5;i++)
         {
            Buzzer_on;
            delay_ms(200);
            Buzzer_off;
            delay_ms(200);
         }
      }
      tiempo_cumplido=0;
      printf(lcd_putc,"\f");
   }
   
   if(CaidaPresion>=0.23)
   { 
      ActivaBuzzer=1;
   }
   else
   {
      ActivaBuzzer=0;
      Buzzer_off;
   }
      
   //if(Cambio){
      lcd_gotoxy(1,2);
      printf(lcd_putc,"Filtro:");
      
      if(estadomv==1)
      {
         lcd_gotoxy(1,4);
         printf(lcd_putc,"Motor:ON ");      
      }
      else
      {
         lcd_gotoxy(1,4);
         printf(lcd_putc,"Motor:OFF");
      }
      
      if(estadouv==1)
      {
         lcd_gotoxy(13,4);
         printf(lcd_putc,"UV:ON ");
      }
      else
      {         
         lcd_gotoxy(13,4);
         printf(lcd_putc,"UV:OFF");
      }
      
      if(estadofl==1)
      {
         lcd_gotoxy(1,3);
         printf(lcd_putc,"Luz:ON ");
      }
      else
      {
         lcd_gotoxy(1,3);
         printf(lcd_putc,"Luz:OFF");
      } 
      Cambio=OFF;
   //}
   
   if(estadomv==1){ 
      if(flagmv==0){
         VelMotor=6;
         lcd_init();delay_ms(200);lcd_init();
         flagmv=1;Cambio=ON;delay_ms(200);
      }
      VelMotor=Vel1;
   }else{
      VelMotor=0;
      if(flagmv==1){
         lcd_init();delay_ms(200);lcd_init();
         flagmv=0;Cambio=ON;
      }
   }
   
   if(estadofl==1)
   {
      Luz_Blanca_on;
      if(flagfl==0)
      {
         lcd_init();delay_ms(200);lcd_init();
         flagfl=1;Cambio=ON;
      }
   }
   else
   {
      Luz_Blanca_off;
      if(flagfl==1)
      {
         lcd_init();delay_ms(200);lcd_init();
         flagfl=0;Cambio=ON;
      }
   }
   
   //if(VIDRIODN){
      //estadomv=0;
      //VelMotor=0;
      //estadofl=0;
      //Luz_Blanca_off;
      if(estadouv==1){
         Luz_UV_on;
         tiempo_cumplido=0;      
         estadomv=0;
         VelMotor=0;
         estadofl=0;
         Luz_Blanca_off;
         if(flaguv==0){
            lcd_init();delay_ms(200);lcd_init();
            flaguv=1;guardaruv=1;Cambio=ON;
         }
      }else{
         estadouv=0;
         Luz_UV_off;
         if(flaguv==1){
            lcd_init();delay_ms(200);lcd_init();
            flaguv=0;guardaruv=1;Cambio=ON;
         }
      }
   //}else{
   //   estadouv=0;
   //   Luz_UV_off;
   //   if(flaguv==1){
         //lcd_init();delay_ms(200);lcd_init();
   //      flaguv=0;guardaruv=1;Cambio=ON;
   //   }
   //}
   
   if(DOWN){//Si oprime hacia Abajo
      Flanco=1;
      if(!flagdn){
         delay_ms(500);
         if(DOWN){
            flagdn=1;
         }else{
            estadofl=!estadofl;Flanco = 1;delay_ms(300);Cambio=ON;
         }
      }
   }else{
      flagdn=0;
      Flanco=0;
   } 
   
   if(RIGHT)//Si oprime hacia abajo
   {
      delay_ms(20);
      if(RIGHT){
         if(Flanco2 == 0)
         {
            estadomv=!estadomv;Flanco1 = 1;delay_ms(300);Cambio=ON;
         }
      }
   }
   else
   {
      Flanco2 = 0;
   }
   
   if(IZQU)//Si oprime hacia izquierda
   {
      delay_ms(20);
      if(IZQU){
         delay_ms(150); 
         estadouv=!estadouv;
         Cambio=ON;
      }
   }
   
   if(UP){// Si oprime Arriba
      Flanco1=1;
      if(!flagup){
         delay_ms(500);
         if(UP){
            flagup=1;
         }else{
            delay_ms(200);
            printf(lcd_putc,"\f");
            delay_ms(300);
            Menu=1; 
            paso=0;
            Flecha=2;
            Flecha2=2;
            Cambio=ON;
         }
      }
   }else{
      flagup=0;
      Flanco1=0;
   }
   
   if(flagup){
      subirVidrio();
   }else{
      if(flagdn){
         bajarVidrio();
      }else{
         detenerVidrio();
      }
   }
      
   if(guardaruv==1)
   {
      write_eeprom(7,segundos);write_eeprom(8,minutos);
      horasL=make8(horas,0);
      horasH=make8(horas,1);
      write_eeprom(9,horasL);
      delay_ms(20);
      write_eeprom(10,horasH);
      delay_ms(20);
      guardaruv=0;
   }
   
   if(segundos>=60)
   {
      segundos=0;minutos++;
   }
   if(minutos==60)
   {
      minutos=0;horas++;
   }  
    
   }
//----------------Fin-Menu100--------------------------------------------------------------  
   if(tiemporeset>=60)
   {
      printf(lcd_putc,"\f");
      lcd_init();
      delay_ms(200);
      tiemporeset=0;
      Cambio=ON;
   }   
   }
}

int8 BIT_BYTE(int1 D0, int1 D1, int1 D2, int1 D3, int1 D4, int1 D5, int1 D6, int1 D7){
   int8 dato;
   dato= D7*128 + D6*64 + D5*32 + D4*16 + D3*8 + D2*4 + D1*2 +D0*1; // 0 al 255 
   return dato; 
}
