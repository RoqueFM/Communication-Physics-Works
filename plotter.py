import numpy as np
import matplotlib.pyplot as plt

# Cargar datos
ex = np.loadtxt("field_ex.dat")
ey = np.loadtxt("field_ey.dat")

# Crear figura con 2 subplots
fig, axs = plt.subplots(1, 2, figsize=(10, 4))

# -------------------------
# Subplot 1: Ex
# -------------------------
im1 = axs[0].imshow(ex, vmin=-0.005, vmax=0.005, cmap="RdBu", origin="lower")
axs[0].set_title(r"$E_x$")
axs[0].set_xlabel("x")
axs[0].set_ylabel("y")

cbar1 = plt.colorbar(im1, ax=axs[0])
cbar1.set_label("Campo eléctrico (V/m)")

# -------------------------
# Subplot 2: Ey
# -------------------------
im2 = axs[1].imshow(ey, vmin=-0.005, vmax=0.005, cmap="RdBu", origin="lower")
axs[1].set_title(r"$E_y$")
axs[1].set_xlabel("x")
axs[1].set_ylabel("y")

cbar2 = plt.colorbar(im2, ax=axs[1])
cbar2.set_label("Campo eléctrico (V/m)")

# Ajuste de layout
plt.tight_layout()

plt.show()