void BorraBuffer(void){
RX_Buffer[0]=0;
RX_Buffer[1]=0;
RX_Buffer[2]=0;
RX_Buffer[3]=0;
RX_Buffer[4]=0;
RX_Buffer[5]=0;
RX_Buffer[6]=0;
RX_Buffer[7]=0;
RX_Buffer[8]=0;
RX_Buffer[9]=0;

RX_Buffer2[0]=0;
RX_Buffer2[1]=0;
RX_Buffer2[2]=0;
RX_Buffer2[3]=0;
RX_Buffer2[4]=0;
RX_Buffer2[5]=0;
RX_Buffer2[6]=0;
RX_Buffer2[7]=0;
RX_Buffer2[8]=0;
RX_Buffer2[9]=0;

}

void Envio_I2C(direccion, posicion, dato){

   i2c_start();            // Comienzo comunicaci?n
   i2c_write(direccion);   // Direcci?n del esclavo en el bus I2C
   i2c_write(posicion);    // Posici?n donde se guardara el dato transmitido
   i2c_write(dato);        // Dato a transmitir
   i2c_stop();             // Fin comunicaci?n
 }

void Lectura_I2C (byte direccion, byte posicion, byte &dato) {

   i2c_start();            // Comienzo de la comunicaci?n
   i2c_write(direccion);   // Direcci?n del esclavo en el bus I2C
   i2c_write(posicion);    // Posici?n de donde se leer? el dato en el esclavo
   i2c_start();            // Reinicio
   i2c_write(direccion+1); // Direcci?n del esclavo en modo lectura
   dato=i2c_read(0);       // Lectura del dato
   i2c_stop();             // Fin comunicaci?n
}

void Carga_Vector(void){
   SlaveA0Tx[0]=instruccion;
   SlaveA0Tx[1]=address;
   SlaveA0Tx[2]=regl;
   SlaveA0Tx[3]=regh;
   SlaveA0Tx[4]=pwm1l;
   SlaveA0Tx[5]=pwm1h;
   SlaveA0Tx[6]=pwm2l;
   SlaveA0Tx[7]=pwm2h;
   SlaveA0Tx[8]=txbuf[0];
   SlaveA0Tx[9]=txbuf[49];
}

void Lee_Vector(void){
   
}

void Envio_Esclavos(void){
   Carga_Vector();   
   for(i=0;i<10;i++)
   {
      direccion=0xB0;
      Envio_I2C(direccion,i,SlaveA0Tx[i]);
   } 
}
void Lectura_Esclavos(void){
   Lectura_I2C(0xB0, 0, dato);    //Lectura por I2C
   txbuf[0]=(int8)dato;
   Lectura_I2C(0xB0, 49, dato);    //Lectura por I2C
   txbuf[49]=(int8)dato;
   if(txbuf[0]==10 && txbuf[49]==128){
      for(i=1;i<74;i++){
         direccion=0xB0;                        //Direcci?n en el bus I2c
         posicion=i;                         //Posici?n de memoria a leer
         Lectura_I2C(direccion, posicion, dato);    //Lectura por I2C
         txbuf[i]=(int8)dato;
      }
      Lee_Vector();   
   }
}

void readHolding(){
   instruccion=3;
   address=1;
   regl=0;
   regh=0;
   Envio_Esclavos();
   delay_ms(200);
   Lectura_Esclavos(); 
}


void writeCommand(int8 inst,int8 direccion,int16 data){
   instruccion=inst;
   address=direccion;
   regl=make8(data,0);
   regh=make8(data,1);
   Envio_Esclavos();
   Lectura_Esclavos();
}

char bgetc(void){
   char c;
   while(RX_Counter==0)
      ;
   c=Rx_Buffer2[RX_Rd_Index];
   if(++RX_Rd_Index>RX_BUFFER_SIZE)
      RX_Rd_Index=0;
   if(RX_Counter)
      RX_Counter--;
   return c;
}

void bputc(char c){
   char restart=0;
   while(TX_Counter> (TX_BUFFER_SIZE-1))
      ;
   if(TX_Counter==0)
      restart=1;
   TX_Buffer[TX_Wr_Index++]=c;
   if(TX_Wr_Index>TX_BUFFER_SIZE)
      TX_Wr_Index=0;
   
   TX_Counter++;
   
   if(restart==1)
      enable_interrupts(int_tbe);
}

void SendDataDisplay(void){
delay_us(10);
putc(0xFF);
delay_us(10);
putc(0xFF);
delay_us(10);
putc(0xFF);
delay_us(10);
}

