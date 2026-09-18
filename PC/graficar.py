#!/usr/bin/env python3
# =====================================================================
# Archivo:     graficar.py  
# Breve:       Graficación de los datos adquiridos utilizado en 
#              Sistemas de Control Automático (UNDAV).
# Autor:       Tomás Domancich
# Fecha:       Junio 2026
# 
# Descripción: 
#   Lee un CSV generado por capturar.py y grafica:
#    - Temperatura promedio + sensores individuales + setpoint
#    - Señal de control u(t) [V]  (subgráfico inferior)
#   Exporta automáticamente un PNG con el mismo nombre del CSV.
#
# Uso:
#    python graficar.py archivo.csv ["Título del gráfico"]
#
# Ejemplos:
#   python graficar.py datos_control_P.csv "Control P (Kp=20)"
#   python graficar.py datos_control_PI.csv "Control PI (Kp=20, Ti=100min)"
#
# Requiere:    pip install numpy matplotlib
# =====================================================================

import sys
import os
import numpy as np
import matplotlib.pyplot as plt

SETPOINT = 40.0   # °C (solo para la línea de referencia)


def cargar(nombre):
    d = np.genfromtxt(nombre, delimiter="\t", names=True)
    t_min = d["Tiempo_ms"] / 60000.0
    return t_min, d["TempProm"], d["Temp1"], d["Temp2"], d["Tension_V"]


def graficar(csv, titulo):
    t, Tprom, s1, s2, u = cargar(csv)

    fig, (ax1, ax2) = plt.subplots(
        2, 1, figsize=(8, 6), dpi=110, sharex=True,
        gridspec_kw={"height_ratios": [3, 1]},
    )

    # --- Temperaturas ---
    ax1.plot(t, s1, color="#7fb3d5", lw=0.8, label="Sensor 1 (sup)")
    ax1.plot(t, s2, color="#e8a87c", lw=0.8, label="Sensor 2 (inf)")
    ax1.plot(t, Tprom, color="black", lw=1.3, label="Promedio")
    ax1.axhline(SETPOINT, color="gray", ls="--", lw=1.0)
    ax1.set_ylabel("Temperatura [°C]")
    ax1.set_title(titulo)
    ax1.legend(loc="lower right", fontsize=8)
    ax1.grid(True, alpha=0.3)

    # --- Señal de control ---
    ax2.plot(t, u, color="green", lw=1.0)
    ax2.set_ylabel("u(t) [V]")
    ax2.set_xlabel("Tiempo [min]")
    ax2.grid(True, alpha=0.3)

    plt.tight_layout()
    salida = os.path.splitext(csv)[0] + ".png"
    plt.savefig(salida, facecolor="white")
    print(f"# Gráfico guardado en {salida}")


def main():
    if len(sys.argv) < 2:
        print('Uso: python graficar.py archivo.csv ["Título"]')
        sys.exit(1)
    csv = sys.argv[1]
    titulo = sys.argv[2] if len(sys.argv) > 2 else os.path.basename(csv)
    graficar(csv, titulo)


if __name__ == "__main__":
    main()
