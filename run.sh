#!/bin/bash

# ================================
# Script FDTD: clean + compile + run
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
    cc -O3 -fopenmp "$file" -lm -o "$exe"

    if [ $? -ne 0 ]; then
        echo "Error compilando $file"
        continue
    fi

    echo "Ejecutando: $exe, el log se guarda en $DIR/${base}.out"
    echo "----------------------------------"

    # 🔑 CLAVE: ejecutar dentro del directorio
    (
        cd "$DIR" || exit
        ./"$base" > "${base}.out"
    )

    echo "----------------------------------"
done

if [ "$found" = false ]; then
    echo "No hay archivos .c en el directorio"
fi

echo "Proceso finalizado"