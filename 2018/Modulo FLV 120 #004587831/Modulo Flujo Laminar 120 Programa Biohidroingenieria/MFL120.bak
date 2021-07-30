// Cuatro pulsadores de entrada (Derecha, Izquierda, arriba y abajo).
// Reloj con XT de 4 MHz.
// Programación para Cabina de PCR.
// Tiene Contraseña de incio para permitir funcionamiento de Cabina.
// Tiene Menú:Luz UV, Luz Blanca, Motor Ventilador y Cambio de Contraseña.
// Ing. Juan David Piñeros.
// JP Inglobal.

#include <18F4550.h>
#device adc=10
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL1,CPUDIV1,NOVREGEN,NOMCLR,NOPUT// PLL1 para 4 MHz
//#fuses XT,NOWDT,NOPROTECT,NOLVP,NODEBUG,VREGEN,NOMCLR// PLL1 para 4 MHz
#use delay(clock=48000000)
#include <LCD420.c>
#include <MATH.h>

// Definición de teclado - NC
#define   UP            input(PIN_A5)
#define   DOWN          input(PIN_E0)
#define   RIGHT         input(PIN_E1)
#define   IZQU          input(PIN_E2)
#define   VIDRIO        !input(PIN_A2)

// Definición de otros puertos
#define   Luz_UV_on         output_bit(PIN_D1,1)
#define   Luz_UV_off        output_bit(PIN_D1,0)
#define   Luz_Blanca_on     output_bit(PIN_D2,1)
#define   Luz_Blanca_off    output_bit(PIN_D2,0)
#define   Motor_L1_on       output_bit(PIN_D3,1)
#define   Motor_L1_off      output_bit(PIN_D3,0)
#define   Buzzer_on         output_bit(PIN_D0,1)
#define   Buzzer_off        output_bit(PIN_D0,0)

