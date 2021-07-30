int8 BIT_BYTE(int1 D7, int1 D6, int1 D5, int1 D4, int1 D3, int1 D2, int1 D1, int1 D0);

void mensajes(int8 x,y)// Funcion para imprimir mensajes de Menu Principal.
{  
   if(x==1)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Temporizador UV    ");}
   if(x==2)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Cambio Contraseña  ");}
   if(x==3)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Tiempo Trabajo UV  ");}
   if(x==4)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Punto Cero Presion ");}
   
   if(x==5)
   {if(estadoalarma==0)
      {lcd_gotoxy(2,y);printf(lcd_putc,"Alarma          OFF");}
   if(estadoalarma==1)
      {lcd_gotoxy(2,y);printf(lcd_putc,"Alarma           ON");}
   }
   
   if(x==6)
   {lcd_gotoxy(2,y);printf(lcd_putc,"Velocidad          ");}
}

float sensores(int x){
float y;set_adc_channel(x);delay_ms(10);y=read_adc();return (y);
}

float Leer_Sensor_Presion(int media){
   float promediopresion=0.0;   
   
   V1=sensores(0); 
   //V1 = (x_uno*5.0)/1023.0;   //Lectura de Divisor de Voltaje de PT100 con resistencia de 1k (+-10%)
   Presion=(V1/G)-1.0;// Presion=(Voltaje/Ganancia)-1
   //VS = (V1*5.0)/1023.0;   //Lectura de Divisor de Voltaje de PT100 con resistencia de 1k (+-10%)
   //G2=G;
   //VG = (G2*5.0)/1023.0;
   //Presion=(VS-VG);// Presion=(Voltaje/Ganancia)-1
   
   if(Presion>0)
      Presion=0.0;
  
   if(Presion<0.0){
      Presion=Presion*-1;
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
   delay_ms(200);
   lcd_init();
   set_tris_a(0XFF);
   set_tris_e(0XFF);
   enable_interrupts(global);
   enable_interrupts(INT_EXT_L2H);
   //enable_interrupts(INT_EXT1_H2L);
   setup_timer_0(RTCC_8_BIT|RTCC_DIV_256);
   enable_interrupts(INT_TIMER0);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);
   enable_interrupts(INT_TIMER1);
   //setup_timer_2(T2_DIV_BY_16,0xfd,1);
   //enable_interrupts(INT_TIMER2);
   setup_adc_ports(AN0);
   setup_adc(ADC_CLOCK_DIV_32 );
   enable_interrupts(global);
}

void MensajeBienvenida (void){
   lcd_gotoxy(1,1);
   printf(lcd_putc,"       MODULO       ");
   lcd_gotoxy(1,2);
   printf(lcd_putc,"   FLUJO LAMINAR    ");
   lcd_gotoxy(1,3);
   printf(lcd_putc,"    JP  INGLOBAL    ");
   lcd_gotoxy(1,4);
   printf(lcd_putc,"  REF:JPCFLVMD48IX  ");
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
   Vel1=read_eeprom(13);
   delay_ms(20);
   codigoSubir=read_eeprom(15);
   delay_ms(20);
   codigoBajar=read_eeprom(16);
   delay_ms(20);
   codigoDetener=read_eeprom(17);
   delay_ms(20);
   G16=make16(G_h,G_l);
   G=G16;
   horasTrabajo=make16(read_eeprom(19),read_eeprom(18));
   delay_ms(20);
   segundosTrabajo=make16(read_eeprom(21),read_eeprom(20));
   delay_ms(20);
   motor=read_eeprom(22);
   delay_ms(20);
   luz=read_eeprom(23);
   delay_ms(20);
   velocidadInicial=read_eeprom(24);
   delay_ms(20);
   delay_ms(10);
   Entero=read_eeprom(25);
   delay_ms(10);
   Temporal=read_eeprom(26);
   delay_ms(10);
   Temporal=Temporal/100.0;
   Flujo[1].Ajuste=Entero+Temporal;
   delay_ms(10);
   Flujo[1].zero_actual=make16(read_eeprom(28),read_eeprom(27));
   delay_ms(10);
   
   Pr=read_eeprom(29);
   delay_ms(10);
   HR=read_eeprom(30);
   delay_ms(10);
   
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
   if(G<150 || G>300){
      G=220;write_eeprom(11,220);delay_ms(20);write_eeprom(12,0);delay_ms(20);
   }
   if(Vel1>20 || Vel1<1){
      Vel1=10;write_eeprom(13,10);delay_ms(20);
   }
   if(horasTrabajo>60000){
      horasTrabajo=0;write_eeprom(19,0);delay_ms(20);write_eeprom(18,0);delay_ms(20);
   }
   if(segundosTrabajo>3600){
      segundosTrabajo=0;write_eeprom(20,0);delay_ms(20);write_eeprom(21,0);delay_ms(20);
   }
   if(motor>1){
      motor=1;write_eeprom(22,1);delay_ms(20);
   }
   if(luz>1){
      luz=1;write_eeprom(23,1);delay_ms(20);
   }
   if(velocidadInicial>15){
      velocidadInicial=10;write_eeprom(24,10);delay_ms(20);
   }
   if(Flujo[1].Ajuste>10.0 || Flujo[1].Ajuste==0.0){
      Flujo[1].Ajuste=1.5;write_eeprom(25,1);write_eeprom(26,50);
   }
   if(Flujo[1].zero_actual>280 ||Flujo[1].zero_actual<180 ){
      Flujo[1].zero_actual=203;write_eeprom(27,203);write_eeprom(28,0);
   }
   if(Pr>110 || Pr<60){
      Pr=71;write_eeprom(29,71);
   }
   if(HR>100 || HR<20){
      HR=60;write_eeprom(30,60);
   }
}

