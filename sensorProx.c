#include <xc.h>
#include <stdint.h>
#define SENSOR 7

void InicializarSensorProx(void)
{
    ANSELA &= ~(1 << SENSOR);
    TRISA |= 1 << SENSOR;
}

int leerSensorProx(void)
{
    return PORTA >> SENSOR;
}