short estadouv=0,estadofl=0,estadomv=0,Flanco=0,Flanco1=0,Flanco2=0,Flanco3=0,tiempo_cumplido=0,Lectura=0,flaguv=0,flagfl=0,estadobuzzer=0,estadoalarma=0;
short flagmv=0;
int8 Menu=0, n_opcionH=6,n_opcionL=2,unidad=11,Flecha=2,Flecha1=3,Flecha2=2, nMenuL=2, nMenuH=4,segundos=0,minutos=0,horasL=0,horasH=0,G_l=0,G_h=0;
int8 tempo_minutos=0,tempo_segundos=0,tempo_minutos_pro=0,tempo_segundos_pro=0,r=0,q=0,i=0,unidad2=1,tiemporeset=0,guardaruv=0;
signed int8   paso=0;
char t[3]={'>',' ','^'}; 
signed  int8 clave[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
int8 contrasena[4]={0,0,0,0};   // 0=Verdadero, 1=Falso
int8 ActivaBuzzer=0;
int16 tiempos,horas=0,G16=0;
float PromPresion[20]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float V1=0.0,Presion=0.0,Saturacion=0.0,CaidaPresion=0.0;
float G=2;

#int_TIMER1
void temp1s(void){
   set_timer1(5536);  // 5 ms
   tiempos++;
   if(tiempos>=200)   // Ha transcurrido una decima de segundo (PIC18F4550 con XT = 16MHZ)
   {tiempos=0;tiemporeset++;Lectura=1;
   
   if(ActivaBuzzer==1)
   {
      if(estadobuzzer==0)
      {
         Buzzer_off;
      }
      else
      {
         if(estadoalarma==1)
            Buzzer_on;
      }
      estadobuzzer=!estadobuzzer;
   }
   //----------------------------- Tiempo Total-----------------------------//
   if(estadouv==1)
   {
      segundos++;
      if(tiempo_cumplido==0)
      {
         if(tempo_segundos>0)
         {
            tempo_segundos--;
         }
         else
         {
            if(tempo_minutos>0)
            {
               tempo_minutos--;
               tempo_segundos=59;
            }
            else
            {
               tiempo_cumplido=1;
               estadouv=0;Luz_UV_off;
               tempo_minutos=tempo_minutos_pro;tempo_segundos=tempo_segundos_pro;
            }
         }
      }
   }
        
   }
}


void mensajes(int8 x,y)// Funcion para imprimir mensajes de Menu Principal.
{  
   if(x==1)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Temporizador UV    ");}
   if(x==2)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Cambio Contraseña  ");}
   if(x==3)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Tiempo Trabajo UV  ");}
   if(x==4)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Punto Cero         ");}
   
   if(x==5)
   {if(estadoalarma==0)
      {lcd_gotoxy(2,y);printf(lcd_putc,"Alarma          OFF");}
   if(estadoalarma==1)
      {lcd_gotoxy(2,y);printf(lcd_putc,"Alarma           ON");}
   }
}

void velocidades(int8 x)// Funcion para imprimir mensajes de Menu Principal.
{  if(x==0)
   {
      Motor_L1_off;
   }
   else
   {
      Motor_L1_on;
   }
}

float sensores(int x){
float y;set_adc_channel(x);delay_ms(10);y=read_adc();return (y);
}

float Leer_Sensor_Presion(int media){
   float promediopresion=0.0;
   
   V1=sensores(0); 
   //V1 = (x_uno*5.0)/1023.0;   //Lectura de Divisor de Voltaje de PT100 con resistencia de 1k (+-10%)
   Presion=(V1/G)-1.0;// Presion=(Voltaje/Ganancia)-1
   
   if(Presion<0.0)
   {
      Presion=0.0;
   }

   if(r>media-1)
   {r=0;}
   PromPresion[r]=Presion;r++;
         
   for(q=0;q<=(media-1);q++)
   {
      promediopresion+=PromPresion[q];
   } 
   promediopresion=promediopresion/media;   
   
   return promediopresion;
}

void Limita(void){
   
   if(Contrasena[0]>9)
      Contrasena[0]=0;
   if(Contrasena[1]>9)
      Contrasena[1]=0;
   if(Contrasena[2]>9)
      Contrasena[2]=0;
   if(Contrasena[3]>9)
      Contrasena[3]=0;
   /*if(tempo_minutos>60)
      tempo_minutos=0;
   if(tempo_segundos)
      tempo_segundos=0;
   if(segundos>60)
      segundos=0;
   if(minutos>60)
      minutos=0;*/ 
}
void main ()
{
   lcd_init();
   set_tris_a(0XFF);
   set_tris_e(0XFF);
   set_tris_d(0X00);
   output_d(0);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);
   enable_interrupts(INT_TIMER1);
   setup_adc_ports(AN0);
   setup_adc(ADC_CLOCK_DIV_32 );
   enable_interrupts(global);
   sensores(0);
   lcd_gotoxy(1,1);
   printf(lcd_putc," CABINA  DE   FLUJO ");
   lcd_gotoxy(1,2);
   printf(lcd_putc," LAMINAR HORIZONTAL ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"    JP INGLOBAL     ");
   lcd_gotoxy(1,4);
   printf(lcd_putc,"   REF:  JPCFLH90   ");
   delay_ms(2000);
   printf(lcd_putc,"\f");   
   
   // Lee la contraseña almacenada en la eeprom para poder comprobar con la que escribe el usuario.
   contrasena[0]=read_eeprom(0);delay_ms(20); contrasena[1]=read_eeprom(1);delay_ms(20);
   contrasena[2]=read_eeprom(2);delay_ms(20); contrasena[3]=read_eeprom(3);delay_ms(20);
   estadoalarma=read_eeprom(5);
   tempo_minutos=read_eeprom(20);tempo_segundos=read_eeprom(30);
   segundos=read_eeprom(50);minutos=read_eeprom(51);horasL=read_eeprom(52);horasH=read_eeprom(53); 
   
   G_l=read_eeprom(60);
   G_h=read_eeprom(61);
   G16=make16(G_h,G_l);
   G=G16;

   Limita();
   horas=make16(horasH,horasL);
   tempo_minutos_pro=tempo_minutos;tempo_segundos_pro=tempo_segundos;
   //segundos=read_eeprom(50);minutos=read_eeprom(51);horas=read_eeprom(52);
  
   while(true){

//------------Menu0------------------------------------------------------------------   
   if(Menu == 0){ // Menu de Contraseña para Poder iniciar el equipo
   lcd_gotoxy(1,1);
   printf(lcd_putc,"     Ingrese        ");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"    Contraseña      ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
   
      
      if(UP)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {clave[unidad-11]++;Flanco = 1;delay_ms(30);}}
         else
            {Flanco = 0;}
            
      if(DOWN)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {clave[unidad-11]--;Flanco2 = 1;delay_ms(30);}}
         else
            {Flanco2 = 0;}
   
      if(RIGHT)// Si Oprime Derecha
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");}}
         else
            {Flanco1 = 0;}   
            
       if(IZQU)// Si Oprime izquierda
       {
         delay_ms(150);
         if(Flanco3 == 0)
         {
            Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");
         }
       }
       else
       {
         Flanco3 = 0;
       }
            
       if(clave[unidad-11]<0)     // Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.
       {clave[unidad-11]=9;}
       if(clave[unidad-11]>9)     // Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.
       {clave[unidad-11]=0;}
       if(unidad<11)             // Si trata de correr mas a la izquierda de la primera unidad, deja el cursor en esa posicion.
       {unidad=11;}
       lcd_gotoxy(unidad,4);// Para mostrar cursor.
       lcd_putc(t[2]);
       if(unidad>14)             // Si a Terminado de ingresar la clave, verifica si es correcta o no.
       {
         if(clave[0]==3&&clave[1]==8&&clave[2]==9&&clave[3]==2) // Si Ingresa clave para reset general del sistema.
            {write_eeprom(0,0);delay_ms(20);write_eeprom(1,0);delay_ms(20);// Reestablece a contraseña de Fabrica y reinicia Programa.
             write_eeprom(2,0);delay_ms(20);write_eeprom(3,0);delay_ms(20);
             reset_cpu();}
            
         if(clave[0]==contrasena[0]&&clave[1]==contrasena[1]&&clave[2]==contrasena[2]&&clave[3]==contrasena[3]) // Si las claves coinciden pasa a Menu Principal.
            {lcd_gotoxy(1,1);
            printf(lcd_putc,"                   ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña    ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"      Correcta     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                   ");
            delay_ms(1000);Menu=50;unidad=11;printf(lcd_putc,"\f");}
         else                                         // Si la clave no coincide vuelve a mostrar el menu para ingresar la clave.
         {lcd_gotoxy(1,1);
            printf(lcd_putc,"");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña    ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Incorrecta    ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                   ");
            delay_ms(1000);unidad=11;printf(lcd_putc,"\f");}
       }
   
       if(unidad>11&&unidad<14)
          { lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
            lcd_putc(t[1]);
          }

   }
//----------------Fin-Menu0---------------------------------------------------------------   

//------------Menu1------------------------------------------------------------------   
   if(Menu == 1){ // Menu de seleccion de lo que desea encender
   lcd_gotoxy(1,1);
   printf(lcd_putc,"---MENU PRINCIPAL---");
      if(paso<0)
        {paso=0;}
        
      if(UP)//Si oprime hacia arriba
      {
         if(Flanco == 0)
         {
            Flecha2--;Flecha--;Flecha1=Flecha+1;Flanco = 1;delay_ms(30);
         }
      }
      else
      {
         Flanco = 0;
      }
            
      if(DOWN)//Si oprime hacia abajo
      {
         if(Flanco2 == 0)
         {
            Flecha2++;Flecha++;Flecha1=Flecha-1;Flanco2 = 1;delay_ms(30);
         }
      }
      else
      {
         Flanco2 = 0;
      }
   
        
        if(Flecha2>nMenuH)
        {paso++;nMenuH=Flecha2;nMenuL=nMenuH-2;Flecha=4;}
        
        if(Flecha2<nMenuL)
        {paso--;nMenuL=Flecha2;nMenuH=nMenuL+2;Flecha=2;}
        
        if(Flecha2>n_opcionH)
        {Flecha2=n_opcionL;Flecha=2;paso=0;nMenuL=Flecha2;nMenuH=nMenuL+2;}
        
        if(Flecha2<n_opcionL)
        {Flecha2=n_opcionH;Flecha=4;paso=n_opcionH-4;nMenuH=Flecha2;nMenuL=nMenuH-2;}               
        
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

       if(RIGHT)// Si oprime derecha
       {
         if(Flanco1 == 0)
         {
            Flanco1 = 1;Menu=Flecha2;Flecha=3;delay_ms(300);printf(lcd_putc,"\f");
            if(Menu==6)
            {
               clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;
            }
         }
       }
       else
       {
         Flanco1 = 0;
       }
            
       if(IZQU)
       {
         delay_ms(500);Menu=50;printf(lcd_putc,"\f");
       }

   }
//----------------Fin-Menu1---------------------------------------------------------------      

//----------------Menu2---------------------------------------------------------------
   if(Menu == 2){ // Menu de tiempo de trabajo de Luz UV   
   
   lcd_gotoxy(1,1);
   printf(lcd_putc,"    Temporizador     ");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"       Luz UV        ");
   lcd_gotoxy(6,3);
   printf(lcd_putc,"%03u:%02u (M:S)  ",tempo_minutos_pro,tempo_segundos_pro); 
   
   lcd_gotoxy(unidad2+6,4);// Para mostrar cursor.
   lcd_putc(t[2]);
   
   if(UP)
   {
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
   }
   
   if(DOWN)
   {
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
   }
   
   if(RIGHT)
   {
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
   }
   
   if(IZQU)
   {
      delay_ms(200);
      printf(lcd_putc,"\f");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"Tiempo Almacenado");
      lcd_gotoxy(1,3);
      printf(lcd_putc,"  Correctamente  ");
      write_eeprom(20,tempo_minutos_pro);
      delay_ms(30);
      write_eeprom(30,tempo_segundos_pro);
      tempo_minutos=tempo_minutos_pro;tempo_segundos=tempo_segundos_pro;
      delay_ms(700);
      delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
   }  
  }
