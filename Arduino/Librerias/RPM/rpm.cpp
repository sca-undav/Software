/******************************************************************************
* Velocidad de cambios de estado
*******************************************************************************
* Archivo: rpm.cpp
* Resumen: El módulo reúne algunas funciones para calcular la velocidad
*          con que un pin cambia de estado.
* Autor:   Guillermo Caporaletti <gfcaporaletti@undav.edu.ar>
* Fecha:   Abril de 2023
******************************************************************************/

/******************************************************************************
* Librería y módulos incluidas
******************************************************************************/

#include "rpm.h"

/******************************************************************************
* Variables privadas (para manejo de la ISR)
******************************************************************************/

byte PinEntrada;                      // Pin de entrada cuya velocidad de cambio de estado deseamos medir 
volatile unsigned long usAnterior=0;  // Almacena el último tiempo de la interrupción en microsegundos.
volatile unsigned long usDelta=0;     // Almacena cuánto pasó entre cada interrupción en microsegundos.
volatile unsigned long contadorISR=0; // Cuenta cambios de estado: otro modo de registro.
volatile bool estadoSensor;           // Guarda estado último para evitar falsas detecciones de cambios de estado.
int velocidadAnterior = 0;            // Esto puede usarse para promediar las velocidades medidas

/******************************************************************************
* Funciones privadas
******************************************************************************/

// ISR que calcula cuánto pasó desde la anterior interrupción
// La respuesta en velocidad la da otra función (que es pública)
void ISRcalculaDelta()
{
  unsigned long usActual = micros();
  if (digitalRead(PinEntrada) != estadoSensor && usActual - usAnterior > MIN_US_ENTRE_CAMBIOS)
  // Si hubo cambio de estado y pasó más del tiempo mínimo...
  {
    estadoSensor = !estadoSensor;     // Cambió el estado del sensor
    usDelta = usActual-usAnterior;    // Calcula delta (sin filtro promediador)
    contadorISR++;                    // Agrega cuenta
    usAnterior = usActual;            // Actualiza
    // usDelta    --> Almacena cuanto pasó entre cada interrupción en microsegundos.
    // usAnterior --> Almacena el último tiempo de la interrupción en microsegundos.
  }
}

/******************************************************************************
* Funciones públicas
******************************************************************************/

// Configura la interrupción
void ConfigurarRPM(byte PIN)
{
  PinEntrada = PIN;             // Almacenamos el pin que debo leer 
  pinMode(PinEntrada, INPUT);   // Lo seteo como entrada digital
  attachInterrupt(digitalPinToInterrupt(PinEntrada),ISRcalculaDelta,CHANGE); 
              // Configuro la función a llamar en cada interrupción
  estadoSensor = digitalRead(PinEntrada);  
              // Verifico incialmente en qué estado está el sensor y lo guardo en la variable.
}

// Devuelve la velocidad en RPM según el tiempo entre cambios de estado
int MedirRPM()
{
  int med = 0;
  unsigned long usActual = micros();
  // micros()--> es el tiempo que paso desde que se inicio el programa en microsegundos

  if ( usAnterior==0 ) {
    // Aún no hubo ninguna interrupción
    // Supongo una velocidad de 0 RPM
    med = 0;
    
  } else if ( (usActual-usAnterior)>usDelta ) {
    // usAnterior --> el delta de la ultima interrupcion
    // usDelta    --> el tiempo q paso entre la interrupcion anterior y la actual
    // Quiere decir que la velocidad real es menor a la que indica msDelta
    // (Esto puede ocurrir porque msDelta sólo se actualiza cuando ocurre la interrupción)
    med = ( RPM_POR_US / (usActual-usAnterior) );
  } else {
    // Calculo con usDelta :-) 
    med = ( RPM_POR_US / usDelta );
  }

  // Ya tengo un valor med: 
  // Puedo limitar y/o promediar su valor.
  med = min(med, MAX_VUELTAS_RPM);
  // med = 0.3*velocidadAnterior + 0.7*med;
  // velocidadAnterior = med;

  // Listo!!!
  return med;
}

/******************************************************************************
* FIN DE ARCHIVO velocidad.cpp
******************************************************************************/
