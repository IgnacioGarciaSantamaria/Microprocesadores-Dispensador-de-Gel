/* 
 * File:   Temporizador.h
 * Author: ignac
 *
 * Created on 22 de febrero de 2022, 18:12
 */

#ifndef TEMPORIZADOR_H
#define	TEMPORIZADOR_H
#include <stdint.h> // Define uint32_t

#ifdef	__cplusplus
extern "C" {
#endif

void InicializarTimer4(void);
uint32_t getTicksT4(void);
int tiempoLED(int tiempo);
void tiempoReferencia(void);
int tiempoUSOS(int tiempo);
void tiempoReferenciaUSO(void);



#ifdef	__cplusplus
}
#endif

#endif	/* TEMPORIZADOR_H */


