#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "Temporizador.h"
#include "Pic32Ini.h"
#include "Bluetooth.h"
#include "sensorNivel.h"
#include "sensorProx.h"
#include "servo.h"

void InicializarDispensador(void);
void decision(void);

int static flag_T4;
int static control_mano;
int static inicio;
int static medida_sensor;

uint32_t static uso, uso_ant;

uint32_t static ticks_ant_T4;
uint32_t static ticks_act_T4;

int main(void) {

    InicializarDispensador();

    while (1) {
        //Sensor de nivel
        set_Q(leerSensorNivel());

        //Sensor de proximidad
        medida_sensor = leerSensorProx();

        //Rutina de la UART
        getcUART();
        procesarInfo();
        
        //Función general
        decision();
    }
}

void InicializarDispensador(void) {
    flag_T4 = 0;
    control_mano = 1;
    inicio = 1;

    uso = 0;
    uso_ant = 0;

    InicializarSensorProx();
    InicializarSensorNivel();
    InicializarServo();
    InicializarUART1(9600);
    InicializarTimer4();
}

void decision(void) {
    //CONTROL ENCENDIDO APAGO SENSOR
    if (((medida_sensor & 1) == 0) && control_mano) {
        flag_T4 = 1;
        control_mano = 0;
        tiempoReferencia();
        tiempoReferenciaUSO();
        ticks_ant_T4 = getTicksT4();
        uso_ant = uso_ant + 1;
        inicio = 0;
    }

    if ((uso_ant != uso) && (tiempoUSOS(100) == 0) && (flag_T4 == 0)) {
        uso = uso + 1;
        uso_ant = uso;
        set_USO(uso);
    }

    //He quitado la mano
    if ((control_mano == 0) && ((medida_sensor & 1) == 1)) {
        flag_T4 = 0;
        control_mano = 1;
    }
    
    //Control de la señal que muestro
    if (flag_T4) {
        flag_T4 = tiempoLED(get_T4());
        //LATCCLR = 1 << 3;
        abrirServo();
        //Actualizo tiempo ranura abierta
        ticks_act_T4 = getTicksT4();
    } else {
        ticks_ant_T4 = ticks_act_T4;
        //LATCSET = 1 << 3;
        cerrarServo();
    }

    //LATCSET = 0xF;
    //LATCCLR = uso;
}