//----------------Fin-Menu2---------------------------------------------------------------

//----------------Menu4---------------------------------------------------------------
   if(Menu == 3){ // Menu para Cambio de Contraseña
   lcd_gotoxy(1,1);
   printf(lcd_putc,"      Ingrese       ");
   lcd_gotoxy(1,2);
   printf(lcd_putc," Contraseña Actual  ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
   
      
   if(UP)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {clave[unidad-11]++;Flanco = 1;delay_ms(30);}}
         else
            {Flanco = 0;}
            
      if(DOWN)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {clave[unidad-11]--;Flanco2 = 1;delay_ms(30);}}
         else
            {Flanco2 = 0;}
   
      if(RIGHT)// Si oprime derecha
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");}}
         else
            {Flanco1 = 0;}   
            
       if(IZQU)// Si oprime Izquierda
      {  if(Flanco3 == 0) 
            {Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");}}
         else
            {Flanco3 = 0;}    
            
       if(clave[unidad-11]<0)// Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.     
       {clave[unidad-11]=9;}
       if(clave[unidad-11]>9)// Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.     
       {clave[unidad-11]=0;}
       if(unidad<11)
       {Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);}
       lcd_gotoxy(unidad,4);// Para mostrar la flecha de seleccion
       lcd_putc(t[2]);
       if(unidad>14)// Si ya ingreso la contraseña muestra si es correcta o no, dependiendo si ingreso la clave correctamente.
       {
         if(clave[0]==contrasena[0]&&clave[1]==contrasena[1]&&clave[2]==contrasena[2]&&clave[3]==contrasena[3])
            {lcd_gotoxy(1,1);
            printf(lcd_putc,"                    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña     ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"      Correcta      ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                    ");
            delay_ms(500);Menu=61;unidad=11;printf(lcd_putc,"\f");
            clave[0]=0;clave[1]=0;clave[2]=0;clave[3]=0;}
         else
         {lcd_gotoxy(1,1);
            printf(lcd_putc,"                    ");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"     Contraseña     ");
            lcd_gotoxy(1,3);
            printf(lcd_putc,"     Incorrecta     ");
            lcd_gotoxy(1,4);
            printf(lcd_putc,"                    ");
            delay_ms(500);unidad=11;printf(lcd_putc,"\f");}
       }
               
          if(unidad>11&&unidad<14)
          { lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
            lcd_putc(t[1]);
          }
   }      
