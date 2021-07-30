
// Funcion para conversor analogo-digital
unsigned int16 sensores(int x){
unsigned int16 y;set_adc_channel(x);delay_ms(1);y=read_adc();return (y);
}
