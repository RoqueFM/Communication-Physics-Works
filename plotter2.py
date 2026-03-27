import numpy as np
import matplotlib.pyplot as plt
import imageio
import glob

files = sorted(glob.glob("./Frames/frame_*.dat"))

images = []

for f in files:

    d = np.loadtxt(f)

    fig,ax = plt.subplots(figsize=(5,5))

    im=ax.imshow(
        d,
        cmap="RdBu",
        vmin=-0.5,
        vmax=0.5,
        origin="lower"
    )

    ax.axis("off")

    fig.canvas.draw()

    image=np.frombuffer(
        fig.canvas.tostring_rgb(),
        dtype='uint8'
    )

    image=image.reshape(fig.canvas.get_width_height()[::-1]+(3,))

    images.append(image)
    print(f)
    plt.close(fig)

imageio.mimsave("fdtd.gif",images,fps=20)