#include "Temporizador.h"
#include <xc.h>
static uint32_t ticks = 0; // Variable global al driver
static uint32_t ticks_ant, ticks_act;
static uint32_t ticks_ant_uso, ticks_act_uso;


void InicializarTimer4(void) {
    TMR4 = 0;
    PR4 = 5000; // Se configura el timer para que termine la cuenta en 1 ms
    IPC4bits.T4IP = 2; // Se configura la prioridad de la interrupción
    IPC4bits.T4IS = 0; // Subprioridad 0
    IFS0bits.T4IF = 0; // Se borra el flag de interrupción por si estaba pendiente
    IEC0bits.T4IE = 1; // y por último se habilita su interrupción
    T4CON = 0x8000; // Se arranca el timer con prescalado 1
    INTCONbits.MVEC = 1; // Modo multivector
    asm(" ei"); // Interr. habilitadas
    ticks_ant = getTicksT4();
}

__attribute__((vector(_TIMER_4_VECTOR), interrupt(IPL4SOFT), nomips16))
void InterrupcionTimer4(void) {
    // Se borra el flag de interrupción
    IFS0bits.T4IF = 0;
    ticks++;
}

uint32_t getTicksT4(void) {
    uint32_t c_ticks;
    asm(" di");
    c_ticks = ticks;
    asm(" ei");
    return c_ticks;
}

int tiempoLED(int tiempo) {
    ticks_act = getTicksT4();
    if ((ticks_act - ticks_ant) > tiempo) {
        ticks_ant = ticks_act;
        return 0;
    }
    return 1;
}

int tiempoUSOS(int tiempo) {
    ticks_act_uso = getTicksT4();
    if ((ticks_act_uso - ticks_ant_uso) > tiempo) {
        ticks_ant_uso = ticks_act_uso;
        return 0;
    }
    return 1;
}


void tiempoReferencia(){
    ticks_ant = getTicksT4();
}

void tiempoReferenciaUSO(){
    ticks_ant_uso = getTicksT4();
}