//----------------Fin-Menu4---------------------------------------------------------------

//----------------Menu6.1---------------------------------------------------------------
   if(Menu == 61){ // Menu cuando ingresa correctamente la contraseña, permite que digite nueva contraseña.
   lcd_gotoxy(1,1);
   printf(lcd_putc,"     Ingrese        ");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"  Contraseña Nueva  ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"    CLAVE=%i%i%i%i  ",clave[0],clave[1],clave[2],clave[3]);
   
      
   if(UP)//Si oprime hacia arriba
      {  if(Flanco == 0) 
            {clave[unidad-11]++;Flanco = 1;delay_ms(30);}}
         else
            {Flanco = 0;}
            
      if(DOWN)//Si oprime hacia abajo
      {  if(Flanco2 == 0) 
            {clave[unidad-11]--;Flanco2 = 1;delay_ms(30);}}
         else
            {Flanco2 = 0;}
   
      if(RIGHT)// Si oprime Derecha
      {  if(Flanco1 == 0) 
            {Flanco1 = 1;unidad++;delay_ms(30);printf(lcd_putc,"\f");}}
         else
            {Flanco1 = 0;}   
            
       if(IZQU)// Si oprime Izquierda
      {  if(Flanco3 == 0) 
            {Flanco3 = 1;unidad--;delay_ms(30);printf(lcd_putc,"\f");}}
         else
            {Flanco3 = 0;}    
            
       if(clave[unidad-11]<0)// Si la unidad donde se encuentra ubicado el cursor es menor que 0 pasa a 9.     
       {clave[unidad-11]=9;}
       if(clave[unidad-11]>9)// Si la unidad donde se encuentra ubicado el cursor es mayor que 9 pasa a 0.     
       {clave[unidad-11]=0;}
       if(unidad<11)
       {Menu=5;unidad=11;}
       lcd_gotoxy(unidad,4);// Para mostrar la flecha de seleccion
       lcd_putc(t[2]);
       if(unidad>14)// Si ya ingreso la nueva contraseña.
       {
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
       }
      
           if(unidad>11&&unidad<14)
          { lcd_gotoxy(unidad-1,4);// Para mostrar cursor.
            lcd_putc(t[1]);
          }
   }      
