/******************************************************************************
* Velocidad de cambios de estado
*******************************************************************************
* Archivo: rpm.h
* Resumen: El módulo reúne algunas funciones para calcular la velocidad
*          con que un pin cambia de estado.
* Autor:   Guillermo Caporaletti <gfcaporaletti@undav.edu.ar>
* Fecha:   Abril de 2023
******************************************************************************/

/******************************************************************************
* Librería y módulos incluidas
******************************************************************************/

#include "arduino.h"

/******************************************************************************
* Constantes definidas
******************************************************************************/

#define MAX_VUELTAS_RPM       3000                // Máxima cantidad de vueltas por minuto, físicamente posible
                                                  // (se recomienda poner un 50% más que el real).
#define MAX_VUELTAS_PS        MAX_VUELTAS_RPM/60  // Máxima cantidad de vueltas por segundo, físicamente posible.
#define CAMBIOS_P_VUELTA      4                   // Cambios de estado producidos en cada vuelta.
#define MIN_US_ENTRE_CAMBIOS  1000000 / ( MAX_VUELTAS_PS * CAMBIOS_P_VUELTA )  
                                                  // Mínimo tiempo posible entre cambios de estado.
#define RPM_POR_US            60*1000000 / CAMBIOS_P_VUELTA
                                                  // Constante para calcular RPM a partir de us.

/******************************************************************************
* Funciones públicas
******************************************************************************/

void ConfigurarRPM(byte PIN);            // Configura el pin a leer e inicializa variables
int  MedirRPM();                         // Mide la velocidad en función de las interrupciones que haya habido

/******************************************************************************
* FIN DE ARCHIVO velocidad.h
******************************************************************************/
