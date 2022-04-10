/* 
 * File:   main.c
 * Author: Sergio Boch 20887
 *
 * Created on April 6, 2022, 5:33 PM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdint.h>  // Para poder usar los int de 8 bits

/*
 * 
 */

#define BOTON_INC PORTBbits.RB0 // Alias para el RB0
#define BOTON_DEC PORTBbits.RB1 // Alias para el RB1
#define _XTAL_FREQ 2000000 // Oscilador de 2MHz

//#define UNI PORTDbits.RD0  // Puerto D0 como unidades
//#define DEC PORTDbits.RD1  // Puerto D1 como decenas
//#define CEN PORTDbits.RD2  // Puerto D2 como centenas

int display;
char residuo = 0;
char residuo2 = 0;
char UNI = 0;
char DEC = 0;
char CEN = 0;
char numeros[10] = {0B00111111, 0B00000110, 0B01011011, 0B01001111, 
0B01100110, 0B01101101, 0B01111101, 0B00000111, 0B01111111, 0B01100111};


void __interrupt() isr(void){
    if(INTCONbits.RBIF){        // Interrupcion del puerto B
        if(!BOTON_INC)          // RB0 genera la interrupción
            PORTA++;            // Incremento en puerto A
        if(!BOTON_DEC)          // RB1 genera la interrupción
            PORTA--;            // Decremento en puerto A
        INTCONbits.RBIF = 0;    // Se limpia la bandera de interrupción]
    }
    if(INTCONbits.TMR0IF){
        switch(display){
            case 1:
                display = 0;
                PORTD = 0b0000100;
                PORTC = numeros[UNI];
                break;
            case 2:
                display = 1;
                PORTD = 0b000000010;
                PORTC = numeros[DEC];
                break;
            case 0:
                display = 2;
                PORTD = 0b00000001;
                PORTC = numeros[CEN];
                break;
        }
        INTCONbits.TMR0IF = 0;
                TMR0 = 246;
    }
    return;
}

void setup(void);
void main(void){
    setup();
    while(1){
        //while(PORTA<256){
            CEN = PORTA/100;
            residuo = PORTA%100;
            DEC = residuo/10;
            UNI = residuo%10;
    }
}

void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISA = 0;      // Puerto A como salida
    PORTA = 0;      // Se limpia el puerto A
    
    TRISC = 0;
    PORTC = 0;
    
    TRISD = 0;
    PORTD = 0;
    
    
    //TRISB = 1;      // Puerto B como entrada
    //PORTB = 0;      // Se limpia el puerto B
    TRISBbits.TRISB0 = 1;   // Se habilita el puerto B0 como entrada
    TRISBbits.TRISB1 = 1;   // Se habilita el puerto B1 como entrada]
    OPTION_REGbits.nRBPU = 0;   // Se habilitan las resistencias pull-up del puerto B
    WPUBbits.WPUB0 = 1;     // Se habilitan las resistencias de pull-up RB0
    WPUBbits.WPUB1 = 1;     // Se habilitan las resistencias de pull-up RB1
    INTCONbits.GIE = 1;     // Se habilitan las interrupciones globales
    INTCONbits.RBIE = 1;    // Se habilitan las interrupciones del puerto B
    IOCBbits.IOCB0 = 1;   // Se habilitan las interrupciones por cambio de estado para el puerto RB0 
    IOCBbits.IOCB1 = 1;   // Se habilitan las interrupciones por cambio de estado para el puerto RB1
    INTCONbits.RBIF = 0;    // Se limpian las banderas de interrupción
    
    //Timer0 Registers Prescaler= 256 - TMR0 Preset = 60 - Freq = 9.96 Hz - Period = 0.100352 seconds
    OPTION_REGbits.T0CS = 0;  // bit 5  Selección de oscilador usado, en este caso oscilador interno
    OPTION_REGbits.T0SE = 0;  // bit 4 Bits de pulsos
    OPTION_REGbits.PSA = 0;   // bit 3  Preescaler asignado para el timer0
    OPTION_REGbits.PS2 = 1;   // bits 2-0  Selección de bits de preescaler
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;

    TMR0 = 246;             // preset for timer register
    INTCONbits.TMR0IF = 0;            // Se habilita la interrupción del timer0
    INTCONbits.TMR0IE = 1;            // Se limpia la bandera de timer0
return;
}


