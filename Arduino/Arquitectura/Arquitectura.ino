/******************************************************************************

 Arquitectura orientativa para el TL #1 de SCA
 - Para que este código funciones se deben incluir las librerías invocadas.
 - Escribir nombres, fecha, breve descripcion de que hace el codigo y demas!!!

******************************************************************************/

#include "controlSiNo_sca.h"
#include "rpm.h"  // Necesario en el caso de encoder con sensor óptico

// --------- PINES ---------
#define PIN_SENSOR      2
#define PIN_MOTOR       6
#define PIN_BOTON       7
#define PIN_LED         13

// --------- PARÁMETROS ---------
#define DELTA_T         100  // milisegundos
#define HISTERESIS      100  // RPM
#define TIEMPO_ARRANQUE 5000 // 5 segundos

// --------- OBJETO CONTROL ---------
controlSiNo Electro(PIN_MOTOR);

// --------- VARIABLES GLOBALES ---------
unsigned long tiempo_actual = 0;
unsigned long tiempo_anterior = 0;
unsigned long tiempo_inicio = 0;
float rpm_objetivo = 800;
float rpm_medida = 0;
bool accion = false;

// --------- PROTOTIPOS ---------
bool debo_muestrear();
void establecer_objetivo();  // sólo si configuración puede cambiar
float medir();
void controlar();
void mostrar_encabezado();
void mostrar_datos();
bool boton_parada_presionado();
void parada();

//----------------------------------

void setup() {

  // Configuración de pines
  pinMode(PIN_SENSOR, INPUT);
  pinMode(PIN_BOTON, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);

  // Configuración de comunicación
  Serial.begin(9600);

  // Configuración de controlador y sensor
  Electro.Configurar(rpm_objetivo, HISTERESIS, SALIDA_NORMAL);
  ConfigurarRPM(PIN_SENSOR);

  // Establecer tiempo inicial y mostrar info
  tiempo_inicio = millis();
  mostrar_encabezado();

}

//----------------------------------

void loop() {

  // Parada de emergencia -------------
  if (boton_parada_presionado()) {
    parada();
  }

  // Muestreo periódico ---------------
  if (debo_muestrear()) {

    // En algunos casos pueden hacer una función  que sense 
    // los botones de entrada para establecer nuevo objetivo 
    // configurando nuevamente el controlador:
    establecer_objetivo();

    // Medimos!!!
    medir();

    // Controlamos:
    controlar();

    // Enviar datos a monitor serie
    mostrar_datos();

  }
}

//----------------------------------

bool debo_muestrear() {
  tiempo_actual = millis();
  if (tiempo_actual - tiempo_anterior >= DELTA_T) {
    tiempo_anterior = tiempo_anterior + DELTA_T;
    return true;
  }
  return false;
}

//----------------------------------

void establecer_objetivo() {
  // ...
  // Electro.Configurar(objetivo, HISTERESIS, SALIDA_NORMAL);
}

float medir() {
  rpm_medida = MedirRPM();
  return rpm_medida;  // estrictamente no es necesario devolver valor 
                      // porque rpm_medida es una variable global.
}

void controlar() {

  // Actuo si paso el tiempo de arranque
  if (millis() - tiempo_inicio >= TIEMPO_ARRANQUE) {
    accion = Electro.Controlar(rpm_medida);
  } else {
    accion = false;
  }

}

void mostrar_encabezado() {
  Serial.println("============================================");
  Serial.println("CONTROL DE RPM...");
  Serial.println("============================================");
  Serial.println("Datos: ");
  Serial.print  ("   RPM objetivo: ");
  Serial.print  (rpm_objetivo);
  Serial.println(" RPM");
  Serial.print  ("   Histéresis: ±");
  Serial.print  (HISTERESIS);
  Serial.println(" RPM");
  Serial.println("--------------------------------------------");
  Serial.println();
  Serial.println("Tiempo\tObjetivo\tMedido\tEstado");
  Serial.println("[s]\t[RPM]\t[RPM]\t[sí-no]");
}

void mostrar_datos() {
  Serial.print  ((1.00*(tiempo_actual - tiempo_inicio)/1000), 2);
  Serial.print  ("\t");
  Serial.print  (rpm_objetivo);
  Serial.print  ("\t");
  Serial.print  (rpm_medida);
  Serial.print  ("\t");
  Serial.println(estadoSalida);
}

//----------------------------------

bool boton_parada_presionado() {
  return digitalRead(PIN_BOTON) == LOW;
}

//----------------------------------

void parada() {

  // Apagar motor
  Electro.Apagar();
  Serial.println("PARADA DE EMERGENCIA");

  // Desactivar interrupción (en el caso que se utilicen)
  detachInterrupt(digitalPinToInterrupt(PIN_SENSOR));

  // Estado seguro
  while (true) {
    digitalWrite(PIN_LED, HIGH);
  }

}

//----------------------------------