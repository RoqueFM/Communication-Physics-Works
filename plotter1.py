import numpy as np
from matplotlib import pyplot as plt

data = np.loadtxt("ex_frente_a_t.txt")

plt.scatter(data[:,0]*1e6,data[:,1]*1e3,s=10,label="(600,55)")
plt.scatter(data[:,0]*1e6,data[:,2]*1e3,s=10,label="(500,740)")
plt.xlabel("$t$ ($\mu$s)")
plt.ylabel("$E$ (mV/m)")
plt.legend()
plt.grid(True)
plt.show()