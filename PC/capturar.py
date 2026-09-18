#!/usr/bin/env python3
# =============================================================================
# Archivo:     capturar.py  
# Breve:       Adquisición de datos por puerto serie utilizado en 
#              Sistemas de Control Automático (UNDAV).
# Autor:       Tomás Domancich
# Fecha:       Junio 2026
# Descripción:
#   Lee las líneas que el ESP32 emite por Serial (formato TSV definido en
#   el sketch codigo_SCA_PI_ESP32_TL2.ino) y las guarda en un CSV con
#   marca de tiempo, mostrando además las mediciones en pantalla.
#
# Uso:
#   python capturar.py [puerto] [archivo_salida.csv]
#
# Ejemplos:
#   python capturar.py COM5 datos_control_PI.csv      (Windows)
#   python capturar.py /dev/ttyUSB0 datos_control_P.csv (Linux)
#
# Requiere:    pip install pyserial
# =============================================================================

import sys
import time
import serial   # pyserial

# ---- Parámetros por defecto ----
PUERTO_DEFAULT = "/dev/ttyUSB0"   # cambiar a COMx en Windows
BAUDIOS        = 115200
SALIDA_DEFAULT = "datos.csv"

# Encabezado que emite el ESP32 (debe coincidir con el sketch)
ENCABEZADO = "Tiempo_ms\tTempProm\tTemp1\tTemp2\tSetpoint\tTension_V\tDuty"

def main():
    puerto = sys.argv[1] if len(sys.argv) > 1 else PUERTO_DEFAULT
    salida = sys.argv[2] if len(sys.argv) > 2 else SALIDA_DEFAULT

    print(f"# Abriendo {puerto} @ {BAUDIOS} baudios...")
    try:
        ser = serial.Serial(puerto, BAUDIOS, timeout=2)
    except serial.SerialException as e:
        print(f"ERROR: no se pudo abrir {puerto}: {e}")
        sys.exit(1)

    time.sleep(2)          # espera al reset del ESP32 al abrir el puerto
    ser.reset_input_buffer()

    print(f"# Guardando en {salida}  (Ctrl+C para terminar)")
    n = 0
    with open(salida, "w", encoding="utf-8") as f:
        f.write(ENCABEZADO + "\n")
        try:
            while True:
                linea = ser.readline().decode("utf-8", errors="ignore").strip()
                if not linea:
                    continue
                # Ignora líneas de comentario / encabezado del firmware
                if linea.startswith("#") or linea.startswith("Tiempo"):
                    print(linea)
                    continue
                # Línea de datos válida: la muestra y la guarda
                f.write(linea + "\n")
                f.flush()
                n += 1
                print(f"[{n:5d}] {linea}")
        except KeyboardInterrupt:
            print(f"\n# Detenido por el usuario. {n} muestras guardadas en {salida}.")
        finally:
            ser.close()

if __name__ == "__main__":
    main()
