#!/bin/bash

# FIXME: checar se há todas as ferramentas instaladas
# FIXME: PQP! que script porco. Vc já fez melhor que isso :-(

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug && make

echo O compilador se encontra em build/experiments/language
