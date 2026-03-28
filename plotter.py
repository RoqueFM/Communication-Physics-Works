import numpy as np
import matplotlib.pyplot as plt
import sys
import os

# =========================
# ARGUMENTO
# =========================
if len(sys.argv) < 2:
    print("Uso: python3 plotter2.py <directorio>")
    sys.exit(1)

DIR = sys.argv[1]

if not os.path.isdir(DIR):
    print(f"Error: el directorio '{DIR}' no existe")
    sys.exit(1)

# =========================
# ARCHIVOS
# =========================
ex_path = os.path.join(DIR, "field_ex.dat")
ey_path = os.path.join(DIR, "field_ey.dat")

if not os.path.isfile(ex_path):
    print(f"Error: no se encuentra {ex_path}")
    sys.exit(1)

if not os.path.isfile(ey_path):
    print(f"Error: no se encuentra {ey_path}")
    sys.exit(1)

# =========================
# CARGA DE DATOS
# =========================
ex = np.loadtxt(ex_path) * 1e3  # mV/m
ey = np.loadtxt(ey_path) * 1e3

# =========================
# ESCALA DE COLOR CONSISTENTE
# =========================
vmax = (np.mean(ex) + np.mean(ey))/2. + np.sqrt(np.std(ex)**2 + np.std(ey)**2)*2
# vmax = 5
vmin = -vmax

# =========================
# FIGURA
# =========================
fig, axs = plt.subplots(1, 2, figsize=(10, 4))

# -------------------------
# Subplot 1: Ex
# -------------------------
im1 = axs[0].imshow(
    ex,
    vmin=vmin,
    vmax=vmax,
    cmap="RdBu",
    origin="lower",
    aspect="equal"
)

axs[0].set_title(r"$E_x$")
axs[0].set_xlabel("$x$ (m)")
axs[0].set_ylabel("$y$ (m)")

cbar1 = plt.colorbar(im1, ax=axs[0])
cbar1.set_label("Campo eléctrico (mV/m)")

# -------------------------
# Subplot 2: Ey
# -------------------------
im2 = axs[1].imshow(
    ey,
    vmin=vmin,
    vmax=vmax,
    cmap="RdBu",
    origin="lower",
    aspect="equal"
)

axs[1].set_title(r"$E_y$")
axs[1].set_xlabel("$x$ (m)")
axs[1].set_ylabel("$y$ (m)")

cbar2 = plt.colorbar(im2, ax=axs[1])
cbar2.set_label("Campo eléctrico (mV/m)")

# =========================
# AJUSTES VISUALES
# =========================
for ax in axs:
    ax.set_xticks(np.linspace(0, ex.shape[1], 6))
    ax.set_yticks(np.linspace(0, ex.shape[0], 6))
    ax.grid(color='k', linestyle=':', linewidth=0.3, alpha=0.3)

plt.tight_layout()
plt.savefig(os.path.join(DIR, "mapa_campos.png"),dpi=600)

# =========================
# FIGURA EXTRA: Ex (colorbar horizontal)
# =========================
fig_ex, ax_ex = plt.subplots()

im_ex = ax_ex.imshow(
    ex,
    vmin=vmin,
    vmax=vmax,
    cmap="RdBu",
    origin="lower",
    aspect="equal"
)

ax_ex.set_title(r"$E_x$")
ax_ex.set_xlabel("$x$ (m)")
ax_ex.set_ylabel("$y$ (m)")

cbar_ex = plt.colorbar(im_ex, ax=ax_ex, orientation='horizontal', pad=0.1)
cbar_ex.set_label("Campo eléctrico (mV/m)")

ax_ex.set_xticks(np.linspace(0, ex.shape[1], 6))
ax_ex.set_yticks(np.linspace(0, ex.shape[0], 6))
ax_ex.grid(color='k', linestyle=':', linewidth=0.3, alpha=0.3)

plt.tight_layout()
plt.savefig(os.path.join(DIR, "Ex.png"), dpi=600)


# =========================
# FIGURA EXTRA: Ey (colorbar horizontal)
# =========================
fig_ey, ax_ey = plt.subplots()

im_ey = ax_ey.imshow(
    ey,
    vmin=vmin,
    vmax=vmax,
    cmap="RdBu",
    origin="lower",
    aspect="equal"
)

ax_ey.set_title(r"$E_y$")
ax_ey.set_xlabel("$x$ (m)")
ax_ey.set_ylabel("$y$ (m)")

cbar_ey = plt.colorbar(im_ey, ax=ax_ey, orientation='horizontal', pad=0.1)
cbar_ey.set_label("Campo eléctrico (mV/m)")

ax_ey.set_xticks(np.linspace(0, ey.shape[1], 6))
ax_ey.set_yticks(np.linspace(0, ey.shape[0], 6))
ax_ey.grid(color='k', linestyle=':', linewidth=0.3, alpha=0.3)

plt.tight_layout()
plt.savefig(os.path.join(DIR, "Ey.png"), dpi=600)

plt.show()