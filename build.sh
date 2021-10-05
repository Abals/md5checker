#!/bin/bash

PROJ_C_COMPILER=/usr/bin/gcc
PROJ_CPP_COMPILER=/usr/bin/g++
PROJ_GENERATOR_NAME="Unix Makefiles"

ARRAY_BUILD_TYPE=("Debug" "Release")

for i in "${ARRAY_BUILD_TYPE[@]}"
do
  echo $i
  cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=${i} -DCMAKE_C_COMPILER=${PROJ_C_COMPILER} -DCMAKE_CXX_COMPILER=${PROJ_CPP_COMPILER} -H./ -B./build -G "${PROJ_GENERATOR_NAME}"
  cmake --build ./build --config ${i} --target clean --
  cmake --build ./build --config ${i} --target all -- -j 4 #2>&1 | tee CMAKE_${i}.log
  if [ $? -ne 0 ]
  then
    echo "Build failed.";
    exit 1;
  fi
done