void bajarVidrio(){
   pulsoSubir=OFF;
   if(VIDRIOUUP || !VIDRIODN){
      O1_on;O2_off;
   }else{
      O1_off;O2_off;
      pulsoBajar=OFF;
   }
}

void subirVidrio(){
   pulsoBajar=OFF;
   if(!VIDRIOUUP || VIDRIODN){
      O1_off;O2_on;
   }else{
      O1_off;O2_off;
      pulsoSubir=OFF;
   }
}

void detenerVidrio(){
   O1_off;O2_off;
}

void leeControlRemoto(){
   if(NUEVO_DATO==1){
      NUEVO_DATO=0;
      BYTE_IR[0]=BIT_BYTE(BITS[0],BITS[1],BITS[2],BITS[3],BITS[4],BITS[5],BITS[6],BITS[7]);
      BYTE_IR[1]=BIT_BYTE(BITS[8],BITS[9],BITS[10],BITS[11],BITS[12],BITS[13],BITS[14],BITS[15]);
      BYTE_IR[2]=BIT_BYTE(BITS[16],BITS[17],BITS[18],BITS[19],BITS[20],BITS[21],BITS[22],BITS[23]);
      BYTE_IR[3]=BIT_BYTE(BITS[24],BITS[25],BITS[26],BITS[27],BITS[28],BITS[29],BITS[30],BITS[31]);
   
      if(BYTE_IR[3]==codigoBajar){ //Bajar Vidrio  //AD
         pulsoBajar=ON;
      }
      
      if(BYTE_IR[3]==codigoSubir){ // Subir Vidrio //E7
         pulsoSubir=ON;
      }
     
      if(BYTE_IR[3]==codigoDetener){ //F7 E3 A5
         O1_off;O2_off;
         pulsoSubir=OFF;
         pulsoBajar=OFF;
      }
   }
   
   if(pulsoBajar)
      bajarVidrio();
      
   if(pulsoSubir)
      subirVidrio();    
     
}

void cuentaTiempoTrabajoFiltro(){
   //**Contador de Tiempo de Trabajo del filtro**//
   if(segundosTrabajo>=3600){ // Tiempo de Trabajo
      segundosTrabajo=0; //Minutos Trabajo realmente son segundos.
      write_eeprom(20,make8(segundosTrabajo,0));
      delay_ms(20);
      write_eeprom(21,make8(segundosTrabajo,1));
      delay_ms(20);
      horasTrabajo++;
      write_eeprom(18,make8(horasTrabajo,0));
      delay_ms(20);
      write_eeprom(19,make8(horasTrabajo,1));
      delay_ms(20);
      delay_ms(20);
   }else if(segundosTrabajo==900 || segundosTrabajo==1800 || segundosTrabajo==2700 ){
      write_eeprom(20,make8(segundosTrabajo,0));
      delay_ms(20);
      write_eeprom(21,make8(segundosTrabajo,1));
      delay_ms(20);
   }
//----------------------------------------//
}

void clearSalidasSinUsar(){
   output_bit(PIN_A3,0);
   //output_bit(PIN_C3,0);//Solo para PIC Diferente a 18F4550
   //output_bit(PIN_C4,0);
   //output_bit(PIN_C5,0);
   output_bit(PIN_C6,0);
   output_bit(PIN_C7,0);
   output_bit(PIN_B5,0);
} 

Float LeerFlujo(int8 media,int8 canal){
    float dif=0.0;
    Flujo[canal].adc=sensores(canal);
    
    dif=zero_fabrica-Flujo[canal].zero_actual;
    x_uno=Flujo[canal].adc+dif;
    x_uno=(x_uno/1023)*5;    
    Flujo[canal].Velocidad=(pow(x_uno,4)*p1)+(pow(x_uno,3)*p2)+(pow(x_uno,2)*p3)+(x_uno*p4)+p5;    
    
    if(Flujo[canal].Velocidad<0.0)
      Flujo[canal].Velocidad=0.0;
     
     if(Flujo[canal].l>media-1)
       {Flujo[canal].l=0;}
     
     Flujo[canal].promedio[Flujo[canal].l]=Flujo[canal].Velocidad;
     Flujo[canal].l++;
     Flujo[canal].Flujo=0;
     
     for(q=0;q<=(media-1);q++){
        Flujo[canal].Flujo+=Flujo[canal].promedio[q];
     } 
     Flujo[canal].Flujo=(Flujo[canal].Flujo/media)*Flujo[canal].Ajuste; 
     
     Flujo[canal].Flujo=Flujo[canal].Flujo/(densidad*0.0052810296*3600);//0.0052810296=Area con tubo de 8.2cm
     return Flujo[canal].Flujo;
}

void calculaDensidad(){
   Temp=20;
   TempK=Temp+273.15;
   Pa=Pr*1000;
   
   Psv=exp((0.000012378847*pow(TempK,2))+(-0.019121316*TempK)+33.93711047+(-6343.1645/TempK));
   f=1.00062+(0.0000000314*Pa)+(0.00000056*Temp);
   Xv=(HR/100)*f*(Psv/Pa);
   Z1=1-((Pa/TempK)*(0.00000158123+(-0.000000029331*Temp)+(0.00000000011043*pow(Temp,2))+((0.000005707+(-0.00000002051*Temp))*Xv)+((0.0001989+(-0.000002376*Temp)))*pow(Xv,2)))+((pow(Pa,2)/pow(TempK,2))*(0.0000000000183+(-0.00000000765*pow(Xv,2))));
   densidad=((Pa*0.028963512440)/(Z1*TempK*8.314472))*(1-(Xv*(1-(0.018015/0.028963512440))));
}
