import numpy as np
from PIL import Image, ImageDraw, ImageFont
import glob

# Configuración de la grid
IMAX, JMAX = 1000, 120
grid_x = 50  # cada 100 m en x
grid_y = 10   # cada 20 m en y

# Fuente para los números (puede ser nativa o cargar una .ttf)
try:
    font = ImageFont.truetype("arial.ttf", 12)
except:
    font = ImageFont.load_default()

frames = []
for fname in sorted(glob.glob("Frames/frame_*.ppm")):
    img = Image.open(fname).convert("RGB")
    draw = ImageDraw.Draw(img)
    
    # Dibujar líneas verticales (x)
    for x in range(0, IMAX, grid_x):
        draw.line([(x,0),(x,JMAX-1)], fill=(255,255,255))
        draw.text((x+2,2), str(x), fill=(255,255,255), font=font)
    
    # Dibujar líneas horizontales (y)
    for y in range(0, JMAX, grid_y):
        draw.line([(0,y),(IMAX-1,y)], fill=(255,255,255))
        draw.text((2,y+2), str(y), fill=(255,255,255), font=font)
    
    frames.append(img)

# Guardar GIF
frames[0].save('simulation_with_grid.gif', save_all=True, append_images=frames[1:], duration=50, loop=0)
print("GIF listo")

# Guardar vídeo (requiere ffmpeg)
frames[0].save('simulation_with_grid.mp4', save_all=True, append_images=frames[1:], duration=50, loop=0)
print("VIDEO listo")