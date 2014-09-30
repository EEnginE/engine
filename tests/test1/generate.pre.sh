#!/bin/bash

if [ ! -x ../oglTest/oglTestBindings.sh ]; then
   echo "ERROR:      -- Unable to ind ../oglTest/oglTestBindings.sh"
   exit 2
fi

echo "INFO:      -- Generating oglTestBind.hpp and oglTestBind.cpp"
../oglTest/oglTestBindings.sh oglTestBind.hpp oglTestBind.cpp oglTestBind

echo "INFO:      -- Generating data/mesh_f.obj"
../../tools/objFaceShift.awk -1 ./data/mesh.obj  | ../../tools/objScale.awk 0.675 > ./data/mesh_f.obj

echo "INFO:      -- Generating data/mesh2_f.obj"
../../tools/objFaceShift.awk -1 ./data/mesh2.obj | ../../tools/objScale.awk 0.685 > ./data/mesh2_f.obj

# kate: indent-mode cstyle; indent-width 3; replace-tabs on; 