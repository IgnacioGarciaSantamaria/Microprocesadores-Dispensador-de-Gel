#include <xc.h>
#include <stdint.h>
#define PIN_POT 3
#define CANAL_POT 5
#define VALOR_MAX 15

static int valor_medio, valor_respuesta;
static int valor_max = 1;

void InicializarSensorNivel(void);
int getPorcentaje(int valor);
int leerSensorNivel(void);
int getPorcentaje(int valor);

__attribute__((vector(23), interrupt(IPL2SOFT), nomips16)) void InterrupcionADC(void) {
    IFS0bits.AD1IF = 0; // Se borra el flag
    valor_medio = (ADC1BUF0 + ADC1BUF1 + ADC1BUF2 + ADC1BUF3) >> 2;
    valor_respuesta = (valor_medio >> 4);
}

void InicializarSensorNivel(void)
{
    // Definición de entradas
    TRISB |= (1 << PIN_POT);
    AD1CON1 = 0; // Apago el ADC
    AD1CON2 = 3 << 2; // SMPI = 3 (4conver) 
    AD1CON3=0x200;//TADC=2TPBCLK y SAMC=2 2TADC 
    AD1CHS=0x00050000;//CH0SA=AN5,CH0NA=0V 
    AD1CON1bits.SSRC = 2;//Timer3 
    AD1CON1bits.ASAM = 1;//Auto Sampling 
    IFS0bits.AD1IF = 0; // Borro el flag 
    IEC0bits.AD1IE = 1; // Habilito 
    IPC5bits.AD1IP = 1; // Prioridad 2 
    IPC5bits.AD1IS = 0; // Subprioridad 0 
    AD1CON1bits.ON = 1; // Arranco el ADC
    T3CON = 0;
    TMR3 = 0;
    PR3 = 5000; // Periodo de 1 ms
    T3CON = 0x8000; // T3 ON, Div = 1 
}

int leerSensorNivel(void)
{
    int c_respuesta;
    asm(" di");
    c_respuesta = getPorcentaje(valor_respuesta);
    asm(" ei");
    return c_respuesta;
}

int getPorcentaje(int valor)
{
    int porcentaje = (valor*100)/(VALOR_MAX);
    return porcentaje;
}

