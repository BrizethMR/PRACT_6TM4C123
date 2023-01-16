
#include "lib/include.h"

extern void Configura_Reg_PWM1(uint16_t freq)
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
   

    /*SE ESTA TRABAJANDO CON EL PB4 (PWM2), PB7(PWM1), PE4(PWM4) Y MODULO 0 */
    /*SE HABILITA MODULO PWM0 P. 354*/
    SYSCTL->RCGCPWM |= (1<<0); 

    /*HABILITA GPIO PUERTO B Y ePORQUE SE TRABAJA COON GEN. 1 MODULO 0, P. 340 Y 1233 PARA SABER QUE PUERTOS/PINEA CORRESPONDE A CADA PWM*/
    SYSCTL->RCGCGPIO |= (1<<1)|(1<<4); /*Enable reloj de GPIO Puerto F pag 340 pin 5*/

    /*ENABLE O DISABLE DIVIRSOR P. 254*/ 
    SYSCTL->RCC &= ~(1<<20)|(0<<19)|(1<<18)|(0<<17);  /*PORQUE SALEN 5MIL CUENTAS QUE CABEN EN 16 BITS DEL CONTADOR DEL PWM, 20 MILLONES/50 = 5 MIL CUENTAS*/
    /*EL 20 INDICA SI SE DIVIDE O NO, EN ESTE CASO SE ESTA DIVIDIENDO P. 254*/

   // GPIOF->AFSEL |= (1<<3)|(1<<2)|(1<<1); /*Control de registros ya sea por GPIO o Otros Pag 672*/
   // HABILLITAR LA FUNCIÓN ALTERNATIVA
    GPIOB->AFSEL |= (1<<4); //PB4
    GPIOB->AFSEL |= (1<<7); //PB7
    GPIOE->AFSEL |= (1<<4); //PE4 

    /*SE PONE UN 4 EN EL LUGAR 4 POR EL AFSEL Y LAS TABLAS DE LA 1351 Y 689*/
    GPIOB->PCTL |= (GPIOB->PCTL&0xFFF0FFFF) | 0x00040000; //PB4
    GPIOB->PCTL |= (GPIOB->PCTL&0x0FFFFFFF) | 0x40000000; //PB7
    GPIOE->PCTL |= (GPIOB->PCTL&0xFFF0FFFF) | 0x00040000; //PE4

    /*HABILITAR LA FUNCIÓN LA ALTERNATIVA - para decirle si es digital o no Pag 682 - solo el 4*/
    GPIOB->DEN |= (1<<4); //PB4
    GPIOB->DEN |= (1<<7); //PB7
    GPIOE->DEN |= (1<<4); //PE4

    /*P. 1266 el bit 0 indica el modo habilitado o deshabilitado */
    /*EL NUMERO DE AQUI INDICA GENERADOR*/
    PWM0->_1_CTL = (0<<0);/*Bloqueo y desbloqueo, SE DESACTIVA EL GENERADOR 1, LO DESABILITA PARA CONFIGURARLO */ 
    PWM0->_0_CTL = (0<<0);
    PWM0->_2_CTL = (0<<0);

    /*MODO DE TRABAJO - CUENTA REGRSIVA P. 1285 Y 1239*/
    //PWM0->_1_GENB = 0x0000080C; /*Registro de las acciones del pwm Pag 1285*/
    PWM0->_0_GENB = 0x0000080C; ///EL GENERADOR 0 TRABAJA CON EL PWM1 COMPARADOR B
    PWM0->_1_GENA = 0x0000008C; // EL GENERADOR 1 TRABAJA CON EL PWM2 Y CON EL COMPARADOR A
    PWM0->_2_GENA = 0x0000008C; // EL GENERADOR 2 TRABAJA CON EL PWM4 Y CON EL COMPARADOR A 

    PWM0->_0_LOAD = 50000; /*cuentas = fclk/fpwm  para 1khz cuentas = (20,000,000/50)*/
    PWM0->_1_LOAD = 50000;
    PWM0->_2_LOAD = 50000;

    //PWM0->_1_CMPB = 3750; /*25% DE LAS CUENTA DE TRABAJO */
    PWM0->_0_CMPB = 37500;
    PWM0->_1_CMPA = 37500;
    PWM0->_2_CMPA = 37500;

    PWM0->_0_CTL = (1<<0);
    PWM0->_1_CTL = (1<<0);// Se activa el generador 1 
    PWM0->_2_CTL = (1<<0);

    PWM0->ENABLE = (1<<2)|(1<<4)|(1<<1); /*habilitar el bloque pa que pase Pag 1247*/
}
