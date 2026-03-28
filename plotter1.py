import numpy as np
import matplotlib.pyplot as plt
import sys
import os
from scipy.constants import speed_of_light

# =========================
# ARGUMENTO
# =========================
if len(sys.argv) < 2:
    print("Uso: python3 plotter1.py <directorio>")
    sys.exit(1)

DIR = sys.argv[1]

if not os.path.isdir(DIR):
    print(f"Error: el directorio '{DIR}' no existe")
    sys.exit(1)

# =========================
# ARCHIVOS
# =========================
files = [
    ("ex_frente_a_t.txt", 0),  # subplot 0
    ("ey_frente_a_t.txt", 1)   # subplot 1
]

# =========================
# PARSE CABECERA
# =========================
def parse_header(path):
    with open(path, "r") as f:
        header = f.readline().strip()

    header = header.lstrip("#").strip()
    parts = header.split()

    field = parts[2]      # $E_x$, $E_y$
    points = parts[4:]    # (i,j)

    return field, points

# =========================
# FIGURA
# =========================
fig, axs = plt.subplots(2, 1, sharex=True, figsize=(8, 6))

# =========================
# PLOT
# =========================
for file, idx in files:
    path = os.path.join(DIR, file)

    if not os.path.isfile(path):
        print(f"Aviso: no se encuentra {path}")
        continue

    field, points = parse_header(path)
    data = np.loadtxt(path, comments="#")

    t = data[:, 0] * 1e6  # μs

    ax = axs[idx]

    for i, point in enumerate(points):
        ax.scatter(t, data[:, i+1]*1e3, s=10,
                   label=f"{field} {point}")
    
    splitted = point.split(",")
    x = float(splitted[0][1:])
    y = float(splitted[1][:-1])
    d = np.sqrt((x-500)**2 + (y-500)**2)
    # d = np.abs(500-x)
    t = d/speed_of_light
    ax.scatter(t*1e6,0,label="$t=c/d$",s=200,marker='*',color='black')
    ax.set_ylabel(r"$E$ (mV/m)")
    ax.set_title(field)
    ax.grid(True)
    ax.legend()
    from matplotlib.ticker import AutoMinorLocator

    ax.xaxis.set_minor_locator(AutoMinorLocator())
    ax.yaxis.set_minor_locator(AutoMinorLocator())

    ax.grid(which='major', linestyle='-', linewidth=0.8, alpha=0.9)
    ax.grid(which='minor', linestyle=':', linewidth=0.6, alpha=0.7)

# =========================
# FORMATO FINAL
# =========================
axs[-1].set_xlabel(r"$t$ ($\mu$s)")

plt.tight_layout()
plt.savefig(os.path.join(DIR, "evolucion_temporal.png"),dpi=700)
plt.show()