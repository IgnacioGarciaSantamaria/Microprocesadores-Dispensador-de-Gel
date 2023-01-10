#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#define TAM_COLA 100
#define TAM_CAD 9

typedef struct {
    int icabeza;
    int icola;
    char cola[TAM_COLA];
} cola_t;

static cola_t cola_tx, cola_rx;
char respuesta[100];
int index_respuesta = 0;
int flag_respuesta = 0;
int flag_size = 0;


uint32_t t4 = 1000;
uint32_t get_T4(void){
    return t4;
}


uint32_t q = 0;
void set_Q(uint32_t cantidad){
    asm(" di");
    q=cantidad;
    asm(" ei");
}
uint32_t uso = 0;
void set_USO(uint32_t numero_usos){
    asm(" di");
    uso = numero_usos/2;
    asm(" ei");
}

__attribute__((vector(32), interrupt(IPL3SOFT), nomips16))

void InterrupcionUART1(void) {
    if (IFS1bits.U1RXIF == 1) { // Ha interrumpido el receptor
        if ((cola_rx.icabeza + 1 == cola_rx.icola) || (cola_rx.icabeza + 1 == TAM_COLA && cola_rx.icola == 0)) {
            LATC &= ~1;
        } else {
            cola_rx.cola[cola_rx.icola] = U1RXREG; // Lee carácter de la UART
            cola_rx.icabeza++;
            if (cola_rx.icabeza == TAM_COLA) {
                cola_rx.icabeza = 0;
            }
        }
        IFS1bits.U1RXIF = 0; // Y para terminar se borra el flag
    }
    if (IFS1bits.U1TXIF == 1) { // Ha interrumpido el transmisor
        if (cola_tx.icola != cola_tx.icabeza) { // Hay datos nuevos
            U1TXREG = cola_tx.cola[cola_tx.icola];
            cola_tx.icola++;
            if (cola_tx.icola == TAM_COLA) {
                cola_tx.icola = 0;
            }
        } else { // Se ha vaciado la cola.
            IEC1bits.U1TXIE = 0; // Para evitar bucle sin fin
        }
        IFS1bits.U1TXIF = 0; // Y para terminar se borra el flag
    }
}

void getcUART(void) {

    if (cola_rx.icola != cola_rx.icabeza) { // Hay datos nuevos 
        respuesta[index_respuesta] = cola_rx.cola[cola_rx.icola];
        cola_rx.icola++;
        if (cola_rx.icola == TAM_COLA) {
            cola_rx.icola = 0;
        }

        if (respuesta[index_respuesta] != 'n') {
            index_respuesta++;
        } else {
            respuesta[index_respuesta] = '\0'; // no ha llegado nada
            flag_respuesta = 1;
        }

    }
}

void putsUART(char cadena[]) {
    int index_cadena = 0;
    while (cadena[index_cadena] != '\n') {
        if ((cola_tx.icabeza + 1 == cola_tx.icola) || (cola_tx.icabeza + 1 == TAM_COLA && cola_tx.icola == 0)) {
            LATC &= ~2;
            break;
        } else {
            cola_tx.cola[cola_tx.icabeza] = cadena[index_cadena]; // Copia el carácter en la cola
            cola_tx.icabeza++;
            index_cadena++;
            if (cola_tx.icabeza == TAM_COLA) {
                cola_tx.icabeza = 0;
            }
        }
    }
    // Se habilitan las interrupciones del transmisor para comenzar a enviar
    IEC1bits.U1TXIE = 1;
}

