void VelocidadMotor(int8 Vel){
   if(Vel==6){
      Motor_L1_on;
      Motor_L2_on;
   }else if(Vel==5){
      Porcentaje=15;
      retardo=2000;
   }else if(Vel==4){
      Porcentaje=25;
      retardo=3000;
   }else if(Vel==3){
      Porcentaje=35;
      retardo=3600;
   }else if(Vel==2){
      Porcentaje=45;
      retardo=4300;
   }else if(Vel==1){
      Porcentaje=50;
      retardo=5000;
   }else if(Vel==0){
      Porcentaje=100;
      Motor_L1_off;
      Motor_L2_off;
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
   
   if(x==6)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Velocidad         ");}
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

void EliminaRuido(void){
   /*if(Menu==50){
      dato[0]=lcd_getc(1,2);
      dato[1]=lcd_getc(2,2);
      dato[2]=lcd_getc(3,2);
      if(dato[0]!=70 || dato[1]!=105 || dato[2]!=108){
         lcd_init();delay_ms(200);lcd_init();
         Cambio=ON;
      }
   }*/
}

void ConfigInterrupciones (void){
   lcd_init();
   set_tris_a(0XFF);
   set_tris_e(0XFF);
   enable_interrupts(INT_EXT_L2H);
   setup_adc_ports(AN0);
   setup_adc(ADC_CLOCK_DIV_32 );
   enable_interrupts(global);
}

void MensajeBienvenida (void){
   lcd_gotoxy(1,1);
   printf(lcd_putc,"MODULO FLUJO LAMINAR");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"      VERTICAL      ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"    JP  INGLOBAL    ");
   lcd_gotoxy(1,4);
   printf(lcd_putc,"  REF:JPCFLVMD96IX  ");
   delay_ms(2000);
   printf(lcd_putc,"\f");
}

void LeeEEPROM (void){
   // Lee la contraseña almacenada en la eeprom para poder comprobar con la que escribe el usuario.
   contrasena[0]=read_eeprom(0);
   delay_ms(20); 
   contrasena[1]=read_eeprom(1);
   delay_ms(20);
   contrasena[2]=read_eeprom(2);
   delay_ms(20); 
   contrasena[3]=read_eeprom(3);
   delay_ms(20);
   estadoalarma=read_eeprom(4);
   delay_ms(20);
   tempo_minutos=read_eeprom(5);
   delay_ms(20);
   tempo_segundos=read_eeprom(6);
   delay_ms(20);
   segundos=read_eeprom(7);
   delay_ms(20);
   minutos=read_eeprom(8);
   delay_ms(20);
   horasL=read_eeprom(9);
   delay_ms(20);
   horasH=read_eeprom(10);
   delay_ms(20);
   G_l=read_eeprom(11);
   delay_ms(20);
   G_h=read_eeprom(12);
   delay_ms(20);
   VelMotor=read_eeprom(13);
   delay_ms(20);
   G16=make16(G_h,G_l);
   G=G16;
   horas=make16(horasH,horasL);
   tempo_minutos_pro=tempo_minutos;tempo_segundos_pro=tempo_segundos;
}

void LimitaValores(void){
   
   if(Contrasena[0]>9 || Contrasena[0]<0){
      Contrasena[0]=0;write_eeprom(0,0);delay_ms(20);
   }
   if(Contrasena[1]>9 || Contrasena[1]<0){
      Contrasena[1]=0;write_eeprom(1,0);delay_ms(20);
   }
   if(Contrasena[2]>9 || Contrasena[2]<0){
      Contrasena[2]=0;write_eeprom(2,0);delay_ms(20);
   }
   if(Contrasena[3]>9 || Contrasena[3]<0){
      Contrasena[3]=0;write_eeprom(3,0);delay_ms(20);
   }
   if(estadoalarma>1){
      estadoalarma=1;write_eeprom(4,1);delay_ms(20);
   }
   if(tempo_minutos>200){
      tempo_minutos=20;write_eeprom(5,20);delay_ms(20);
   }
   if(tempo_segundos>59){
      tempo_segundos=0;write_eeprom(6,0);delay_ms(20);
   }
   if(segundos>60){
      segundos=0;write_eeprom(7,0);delay_ms(20);
   }
   if(minutos>60){
      minutos=0;write_eeprom(8,0);delay_ms(20);
   }
   if(horas>10000){
      horas=0;write_eeprom(9,10);delay_ms(20);write_eeprom(10,0);delay_ms(20);
   }
   if(G>255){
      G=255;write_eeprom(11,255);delay_ms(20);write_eeprom(12,0);delay_ms(20);
   }
   if(VelMotor>6){
      VelMotor=4;write_eeprom(13,4);delay_ms(20);
   }
}
