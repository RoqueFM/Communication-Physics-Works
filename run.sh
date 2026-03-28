#!/bin/bash

# ================================
# Script FDTD: clean + compile + run + video
# ================================

# Argumentos
DIR="${1:-.}"        # Directorio (por defecto actual)
MODE="$2"            # --delete opcional

# Comprobar directorio
if [ ! -d "$DIR" ]; then
    echo "Error: el directorio '$DIR' no existe"
    exit 1
fi

echo "Directorio: $DIR"
echo "----------------------------------"

# ================================
# LIMPIEZA OPCIONAL (--delete)
# ================================
if [ "$MODE" == "--delete" ]; then
    echo "Modo limpieza: eliminando todo excepto .c (incluye subdirectorios)"
    find "$DIR" -mindepth 1 -maxdepth 1 ! -name "*.c" -exec rm -rf {} +
    echo "Limpieza completada"
    echo "----------------------------------"
fi

# ================================
# COMPILACIÓN Y EJECUCIÓN
# ================================
found=false

for file in "$DIR"/*.c; do

    [ -e "$file" ] || continue
    found=true

    base=$(basename "$file" .c)
    exe="$DIR/$base"

    echo "Compilando: $file"
    cc -O3 -fopenmp "$file" -lm -o "$exe".exe

    if [ $? -ne 0 ]; then
        echo "Error compilando $file"
        continue
    fi

    echo "Ejecutando: $exe, el log se guarda en $DIR/${base}.out"
    echo "----------------------------------"

    (
        cd "$DIR" || exit
        ./"$base".exe > "${base}.out"
    )

    echo "----------------------------------"
done

if [ "$found" = false ]; then
    echo "No hay archivos .c en el directorio"
    exit 0
fi

# ================================
# GENERAR VÍDEO Y GIF (si hay Frames)
# ================================
FRAMES_DIR="$DIR/Frames"

if [ -d "$FRAMES_DIR" ]; then
    echo "Carpeta Frames detectada → generando vídeo y GIF"

    # Comprobar ffmpeg
    if ! command -v ffmpeg &> /dev/null; then
        echo "Error: ffmpeg no está instalado"
        exit 1
    fi

    cd "$DIR" || exit

    # Generar vídeo MP4
    echo "Generando video.mp4 ..."
    ffmpeg -y -framerate 30 -i ./Frames/frame_%05d.ppm \
        -c:v libx264 -pix_fmt yuv420p video.mp4

    # Generar GIF (mejor calidad con palette)
    echo "Generando animation.gif ..."
    ffmpeg -y -i ./Frames/frame_%05d.ppm -vf "palettegen" ./Frames/palette.png
    ffmpeg -y -framerate 30 -i ./Frames/frame_%05d.ppm -i ./Frames/palette.png \
        -lavfi "paletteuse" animation.gif

    # Limpiar palette
    rm -f ./Frames/palette.png

    echo "Vídeo y GIF generados en $DIR"
    echo "----------------------------------"
else
    echo "No se encontró carpeta Frames → no se genera vídeo"
fi

echo "Proceso finalizado"