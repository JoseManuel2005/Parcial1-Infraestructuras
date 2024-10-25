#!/usr/bin/env bash
#
# Este script se encarga de invocar los tres programas que permiten la 
# conversion de un PNG a secuencia de pixeles, se procesan esos pixeles y
# posteriormente se convierte esa secuencia de pixeles a un archivo en formato
# PNG
#
# Autor: John Sanabria - john.sanabria@correounivalle.edu.co
# Fecha: 2024-08-22
#
# INPUT_PNG="image.png"
# TEMP_FILE="image.bin"
# python3 fromPNG2Bin.py ${INPUT_PNG}
# ./main ${TEMP_FILE}
# python3 fromBin2PNG.py ${TEMP_FILE}.new
for i in {1..18}; do
    INPUT_PNG="image_${i}.png"
    TEMP_FILE="image_${i}.bin"

    echo "Procesando ${INPUT_PNG}..."

    # Convertir la imagen PNG a binario
    python3 fromPNG2Bin.py ${INPUT_PNG}

    # Ejecutar el programa principal
    ./main ${TEMP_FILE}

    # Convertir el binario a una nueva imagen PNG
    python3 fromBin2PNG.py ${TEMP_FILE}.new

    echo "${INPUT_PNG} procesada."
done