short esBisiesto(int8 year) {
     return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}

void LimitaDia(){
   if(MesTx>12)
      MesTx=10;
   if(DiaTx>31)
      DiaTx=16;
   if(YearTx>99)
      YearTx=18;   
   if(HoraTx>24)
      HoraTx=11;      
   if(MinutoTx>59)
      MinutoTx=30;      
   
      if(MesTx==2){
         if(esBisiesto(YearTx)){
            if(DiaTx>29){
               DiaTx=29;
            }
         }else{
            if(DiaTx>28){
               DiaTx=28;
            }
         }
      }else{
         if(MesTx<=7){
            if(MesTx % 2 ==0){
               if(DiaTx>30){
                  DiaTx=30;                   
               }
            }
         }else{
            if(MesTx % 2 ==1){
               if(DiaTx>30){
                  DiaTx=30; 
               }
            }
         }
      }
}

void LeeDisplay(void){
   if(Dato_Exitoso==5){
         RX_Buffer[0]=RX_Buffer2[0];
         RX_Buffer[1]=RX_Buffer2[1];
         RX_Buffer[2]=RX_Buffer2[2];
         RX_Buffer[3]=RX_Buffer2[3];
         RX_Buffer[4]=RX_Buffer2[4];
         RX_Buffer[5]=RX_Buffer2[5];
         RX_Buffer[6]=RX_Buffer2[6];
         RX_Buffer[7]=RX_Buffer2[7];
         RX_Buffer[8]=RX_Buffer2[8];
         RX_Buffer[9]=RX_Buffer2[9];
         
      if(RX_Buffer[3]==0x01){// Pregunta por la pagina en la que esta el display,01 es Contrase?a de Acceso
         Menu=0;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x02){//02 es Menu de Tiempos
         Menu=1;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x03){//03 es Tiempo de Purga
         Menu=2;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x04){//04 es Tiempo de Post-Purga
         Menu=3;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x05){//05 es Tiempo de Purga Corriendo
         Menu=4;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x06){//06 es Pantalla Principal
         Menu=5;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x07){//07 es Tiempo de Post-Purga Corriendo
         Menu=6;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x08){//08 es Fecha y hora
         Menu=7;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x09){//09 es off
         Menu=8;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x0a){//0a es TiempoUV
         Menu=9;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x0f){//0f es Recibe caracteres de contrase?a desde display
         
      }else if(RX_Buffer[3]==0x1a){//1a es Mantenimiento
         Menu=10;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x1b){//1b es Menu de clave correcta
         Menu=15;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x1c){//1c es Menu de clave incorrecta
         Menu=16;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x1d){//1c es Menu de Silencio Alarmas
         Menu=17;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0x2c){//2c es Menu de Configuracion de Parametros
         Menu=100;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(RX_Buffer[3]==0xcc){//cc es Menu de Bienvenida
         Menu=240;
         if(MenuAntt!=240)
            MenuAntt=Menu;
            //reset_cpu();
      }
   }else{
      for(z=0;z<RX_BUFFER_SIZE;z++){
            //Rx_Buffer[z]=0;
            //Rx_Buffer2[z]=0;
         }
   }
}

Float LeerFlujo(int8 media,int8 canal){
    float dif=0.0;
    Flujo[canal].adc=sensores(canal);
    
    dif=zero_fabrica-Flujo[canal].zero_actual;
    x_uno=Flujo[canal].adc+dif;
    /*
    if(Flujo[canal].negativo==10){
      x_uno=Flujo[canal].adc+Flujo[canal].Diferencia;
    }
    if(Flujo[canal].negativo==20){
      x_uno=Flujo[canal].adc-Flujo[canal].Diferencia;
    }
    */
    //x_cuadrado=x_uno*x_uno;
    //x_cubo=x_uno*x_cuadrado;
    x_uno=(x_uno/1023)*5;
    //Flujo[canal].Velocidad=(x_cubo*a)+(x_cuadrado*b)+(x_uno*c)+d; 
    Flujo[canal].Velocidad=(pow(x_uno,4)*p1)+(pow(x_uno,3)*p2)+(pow(x_uno,2)*p3)+(x_uno*p4)+p5; 
    //Velocidad=Velocidad*correccion;//Ajuste de Temperatura
    //Flujo[canal].Velocidad=Flujo[canal].Velocidad-0.08;
    
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
     
     Flujo[canal].Flujo=Flujo[canal].Flujo/(densidad*0.00384846*3600);//0.0052810296=Area con tubo de 8.2cm
     return Flujo[canal].Flujo;
}

float D6FW101(int8 media,canal){
   static float a1=-0.054947,b1=0.56321,c1=-0.45502;
   float flow=0.0,promedio=0.0,adc=0.0,Qdn=0,flowV=0.0;//Areas en m2
   int8 pos=0;
   
   promedio=0.0;
   Qdn=0.0;
   adc=sensores(canal);
   adc=(adc/1023)*5.0;
   flow=(adc*adc*a1)+(adc*b1)+c1;
   flow=flow-0.04;
   if(flow<0.0)
      flow=0.0;
      
   //Qdn=Adn*flow;   
   //flowV=Qdn/AdnV;
   flowV=flow;
   
   if(pos>media-1)
      pos=0;
   
   prom[pos]=flowV;
   pos++;
   
   for(i=0;i<=(media-1);i++){
      promedio+=prom[h];
   } 
   promedio=promedio/media;
   return promedio*Flujo[canal].Ajuste;
   //return flowV;

}

float Leer_Sensor_Presion(int8 media,int8 canal){
   Presion[canal].promediopresion=0.0;
   
   Presion[canal].adc=sensores(canal);
   Presion[canal].V1=Presion[canal].adc; 
   Presion[canal].Presion=(Presion[canal].V1/Presion[canal].G)-1.0;// Presion=(Voltaje/Ganancia)-1
   //V1 = (x_uno*5.0)/1023.0;   //Lectura de Divisor de Voltaje de PT100 con resistencia de 1k (+-10%)
   //if(canal==4){
   //   Presion[canal].Presion=(Presion[canal].V1/Gdn)-1.0;// Presion=(Voltaje/Ganancia)-1
   //}else{
   //}
 
   if(canal==4){//Ajuste para sensor MPXV5004GV
      if(Presion[canal].Presion>0)
         Presion[canal].Presion=0.0;
    
      if(Presion[canal].Presion<0.0){
         Presion[canal].Presion=Presion[canal].Presion*-1;
      }
   }else{
      if(Presion[canal].Presion<0.0){
         Presion[canal].Presion=0.0;
      } 
   }

   if(Presion[canal].r>media-1)
   {Presion[canal].r=0;}
   Presion[canal].PromPresion[Presion[canal].r]=Presion[canal].Presion*4.02;Presion[canal].r++;
         
   for(q=0;q<=(media-1);q++)
   {
      Presion[canal].promediopresion+=Presion[canal].PromPresion[q];
   } 
   Presion[canal].promediopresion=Presion[canal].promediopresion/media;   
   
   //return Presion[canal].promediopresion;
   return Presion[canal].promediopresion*Presion[canal].AjustePresion;
   
   //if(canal==4){//return Presion[canal].promediopresion*Presion[canal].Ajuste;
   //   return Presion[canal].promediopresion*Ajustedn;
   //}else{
      
   //}
}

float leeTemperatura(int8 media){
   float ADC=0,V=0,T=0,Tempera=0;
   ADC=sensores(0);
   V=5*ADC/1023;
   T=V/0.01;
   if(l2>media)
      l2=0;
      
   promedio2[l2]=T;l2++;
   Tempera=0;
     
     for(h2=0;h2<=(media-1);h2++){
        Tempera+=promedio2[h2];
     }      
   return Tempera/media;
}

void LeeEEPROM(void){
   Password[0]=read_eeprom(0);
   delay_ms(10);
   Password[1]=read_eeprom(1);
   delay_ms(10);
   Password[2]=read_eeprom(2);
   delay_ms(10);
   Password[3]=read_eeprom(3);
   delay_ms(10);
   mPurgap=read_eeprom(4);
   delay_ms(10);
   sPurgap=read_eeprom(5);
   delay_ms(10);
   mPPurgap=read_eeprom(6);
   delay_ms(10);
   sPPurgap=read_eeprom(7);
   delay_ms(10);
   mUVp=read_eeprom(8);
   delay_ms(10);
   sUVp=read_eeprom(9);
   delay_ms(10);
   
   Tuv[0]=read_eeprom(12);
   delay_ms(10);
   Tuv[1]=read_eeprom(13);
   delay_ms(10);
   Tuv[2]=read_eeprom(14);
   delay_ms(10);
   Tuv[3]=read_eeprom(15);
   delay_ms(10);
   Ttrabajo[0]=read_eeprom(16);
   delay_ms(10);
   Ttrabajo[1]=read_eeprom(17);
   delay_ms(10);
   Ttrabajo[2]=read_eeprom(18);
   delay_ms(10);
   Ttrabajo[3]=read_eeprom(19);
   delay_ms(10);
   //flag_alarma=read_eeprom(20);
   //delay_ms(10);
   /*
   delay_ms(10);
   Entero=read_eeprom(25);
   delay_ms(10);
   Temporal=read_eeprom(26);
   delay_ms(10);
   Temporal=Temporal/100.0;
   Flujo[1].Ajuste=Entero+Temporal;
   
   
   Entero=read_eeprom(27);
   delay_ms(10);
   Temporal=read_eeprom(28);
   delay_ms(10);
   Temporal=Temporal/100.0;
   Flujo[2].Ajuste=Entero+Temporal;
   */
   delay_ms(10);
   velocidadInicial=read_eeprom(25);//Decimal1=read_eeprom(26);
   delay_ms(10);
   
   delay_ms(10);
   Entero=read_eeprom(29);//Decimal1=read_eeprom(26);
   delay_ms(10);
   Temporal=read_eeprom(30);
   delay_ms(10);
   Temporal=Temporal/10.0;
   Presion[4].Ajuste=Entero+Temporal;
   
   Entero=read_eeprom(31);//Decimal1=read_eeprom(26);
   delay_ms(10);
   Temporal=read_eeprom(32);
   delay_ms(10);
   Temporal=Temporal/10.0;
   Presion[3].Ajuste=Entero+Temporal;
   
   Flujo[1].zero_actual=make16(read_eeprom(34),read_eeprom(33));
   delay_ms(10);
   Flujo[2].zero_actual=make16(read_eeprom(36),read_eeprom(35));
   delay_ms(10);
   
   minutos_trabajo=make16(read_eeprom(38),read_eeprom(37));
   delay_ms(10);
   minutos_uv=read_eeprom(39);
   delay_ms(10);
   Vel1=read_eeprom(40);
   delay_ms(10);
   Vel2=read_eeprom(41);
   delay_ms(10);
   
   Flujo[1].Diferencia=make16(read_eeprom(43),read_eeprom(42));
   delay_ms(10);
   Flujo[2].Diferencia=make16(read_eeprom(45),read_eeprom(44));
   delay_ms(10);
   Presion[4].G=make16(read_eeprom(47),read_eeprom(46));
   //Gdn=make16(read_eeprom(47),read_eeprom(46));
   delay_ms(10);
   Presion[3].G=make16(read_eeprom(49),read_eeprom(48));
   delay_ms(10);
   Flujo[1].negativo=read_eeprom(50);
   delay_ms(10);
   Flujo[2].negativo=read_eeprom(51);
   delay_ms(10);

   Entero=read_eeprom(52);
   delay_ms(10);
   Temporal=read_eeprom(53);
   delay_ms(10);
   Temporal=Temporal/100.0;
   Relacion=Entero+Temporal;
   
   Entero=read_eeprom(54);
   delay_ms(10);
   Temporal=read_eeprom(55);
   delay_ms(10);
   Temporal=Temporal/100.0;
   pInDn=Entero+Temporal;
   
   Entero=read_eeprom(56);
   delay_ms(10);
   Temporal=read_eeprom(57);
   delay_ms(10);
   Temporal=Temporal/100.0;
   pInIn=Entero+Temporal;
   
   Pr=read_eeprom(58);
   delay_ms(10);
   HR=read_eeprom(59);
   delay_ms(10);
   
   mSilenciop=read_eeprom(60);
   delay_ms(10);
   sSilenciop=read_eeprom(61);
   delay_ms(10);
   
   delay_ms(10);
   Entero=read_eeprom(62);//Decimal1=read_eeprom(26);
   delay_ms(10);
   Temporal=read_eeprom(63);
   delay_ms(10);
   Temporal=Temporal/10.0;
   Presion[4].AjustePresion=Entero+Temporal;
}

void LimitaValores(void){
   if(Password[0]>9 || Password[0]<0){
      Password[0]=0;write_eeprom(0,0);
   }
   if(Password[1]>9 || Password[1]<0){
      Password[1]=0;write_eeprom(1,0);
   }
   if(Password[2]>9 || Password[2]<0){
      Password[2]=0;write_eeprom(2,0);
   }
   if(Password[3]>9 || Password[3]<0){
      Password[3]=0;write_eeprom(3,0);
   }
   if(mPurgap>99){
      mPurgaP=1;write_eeprom(4,1);
   }
   if(sPurgap>59){
      sPurgaP=0;write_eeprom(5,0);
   }
   if(mPPurgap>99){
      mPPurgaP=1;write_eeprom(6,1);
   }
   if(sPPurgap>59){
      sPPurgaP=0;write_eeprom(7,0);
   }
   if(mUVp>99){
      mUVP=1;write_eeprom(8,1);
   }
   if(sUVp>59){
      sUVP=0;write_eeprom(9,0);
   }
   if(Tuv[0]>9 || Tuv[0]<0){
      Tuv[0]=0;write_eeprom(12,0);
   }
   if(Tuv[1]>9 || Tuv[1]<0){
      Tuv[1]=0;write_eeprom(13,0);
   }
   if(Tuv[2]>9 || Tuv[2]<0){
      Tuv[2]=0;write_eeprom(14,0);
   }
   if(Tuv[3]>9 || Tuv[3]<0){
      Tuv[3]=0;write_eeprom(15,0);
   }
   if(Ttrabajo[0]>9 || Ttrabajo[0]<0){
      Ttrabajo[0]=0;write_eeprom(16,0);
   }
   if(Ttrabajo[1]>9 || Ttrabajo[1]<0){
      Ttrabajo[1]=0;write_eeprom(17,0);
   }
   if(Ttrabajo[2]>9 || Ttrabajo[2]<0){
      Ttrabajo[2]=0;write_eeprom(18,0);
   }
   if(Ttrabajo[3]>9 || Ttrabajo[3]<0){
      Ttrabajo[3]=0;write_eeprom(19,0);
   }
   /*
   if(Flujo[1].Ajuste>10.0 || Flujo[1].Ajuste==0.0){
      Flujo[1].Ajuste=1.0;write_eeprom(25,1);write_eeprom(26,0);
   }
   if(Flujo[2].Ajuste>10.0 || Flujo[2].Ajuste==0.0){
      Flujo[2].Ajuste=1.;write_eeprom(27,1);write_eeprom(28,0);
   }
   */
   if(velocidadInicial>95 || velocidadInicial<20){
      velocidadInicial=50;write_eeprom(25,50);
   }
   if(Presion[3].Ajuste>3.0 || Presion[3].Ajuste==0.0){
      Presion[3].Ajuste=1.0;write_eeprom(31,1);write_eeprom(32,0);
   }
   if(Presion[4].Ajuste>2.0 || Presion[4].Ajuste==0.0){
      Presion[4].Ajuste=1.0;write_eeprom(29,1);write_eeprom(30,0);
   }
   if(Flujo[1].zero_actual>280 ||Flujo[1].zero_actual<180 ){
      Flujo[1].zero_actual=203;write_eeprom(33,203);write_eeprom(34,0);
   }
   if(Flujo[2].zero_actual>280 ||Flujo[2].zero_actual<180){
      Flujo[2].zero_actual=203;write_eeprom(35,203);write_eeprom(36,0);
   }
   if(minutos_trabajo>3600){
      minutos_trabajo=0;write_eeprom(37,0);write_eeprom(38,0);
   }
   if(minutos_uv>60){
      minutos_uv=0;write_eeprom(39,0);
   }
   if(Vel1>100 || Vel1<20){
      Vel1=50;write_eeprom(40,50);
   }   
   if(Vel2>100 || Vel2<20){
      Vel2=50;write_eeprom(41,50);
   }   
   
   if(Flujo[1].Diferencia>50){
      Flujo[1].Diferencia=0;write_eeprom(42,0);write_eeprom(43,0);
   }
   if(Flujo[2].Diferencia>50){
      Flujo[2].Diferencia=0;write_eeprom(44,0);write_eeprom(45,0);
   }
   if(Presion[4].G>300 || Presion[4].G<150){
      Presion[4].G=250;write_eeprom(46,250);write_eeprom(47,0);
   }
   if(Presion[3].G>300 || Presion[3].G<150){
      Presion[3].G=250;write_eeprom(48,250);write_eeprom(49,0);
   }
   if(Flujo[1].negativo>20){
      Flujo[1].negativo=10;write_eeprom(50,10);
   }
   if(Flujo[2].negativo>20){
      Flujo[2].negativo=10;write_eeprom(51,10);
   }
   if(pInDn>0.6 || pInDn<0.1){
      pInDn=0.3;write_eeprom(54,0);write_eeprom(55,1);
   }
   if(pInIn>4.0 || pInIn<0.1){
      pInDn=1.0;write_eeprom(56,1);write_eeprom(57,0);
   }
   if(Pr>110 || Pr<60){
      Pr=71;write_eeprom(58,71);
   }
   if(HR>100 || HR<20){
      HR=60;write_eeprom(59,60);
   }
   if(mSilenciop>99){
      mSilenciop=1;write_eeprom(60,1);
   }
   if(sSilenciop>59){
      sSilenciop=0;write_eeprom(61,0);
   }
   if(Presion[4].AjustePresion>4.0 || Presion[4].AjustePresion==0.0){
      Presion[4].AjustePresion=1.0;write_eeprom(62,1);write_eeprom(63,0);
   }
   Pa=Pr*1000;
}

void ApagaSalidas(){
   Alarma_off;
   LuzBlanca_off;
   LuzUV_off;
   Aux_off;
   Toma_off;
   Motor_off;
   Motor2_off;
}

void dibujaBarraDn(){
   if(barraDn>=0 && barraDn<=25){
         printf("j0.val=%u",(int8)((barraDn*100)/25));
         SendDataDisplay();
         printf("j1.val=0");
         SendDataDisplay();
         printf("j2.val=0");
         SendDataDisplay();
         printf("j3.val=0");
         SendDataDisplay();
      }else if(barraDn>25 && barraDn<=50){
         printf("j1.val=%u",(int8)(((barraDn*100)/25)-100));
         SendDataDisplay();
         printf("j0.val=100");
         SendDataDisplay();
         printf("j2.val=0");
         SendDataDisplay();
         printf("j3.val=0");
         SendDataDisplay();
      }else if(barraDn>50 && barraDn<=75){
         printf("j2.val=%u",(int8)(((barraDn*100)/25)-200));
         SendDataDisplay();
         printf("j0.val=100");
         SendDataDisplay();
         printf("j1.val=100");
         SendDataDisplay();
         printf("j3.val=0");
         SendDataDisplay();
      }else if(barraDn>75 && barraDn<=100){
         printf("j3.val=%u",(int8)(((barraDn*100)/25)-300));
         SendDataDisplay();
         printf("j0.val=100");
         SendDataDisplay();
         printf("j1.val=100");
         SendDataDisplay();
         printf("j2.val=100");
         SendDataDisplay();
      }
}

void dibujaBarraIn(){
   if(barraIn>=0 && barraIn<=25){
         printf("j4.val=%u",(int8)((barraIn*100)/25));
         SendDataDisplay();
         printf("j5.val=0");
         SendDataDisplay();
         printf("j6.val=0");
         SendDataDisplay();
         printf("j7.val=0");
         SendDataDisplay();
      }else if(barraIn>25 && barraIn<=50){
         printf("j5.val=%u",(int8)(((barraIn*100)/25)-100));
         SendDataDisplay();
         printf("j4.val=100");
         SendDataDisplay();
         printf("j6.val=0");
         SendDataDisplay();
         printf("j7.val=0");
         SendDataDisplay();
      }else if(barraIn>50 && barraIn<=75){
         printf("j6.val=%u",(int8)(((barraIn*100)/25)-200));
         SendDataDisplay();
         printf("j4.val=100");
         SendDataDisplay();
         printf("j5.val=100");
         SendDataDisplay();
         printf("j7.val=0");
         SendDataDisplay();
      }else if(barraIn>75 && barraIn<=100){
         printf("j7.val=%u",(int8)(((barraIn*100)/25)-300));
         SendDataDisplay();
         printf("j4.val=100");
         SendDataDisplay();
         printf("j5.val=100");
         SendDataDisplay();
         printf("j6.val=100");
         SendDataDisplay();
      }
}  

void cambiaColor(int16 col){ //64512 Naranja, Rojo 59620
   printf("fecha.bco=%Lu",col);
   SendDataDisplay(); 
   printf("estado.bco=%Lu",col);
   SendDataDisplay(); 
   printf("fex.bco=%Lu",col);
   SendDataDisplay(); 
   printf("fdown.bco=%Lu",col);
   SendDataDisplay(); 
   printf("tventana.bco=%Lu",col);
   SendDataDisplay(); 
   printf("tfiltro.bco=%Lu",col);
   SendDataDisplay(); 
   printf("tf2.bco=%Lu",col);
   SendDataDisplay(); 
}

void calculaDensidad(){
   Temp=Temperatura-4;
   TempK=Temp+273.15;
   Pa=Pr*1000;
   
   Psv=exp((0.000012378847*pow(TempK,2))+(-0.019121316*TempK)+33.93711047+(-6343.1645/TempK));
   f=1.00062+(0.0000000314*Pa)+(0.00000056*Temp);
   Xv=(HR/100)*f*(Psv/Pa);
   Z1=1-((Pa/TempK)*(0.00000158123+(-0.000000029331*Temp)+(0.00000000011043*pow(Temp,2))+((0.000005707+(-0.00000002051*Temp))*Xv)+((0.0001989+(-0.000002376*Temp)))*pow(Xv,2)))+((pow(Pa,2)/pow(TempK,2))*(0.0000000000183+(-0.00000000765*pow(Xv,2))));
   densidad=((Pa*0.028963512440)/(Z1*TempK*8.314472))*(1-(Xv*(1-(0.018015/0.028963512440))));
}

void LeerBotones(){
   if(tup>=30){
      UP=ON;            
   }
   
   if(tdown>=30){
      DOWN=ON;
   }
   
   if(tright>=30){
      RIGHT=ON;
   }
   
   if(tleft>=30){
      LEFT=ON;
   }
}

void VelocidadMotor(int8 Vel){
   float tmp;
   if(Vel==0){
      #ifdef RS485
      Motor_off;
      Motor2_off;
      readHolding(); 
      if(make16(txbuf[1],txbuf[2])!=0 || reg!=make16(txbuf[3],txbuf[4])){//Compara Registro 1 con el valor de activacion de variador
         writeCommand(5,0,0);//Envia comando 5 a direccion 1 con dato 0         
         conteoError++;
         flagError=ON;
         if(conteoError>15)
            Alarmas=ON;      
      }else{
         if(flagError){
            flagError=OFF;
            conteoError=0;
            Alarmas=OFF;
            writeCommand(0,0,0);
         }
      }
      
      #else
      Motor_off;
      Motor2_off;
      Alarmas=OFF;
      flagError=OFF;
      conteoError=0;
      #endif
   }else{
      #ifdef RS485
      Motor_on;
      tmp=((float)Vel/(1.6666666))*10.0;
      readHolding(); 
      reg=(int16)tmp;
      //if(make16(txbuf[1],txbuf[2])!=1 || reg!=make16(txbuf[3],txbuf[4])){//Compara Registro 1 con el valor de activacion de variador
      if(make16(txbuf[1],txbuf[2])!=1 || reg!=make16(txbuf[3],txbuf[4])){//Compara Registro 1 con el valor de activacion de variador
         //writeCommand(5,1,reg);
         //delay_ms(10);
         writeCommand(5,0,1); 
         conteoError++;
         flagError=ON;
         if(conteoError>15)
            Alarmas=ON;      
         
         if(reg!=make16(txbuf[3],txbuf[4])){
            writeCommand(5,1,reg);
            delay_ms(10); 
            writeCommand(5,1,reg);
            delay_ms(10); 
            writeCommand(5,1,reg);
            delay_ms(10); 
         }
      }else{
         Alarma_off;
         Aux_off;
         if(flagError){
            flagError=OFF;
            conteoError=0;
            Alarmas=OFF;
            writeCommand(0,0,0);
         }
      }
      
      #else
         Motor_on;
         Motor2_on;
         tmp=((float)Vel*1023.0)/100.0;
         pwm1=1023-((int16)tmp);
         pwm2=pwm1;
         pwm1l=make8(pwm1,0);
         pwm1h=make8(pwm1,1);
         pwm2l=make8(pwm2,0);
         pwm2h=make8(pwm2,1);
         Envio_Esclavos();
      #endif
   }
}

void guardaHora(){
            UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
            PantallaPrincipal=0;
            
            if(esBisiesto(YearTx))
               Modulo=Bisiesto[MesTx];
            else
               Modulo=Regular[MesTx];
         
            dowTx=((YearTx-1)%7+((YearTx-1)/4-3*((YearTx-1)/100+1)/4)%7+Modulo+DiaTx%7)%7;
            rtc_set_datetime(DiaTx,MesTx,YearTx,dowTx,HoraTx,MinutoTx);
}

void leeTeclasLaterales(){
      if(RX_Buffer[4]==0x10){//Selecciono Principal
         silencio=OFF;
         if(Menu==7)
            guardaHora();
         printf("page Principal");
         SendDataDisplay();
         UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         tiempo_uv=0;LuzUV=10;flag_uv=0;
         sUV=0;mUV=0;UV2=0;
         flag_luz=0;
         CambioColorR=OFF;
         CambioColorN=OFF;
         CambioColorG=OFF;
      }
      
      if(RX_Buffer[4]==0x20){//Selecciono Mantenimiento
         silencio=OFF;
         if(Menu==7)
            guardaHora();
         printf("page Mantenimiento");
         SendDataDisplay();  
         UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         tiempo_uv=0;LuzUV=10;flag_uv=0;
         sUV=0;mUV=0;UV2=0;
         flag_luz=0;
         CambioColorR=OFF;
         CambioColorN=OFF;
         CambioColorG=OFF;
      }
      
      if(RX_Buffer[4]==0x30){//Selecciono Clave
         silencio=OFF;
         if(Menu==7)
            guardaHora();
         printf("page Clave");
         SendDataDisplay();            
         TipoClave=2;
         UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         tiempo_uv=0;LuzUV=10;flag_uv=0;
         sUV=0;mUV=0;UV2=0;
         flag_luz=0;
         CambioColorR=OFF;
         CambioColorN=OFF;
         CambioColorG=OFF;
      }
      
      if(RX_Buffer[4]==0x40){//Selecciono Mantenimiento
         silencio=OFF;
         if(Menu==7)
            guardaHora();
         printf("page Tiempos");
         SendDataDisplay();  
         UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         tiempo_uv=0;LuzUV=10;flag_uv=0;
         sUV=0;mUV=0;UV2=0;
         flag_luz=0;
         CambioColorR=OFF;
         CambioColorN=OFF;
         CambioColorG=OFF;
      }
      
      if(RX_Buffer[4]==0x50){//Selecciono Hora y Fecha
         silencio=OFF;
         if(Menu==7)
            guardaHora();
         printf("page Hora");
         SendDataDisplay();
         DiaTx=Dia;
         MesTx=Mes;
         YearTx=Year;
         HoraTx=Hora;
         MinutoTx=Minuto;
         UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         tiempo_uv=0;LuzUV=10;flag_uv=0;
         sUV=0;mUV=0;UV2=0;
         flag_luz=0;
         CambioColorR=OFF;
         CambioColorN=OFF;
         CambioColorG=OFF;
      }
      
      if(RX_Buffer[4]==0x60){//Selecciono Servicio
         silencio=OFF;
         printf("page Clave");
         SendDataDisplay();            
         TipoClave=1;
         UP=OFF;DOWN=OFF;RIGHT=OFF;LEFT=OFF;
         tiempo_uv=0;LuzUV=10;flag_uv=0;
         sUV=0;mUV=0;UV2=0;
         flag_luz=0;
         CambioColorR=OFF;
         CambioColorN=OFF;
         CambioColorG=OFF;
      }
}

void leeEstados(){
      if(flag_luz==1){
            printf("b0.pic=53");
            SendDataDisplay();    
         }else{
            printf("b0.pic=52");
            SendDataDisplay(); 
         }
      
      if(flag_uv){
            printf("b3.pic=57");
            SendDataDisplay();              
         }else{            
            printf("b3.pic=56");
            SendDataDisplay(); 
         }
         
      if(flag_blower==1){
         printf("b2.pic=59");
         SendDataDisplay(); 
         tiempo_trabajo=1;
         if(guardatrabajo==1){
            guardatrabajo=0;
            write_eeprom(37,make8(minutos_trabajo,0));
            delay_ms(20);
            write_eeprom(38,make8(minutos_trabajo,1));
            delay_ms(20);
            write_eeprom(16,Ttrabajo[0]);
            delay_ms(20);
            write_eeprom(17,Ttrabajo[1]);
            delay_ms(20);
            write_eeprom(18,Ttrabajo[2]);
            delay_ms(20);
            write_eeprom(19,Ttrabajo[3]);
            delay_ms(20);            
         }
         VelocidadMotor(Vel1);
      }else{
         //Filtro_Downflow=0;
         printf("b2.pic=58");
         SendDataDisplay(); 
         tiempo_trabajo=0;
         VelocidadMotor(0);
         if(guardatrabajo==0){
            guardatrabajo=1;
            write_eeprom(37,make8(minutos_trabajo,0));
            delay_ms(20);
            write_eeprom(38,make8(minutos_trabajo,1));
            delay_ms(20);
            write_eeprom(16,Ttrabajo[0]);
            delay_ms(20);
            write_eeprom(17,Ttrabajo[1]);
            delay_ms(20);
            write_eeprom(18,Ttrabajo[2]);
            delay_ms(20);
            write_eeprom(19,Ttrabajo[3]);
            delay_ms(20);
         }
      }
      
      if(flag_toma==1){ // Si activo Toma
         printf("b1.pic=55");
         SendDataDisplay(); 
         Toma_on;
      }else{// Si desactivo Toma        
         printf("b1.pic=54");
         SendDataDisplay(); 
         Toma_off;
      }
}
