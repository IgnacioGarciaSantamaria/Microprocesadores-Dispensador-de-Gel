/* 
 * File:   Bluetooth.h
 * Author: ignac
 *
 * Created on 11 de abril de 2022, 18:13
 */

#ifndef BLUETOOTH_H
#define	BLUETOOTH_H

#ifdef	__cplusplus
extern "C" {
#endif

void getcUART(void);
void putsUART(char cadena[]);
void InicializarUART1(int baudios);
void procesarInfo(void);
int ASCIItoInt(char c);
int get_T4(void);
void set_Q(int cantidad);
void set_USO(int numero_usos);


#ifdef	__cplusplus
}
#endif

#endif	/* BLUETOOTH_H */

