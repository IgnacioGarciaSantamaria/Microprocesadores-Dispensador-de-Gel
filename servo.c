#include <xc.h>
#include <stdint.h>
#include "Pic32Ini.h"
#define POSICION_ABIERTA_SERVO 2500
#define POSICION_CERRADA_SERVO 4000

void InicializarServo(void) {
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPB15R = 5; // OC1 conectado a RB15
    SYSKEY = 0x1CA11CA1;
    OC1CON = 0; // OC OFF
    OC1R = 2500; // Tiempo en alto de 1 ms inicial
    OC1RS = 2500;
    OC1CON = 0x8006; // OC ON, modo PWM sin faltas
    T2CON = 0;
    TMR2 = 0;
    PR2 = 49999; // Periodo de 20 ms
    T2CON = 0x8010; // T2 ON, Div = 2
}

void abrirServo(void)
{
    OC1RS = POSICION_ABIERTA_SERVO;
}

void cerrarServo(void)
{
    OC1RS = POSICION_CERRADA_SERVO;
}
