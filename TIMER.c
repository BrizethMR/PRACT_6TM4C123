/*
 * TIMER.c
 *
 *  Created on: 4 dic. 2020
 *      Author: root
 */
#include "lib/include.h"

/*
    Configurar el timer 0 y timer 1 en modo de 32bits para hacer la siguiente rutina:
    una función que active  el convertidor analógico digital y muestre dos señales de 
    cierta frecuencia y amplitud definida.
    y las guarde en dos arreglos  de tamaño 1024 y la envié por el protocolo de 
    comunicación asíncrono para ser procesadas y regresadas al microcontrolador en 
    valores binarios las cuales modificaran el ancho de pulso y reconstruirán la señal 
    enviada a un transistor u opam.
    b) 2khz , -5 a 5
    c)10khz, -1 a 1
    */


void (*PeriodicTask)(void); // periodictask es un puntero para una función que no recibe ni retorna nada
extern void Configurar_Timer0A(void(*task)(void), unsigned long int periodo)
{
    SYSCTL->RCGCTIMER |= (1<<0); //Encender el Periferico Timer 0 pag 338

    PeriodicTask = task; // asignar la direccion de memoria de la tarea a temporizar 

    TIMER0->CTL = (0<<0);  // desabilitar el temporizador a usar antes de hacer cambios para poder configurar p. 722

    TIMER0->CFG = 0x00000000; // modo de operacion 16 o 32 bits - p. 728
    // seleciona 32 bits con 0x0

    TIMER0->TAMR = (0x2<<0); // modo del temporizador - modo periodico pag 732
    // AQUI SE SELECCIONO EL TIMER A POR ESO ES TaMR

    TIMER0->TAILR = periodo - 1; //para establecer la carga - cuantas cuentas queremos - el periodo viene del main (10,000) 
    // y recibiria informacion durante 500 ms - esa conversión la tiene rox p.756 para TAILR
    // le quita 1 para que se cuente hasta el 0 **********

    TIMER0->ICR = 1<<0; // limpiar el registro de la interrupción - aqui se limpia el timer0 p.754

    TIMER0->IMR = 1<<0; // se habilita la mascara del timer0 - p. 745
    // segun la p. 724 se habilita si se hace uso de interrupciones 

    NVIC->IP[4] = (NVIC->IP[4]&0x00FFFFFF) | 0x20000000; //interrupcion por prioridad 
                        // 4n + 3

    NVIC->ISER[0] = (1UL << 19); //pag 147
    // numero de interrupcion TIMER0A = 19  
    // n=19 ----> [4n+3] [4n+2] [4n+1] [4n] ---> [4n+3]
    TIMER0->CTL = (1<<0);
}

extern void Timer0A_Handler(void)
{
  TIMER0->ICR = 1<<0;// acknowledge TIMER0A timeout
  (*PeriodicTask)();           
}