void InicializarUART1(int baudios) {

    cola_tx.icabeza = 0;
    cola_tx.icola = 0;
    cola_rx.icabeza = 0;
    cola_rx.icola = 0;

    ANSELB &= ~((1 << 13) | (1 << 7)); // Se configura RB13 y RB7 como digital
    TRISB |= 1 << 13; // y como entrada
    LATB |= 1 << 7; // A 1 si el transmisor está inhabilitado.
    SYSKEY = 0xAA996655; // Se desbloquean los registros
    SYSKEY = 0x556699AA; // de configuración.
    U1RXR = 3; // U1RX conectado a RB13.
    RPB7R = 1; // U1TX conectado a RB7.
    SYSKEY = 0x1CA11CA1; // Se vuelven a bloquear.

    if (baudios > 38400) {
        U1MODEbits.BRGH = 1;
        U1BRG = (5000000 / (4 * baudios)) - 1;
    } else {
        U1MODEbits.BRGH = 0;
        U1BRG = (5000000 / (16 * baudios)) - 1;
    }
    IFS1bits.U1RXIF = 0; // Borro flag por si acaso
    IEC1bits.U1RXIE = 1; // Habilito interrupciones
    IFS1bits.U1TXIF = 0; // Borro flag del transmisor por si acaso
    IPC8bits.U1IP = 3; // Prioridad 3
    IPC8bits.U1IS = 1; // Subprioridad 1

    U1STAbits.URXISEL = 0; // Interrupción cuando llegue 1 char.
    U1STAbits.UTXISEL = 2; // Interrupción cuando FIFO vacía.
    U1STAbits.URXEN = 1; // Se habilita el receptor.
    U1STAbits.UTXEN = 1; // Y el transmisor, fuerza un 1er envio
    U1MODEbits.ON = 1;

    ANSELC &= ~3; // Bits 0 y 1 de los LEDS
    TRISC &= ~3;
    LATC |= 3;

    INTCON |= 1 << 12;
    asm(" ei");

}

int ASCIItoInt(char c) {
    int sol;
    if (c <= '9') {
        sol = (int) c - 48;
    } else {
        sol = (int) c - 55;
    }
    return sol;
}

void procesarInfo(void) {
    char cad[50];
    if ((flag_respuesta == 1)) {
        flag_respuesta = 0;
        
        if( (((respuesta[0]) == 'G') || ((respuesta[0]) == 'S')) && (respuesta[1] == 'E') && (respuesta[2] == 'T') && (index_respuesta < 12)){
            if((respuesta[0]) == 'G'){
                if((respuesta[4] == 'T') && (respuesta[5] == '4')){
                    sprintf(cad, "TIMER4 (ms): %d\n", t4);
                    putsUART(cad);
                }else if ((respuesta[4] == 'Q')){
                    sprintf(cad, "QUANTITY (percentage): %d\n", q);
                    putsUART(cad);
                } else if ((respuesta[4] == 'U') && (respuesta[5] == 'S') && (respuesta[6] == 'O')){
                    sprintf(cad, "NUMERO USOS:  %d\n", uso);
                    putsUART(cad);                    
                } else {
                   putsUART("Error Instruccion GET Incorrecta\n"); 
                }
            }else{
                if((respuesta[4] == 'T') && (respuesta[5] == '4')){
                    if(index_respuesta == 10){
                        t4 = ASCIItoInt(respuesta[7])*100 + ASCIItoInt(respuesta[8])*10 + ASCIItoInt(respuesta[9]);
                        sprintf(cad, "TIMER4 CHANGED TO: %d\n", t4);
                        putsUART(cad);
                    } else if (index_respuesta == 11) {
                        t4 = ASCIItoInt(respuesta[7])*1000 + ASCIItoInt(respuesta[8])*100 + ASCIItoInt(respuesta[9])*10 + ASCIItoInt(respuesta[10]);
                        sprintf(cad, "TIMER4 CHANGED TO: %d\n", t4);
                        putsUART(cad);
                    } else {
                        putsUART("Error Instruccion SET | Size Timer [100-9999]\n");
                    }
                }else{
                    putsUART("Error Instruccion SET Incorrecta\n");
                }
            }
        } else {
            putsUART("Error Instruccion Incorrecta\n");
        }
        for (int i = 0; i < TAM_CAD; i++) {
            respuesta[i] = '\0';
        }
        index_respuesta = 0;
    }
}