//----------------Fin-Menu6.1---------------------------------------------------------------
   
//----------------Menu5---------------------------------------------------------------
   if(Menu == 4){ // Menu de tiempo de trabajo de Luz UV
   lcd_gotoxy(1,1);
   printf(lcd_putc,"  Duracion Actual   ");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"   Tiempo= %05Lu   ",horas);
   lcd_gotoxy(1,4);
   printf(lcd_putc," RESET= Oprima ^ y > ");
   
      if(UP && RIGHT)//Si oprime hacia arriba
      {  
         delay_ms(200);
         printf(lcd_putc,"\f");
         lcd_gotoxy(1,2);
         printf(lcd_putc," Reset de tiempo ");
         lcd_gotoxy(1,3);
         printf(lcd_putc,"     Exitoso     ");
         write_eeprom(50,0);write_eeprom(51,0);write_eeprom(52,0);
         delay_ms(700);
         segundos=0;minutos=0;horas=0;
         delay_ms(30);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
      }
            
      if(IZQU)// Si oprime Izquierda
      {
         if(Flanco3 == 0)
         {
            Flanco3 = 1;delay_ms(500);Menu=1; paso=0;Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
         }
      }
      else
      {
         Flanco3 = 0;
      }
   }
//----------------Fin-Menu5---------------------------------------------------------------

