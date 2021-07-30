%Calculos caida de presion en ventiladores CBB2
clc,clear
%Curva Ventilador Chino
yy=[0    2.05 2.89 3.33 3.86 4.55 4.81 5.0 ];
xx=[1700 1561 1375 1189 929  594  446  0 ];
xx=xx.*2;

p1=polyfit(xx,yy,1);    %aproximaciòn por polinomio de segundo orden
p2=polyfit(xx,yy,2);    %aproximaciòn por polinomio de tercer orden
p3=polyfit(xx,yy,3);    %aproximaciòn por polinomio de tercer orden
p33=polyfit(xx,yy,4);    %aproximaciòn por polinomio de tercer orden
p5=polyfit(xx,yy,5);    %aproximaciòn por polinomio de tercer orden
p6=polyfit(xx,yy,6);    %aproximaciòn por polinomio de tercer orden

[M,I]=max(xx);
Qexmincfm=535;%cfm
adc=Qexmincfm;

Largofiltro1=36*0.0254;%m
Anchofiltro1=24*0.0254;%m
Areafiltro1=Largofiltro1*Anchofiltro1;%m2
Areafiltro1=Areafiltro1*6;

Largofiltro2=24*0.0254;%m
Anchofiltro2=12*0.0254;%m
Areafiltro2=Largofiltro2*Anchofiltro2;%m2
Areafiltro2=Areafiltro2*3;

Largofiltro3=1*0.0254;%m
Anchofiltro3=1*0.0254;%m
Areafiltro3=Largofiltro3*Anchofiltro3;%m2
i=1;
Vin=3600;
Vfinal=3100;
for Qexmincfm=0:0.5:xx(I)
  Polinomio1(i)=(Qexmincfm*p1(1))+p1(2);
  Polinomio2(i)=(Qexmincfm^2*p2(1))+(Qexmincfm*p2(2))+p2(3);
  Polinomio3(i)=(Qexmincfm^3*p3(1))+(Qexmincfm^2*p3(2))+(Qexmincfm*p3(3))+p3(4);
  
  Polinomio33(i)=(Qexmincfm^4*p33(1))+(Qexmincfm^3*p33(2))+...
  (Qexmincfm^2*p33(3))+(Qexmincfm*p33(4))+p33(5);
  
  Polinomio5(i)=(Qexmincfm^5*p5(1))+(Qexmincfm^4*p5(2))+(Qexmincfm^3*p5(3))+...
  (Qexmincfm^2*p5(4))+(Qexmincfm*p5(5))+p5(6);
  
  Polinomio6(i)=(Qexmincfm^6*p6(1))+(Qexmincfm^5*p6(2))+(Qexmincfm^4*p6(3))+(Qexmincfm^3*p6(4))+...
  (Qexmincfm^2*p6(5))+(Qexmincfm*p6(6))+p6(7);
  
  Vmin1(i)=(Qexmincfm*1.69901082/(Areafiltro1+Areafiltro2))/3600;%m/s
  PresionFiltromin1(i)=(250*Vmin1(i)*0.00401865);%inWg --0.1 de caida de cabina
  PresionFiltromin1(i)=PresionFiltromin1(i)*1;
  PresionTotal(i)=PresionFiltromin1(i)+2;
  caudal(i)=i*0.5;
  
  if(abs(Polinomio33(i)-PresionTotal(i))<=0.01)
      CaudalFinal=Qexmincfm;
      PresionFinal=Polinomio33(i);
      VelocidadFinal=Vmin1(i);
      %break;
  end
  Q2(i)=Qexmincfm*(Vfinal/Vin);
  Pr2=Polinomio33.*((Vfinal/Vin)^2);
  Vmin2(i)=(Q2(i)*1.69901082/(Areafiltro1+Areafiltro2))/3600;%m/s
  if(abs(Pr2(i)-PresionTotal(i))<=0.01)
      CaudalFinal2=Q2(i);
      PresionFinal2=Pr2(i);
      VelocidadFinal2=Vmin2(i);
      %break;
  end
  i++;
end


subplot(1,3,1)
title('Curva Ventilador')
plot(xx,yy,'b',xx.*(Vfinal/Vin),yy.*((Vfinal/Vin)^2));
ylim([0 6])
title('Curva Ventilador CHINO X3');xlabel('Caudal (CFM)');ylabel('Presion (inWg)')
grid
subplot(1,3,2)
plot(caudal,Polinomio33,'b',caudal,PresionTotal,'g',Q2,Pr2,'r');
title('Q VS P Filtros HEPA (Orden 4)');xlabel('Caudal (CFM)');ylabel('Presion (inWg)')
legend('Caudal','Presion Filtros')
grid
subplot(1,3,3)
plot(caudal,Polinomio1,'r',caudal,Polinomio2,'b',caudal,Polinomio3,'k',caudal,Polinomio33,'c',...
caudal,Polinomio5,'--r',caudal,Polinomio6,'m:');
title('Curva Ventilador Polinomios');xlabel('Caudal (CFM)');ylabel('Presion (inWg)')
legend('Orden 1','Orden 2','Orden 3','Orden 4','Orden 5','Orden 6')
grid