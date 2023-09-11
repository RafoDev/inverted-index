#!/bin/bash

if [ $# -ne 2 ]; then
  echo "Uso: $0 <n> <m>"
  echo "Donde <n> es el número de archivos y <m> es el tamaño en megabytes."
  exit 1
fi

n=$1  
m=$2
base_file="sample.txt"

if [ ! -e "$base_file" ]; then
  echo "El archivo base '$base_file' no existe."
  exit 1
fi

for ((i = 1; i <= n; i++)); do
  output_file="../files/f${i}.txt"
  
  content_size=$(wc -c < "$base_file")
  target_size=$((m * 1024 * 1024))
  copies=$((target_size / content_size))
  
  cat "$base_file" > "$output_file"
  
  echo $copies
  for ((j = 1; j < copies; j++)); do
    cat "$base_file" >> "$output_file"
  done
  
  echo "Archivo '$output_file' creado con un tamaño de ${m} MB."
done

echo "Proceso completado."
