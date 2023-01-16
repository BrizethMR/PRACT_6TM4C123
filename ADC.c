/*
 * ADC.c
 *
 *  Created on: 02/01/2023
 *      Author: AdrianaMtzR
 */
#include "lib/include.h"

extern void Configura_Reg_ADC0(void)
{   
/*
    Configurar el timer 0 y timer 1 en modo de 32bits para hacer la siguiente rutina:
    una función que active  el convertidor analógico digital y muestre dos señales de 
    cierta frecuencia y amplitud definida.
    y las guarde en dos arreglos  de tamaño 1024 y la envié por el protocolo de 
    comunicación asíncrono para ser procesadas y regresadas al microcontrolador en 
    valores binarios las cuales modificaran el ancho de pulso y reconstruirán la señal 
    enviada a un transistor u opam.
    c)10khz, -1 a 1
    */
   
     //Pag 352 para inicializar el modulo 0 de reloj del adc RCGCADC
    SYSCTL->RCGCADC = (1<<0); 

    //Pag 340 (RGCGPIO) Puertos base habilitación del reloj
    //                     F     E      D       C      B     A
    SYSCTL->RCGCGPIO |= (1<<5)|(1<<4)|(0<<3)|(0<<2)|(0<<1)|(1<<1);

    //Pag 663 (GPIODIR) Habilta los pines como I/O un cero para entrada y un uno para salida
    GPIOE_AHB->DIR = (0<<1) | (0<<2) | (0<<5); //PE5 PE1 y PE2* 

    //(GPIOAFSEL) pag.671 Enable alternate función para que el modulo analógico tenga control de esos pines
    GPIOE_AHB->AFSEL =  (1<<1) | (1<<2) | (1<<5);

    //(GPIODEN) pag.781 desabilita el modo digital
    GPIOE_AHB->DEN = (0<<1) | (0<<2)| (0<<5);

    //Pag 1351 GPIOPCTL registro combinado con el GPIOAFSEL y la tabla pag 1808 - establecer funcion alternativa con la mascara
    GPIOE_AHB->PCTL = GPIOE_AHB->PCTL & (0xFF0FF00F);

    //(GPIOAMSEL) pag.687 habilitar analogico
    GPIOE_AHB->AMSEL = (1<<1) | (1<<2) | (1<<5);

    //Pag 891 El registro (ADCPC) establece la velocidad de reloj de conversión por segundo
    ADC0->PC = (0<<2)|(1<<1)|(1<<0);//250ksps 

    //Pag 841 Este registro (ADCSSPRI) configura la prioridad de los secuenciadores
    ADC0->SSPRI = 0x3012; //usa secuenciador 2 que recibe 4 canales 
     
    //Pag 821 (ADCACTSS) Este registro controla la desactivación de los secuenciadores
    ADC0->ACTSS  =   (0<<3) | (0<<2) | (0<<1) | (0<<0);

    //Pag 834 Este registro (ADCEMUX) selecciona el evento que activa la conversión (trigger)
    ADC0->EMUX  = (0x0000); //por procesador 

    //Pag 867 Este registro (ADCSSMUX2) define las entradas analógicas con el canal y secuenciador seleccionado
    ADC0->SSMUX2 = 0x0821; 

    //pag 868 Este registro (ADCSSCTL2), configura el bit de control de muestreo y la interrupción 
    ADC0->SSCTL2 = (1<<1) | (1<<2) | (1<<5) | (1<<6) | (1<<10) | (1<<9); // son dos por cada canal y tenemos 3 canales 
    
    // indicar cuando manda la señal, mandar 1 al secuenciador que se ocupa (2)
    /* Enable ADC Interrupt */
    ADC0->IM |= (1<<2); /* Unmask ADC0 sequence 2 interrupt pag 825*/
    //NVIC_PRI4_R = (NVIC_PRI4_R & 0xFFFFFF00) | 0x00000020;
    //NVIC_EN0_R = 0x00010000;

    //Pag 821 (ADCACTSS) Este registro controla la activación de los secuenciadores
    ADC0->ACTSS = (0<<3) | (1<<2) | (0<<1) | (0<<0);
    ADC0->PSSI |= (1<<2);
}
extern void ADC0_InSeq2(uint16_t *Result, uint16_t *duty){ //recibe resultado y duty 
        //uint16_t Rojo;
    //ADC Processor Sample Sequence Initiate (ADCPSSI)
       ADC0->PSSI = 0x00000004; //habilita por procesador el 4 es commo porner un 1 en el 2 pero como hexadecimal 
       while((ADC0->RIS&0x04)==0){}; // espera al convertidor que termine
       Result[2] = ADC0->SSFIFO2&0xFFF; //  Leer  el resultado almacenado en la pila2 //FIFO 2 por secuenciador 2
       // FIFO para almacenar las muestras que se van a leer - p. 860
       Result[1] = ADC0->SSFIFO2&0xFFF; 
       Result[0] = ADC0->SSFIFO2&0xFFF; 
       Result[2] = ADC0->SSFIFO2&0xFFF;
       // es un resultado por cada canal - usamos 3 canales - resultado se cambia en main para ver cuantos canales se usan
       //Rojo = (uint16_t)readString('%');
       //printChar('A');
       
       duty[0] = 50000 - (Result[0]*5000)/4096;
       duty[1] = 50000 - (Result[1]*5000)/4096;
       duty[2] = 50000 - (Result[2]*5000)/4096;

       ADC0->ISC = 0x0004;  //Conversion finalizada

}




