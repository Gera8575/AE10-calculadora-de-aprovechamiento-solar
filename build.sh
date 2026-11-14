#!/usr/bin/env bash
# Compila el .c ORIGINAL a WebAssembly y arma la carpeta _site/ lista para publicar.
set -euo pipefail
SRC="AE10_Calculadora_potencial_solar.c"   # <- nombre de tu archivo .c

rm -rf _site && mkdir -p _site
emcc "$SRC" web_shim.c -O2 \
  -sASYNCIFY -sEXIT_RUNTIME=1 -sENVIRONMENT=web \
  -sMODULARIZE=1 -sEXPORT_NAME=crearPrograma \
  -Wl,--wrap=scanf -Wl,--wrap=usleep \
  -o _site/programa.js
cp index.html "$SRC" _site/
echo "Listo: prueba con  python3 -m http.server -d _site"