//----------------Menu5---------------------------------------------------------------
   if(Menu == 5){ // Menu de Punto Cero
      lcd_gotoxy(1,1);
      printf(lcd_putc,"  Zero Point Config ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"Presion= %3.2f inH2O ",(Leer_Sensor_Presion(5)*4.02));
      lcd_gotoxy(1,3);
      printf(lcd_putc,"    ADC=%2.0f",sensores(0));
      lcd_gotoxy(1,4);
      printf(lcd_putc,"    ZF=%2.0f",G);
    
      if(RIGHT)// Si oprime derecha
      {  if(Flanco1 == 0) 
            {/*Flanco1 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(500);printf(lcd_putc,"\f");
            G=sensores(0);
            G16=(int16)G;
            G_l=G16; G_h=(G16>>8);
            
            write_eeprom(60,G_l);write_eeprom(61,G_h);//Guardar valor de Setpoint en eeprom
            }}
         else
            {Flanco1 = 0;}   
            
       if(IZQU)// Si oprime Izquierda
      {  if(Flanco3 == 0) 
            {/*Flanco3 = 1;*/Menu=1;paso=0;Flecha=2;Flecha2=2;unidad=11;delay_ms(30);printf(lcd_putc,"\f");}}
         else
            {Flanco3 = 0;}  
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
             
            write_eeprom(5,estadoalarma);delay_ms(1000);Flecha=2;Flecha2=2;printf(lcd_putc,"\f");
   }      
//----------------Fin-Menu6---------------------------------------------------------------

//----------------Menu100---------------------------------------------------------------
   if(Menu == 50){ // Menu de seleccion de Estado de Luz UV

   if(Lectura==1)
   {
      Lectura=0;
      CaidaPresion=Leer_Sensor_Presion(5);
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
   
   if(CaidaPresion>=0.37)
   { 
      ActivaBuzzer=1;
   }
   else
   {
      ActivaBuzzer=0;
      Buzzer_off;
   }
      
   lcd_gotoxy(1,1);
   printf(lcd_putc,"JPCR DP:%3.2f inH2O ",(CaidaPresion*4.02));
   lcd_gotoxy(1,2);
   printf(lcd_putc,"Filtro:");
   
   Saturacion=(CaidaPresion/0.37)*11;
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
   
   if(estadomv==1)
   {
      lcd_gotoxy(1,4);
      printf(lcd_putc,"Motor:ON ");      
      velocidades(1);
      if(flagmv==0){
         lcd_init();delay_ms(200);lcd_init();flagmv=1;
      }
   }
   else
   {
      lcd_gotoxy(1,4);
      printf(lcd_putc,"Motor:OFF");
      velocidades(0); 
      if(flagmv==1){
         lcd_init();delay_ms(200);lcd_init();flagmv=0;
      }
   }
   
   if(estadouv==1)
   {
      lcd_gotoxy(13,4);
      printf(lcd_putc,"UV:ON ");
      estadomv=0;
      velocidades(0);
      estadofl=0;
      Luz_Blanca_off;
      Luz_UV_on;
      tiempo_cumplido=0;
      if(flaguv==0)
      {
         lcd_init();delay_ms(200);lcd_init();flaguv=1;guardaruv=1;
      }
   }
   else
   {
      estadouv=0;
      lcd_gotoxy(13,4);
      printf(lcd_putc,"UV:OFF");
      Luz_UV_off;
      if(flaguv==1)
      {
         lcd_init();delay_ms(200);lcd_init();flaguv=0;guardaruv=1;
      }
   }
   
   if(estadofl==1)
   {
      lcd_gotoxy(1,3);
      printf(lcd_putc,"Luz:ON ");
      Luz_Blanca_on;
      if(flagfl==0)
      {
         lcd_init();delay_ms(200);lcd_init();flagfl=1;
      }
   }
   else
   {
      lcd_gotoxy(1,3);
      printf(lcd_putc,"Luz:OFF");
      Luz_Blanca_off;
      if(flagfl==1)
      {
         lcd_init();delay_ms(200);lcd_init();flagfl=0;
      }
   }
   
  if(DOWN)//Si oprime hacia arriba
   {
      if(Flanco == 0)
      {
         estadofl=!estadofl;Flanco = 1;delay_ms(300);
      }
   }
   else
   {
      Flanco = 0;
   }
   
   if(RIGHT)//Si oprime hacia abajo
   {
      if(Flanco1 == 0)
      {
         estadomv=!estadomv;Flanco1 = 1;delay_ms(300);
      }
   }
   else
   {
      Flanco1 = 0;
   }
   
   if(IZQU)//Si oprime hacia izquierda
   {
      delay_ms(150); 
      estadouv=!estadouv;
   }
   
   if(UP)//Si oprime hacia izquierda
   {
      delay_ms(200);
      printf(lcd_putc,"\f");
      delay_ms(300);
      Menu=1; 
      paso=0;
      Flecha=2;
      Flecha2=2;
   }
    
   }
//----------------Fin-Menu100--------------------------------------------------------------   
   if(guardaruv==1)
   {
      write_eeprom(50,segundos);write_eeprom(51,minutos);
      horasL=(int8)horas;
      horasH=(int8)(horas>>8);
      write_eeprom(52,horasL);
      write_eeprom(53,horasH);
      guardaruv=0;
   }
  
   if(tiemporeset>=20)
   {
      if(Menu==50){
         lcd_init();delay_ms(200);lcd_init();delay_ms(20);
      }
      tiemporeset=0;
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
}

