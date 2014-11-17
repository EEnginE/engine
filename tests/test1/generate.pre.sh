#!/bin/bash

if [ ! -x ../oglTest/oglTestBindings.sh ]; then
   echo "ERROR:      -- Unable to ind ../oglTest/oglTestBindings.sh"
   exit 2
fi

echo "INFO:      -- Generating oglTestBind.hpp and oglTestBind.cpp"
../oglTest/oglTestBindings.sh oglTestBind.hpp oglTestBind.cpp oglTestBind

echo "INFO:      -- Generating data/mesh_f.obj"
../../tools/objFaceShift.awk -1 ./data/mesh.obj           | ../../tools/objScale.awk 0.675 > ./data/mesh_f.obj

echo "INFO:      -- Generating data/mesh2_f.obj"
../../tools/objFaceShift.awk -1 ./data/mesh2.obj          | ../../tools/objScale.awk 0.685 > ./data/mesh2_f.obj

echo "INFO:      -- Generating data/e-engine_f.obj"
../../tools/objFaceShift.awk -1 ./data/e-engine.obj       | ../../tools/objScale.awk 0.685 > ./data/e-engine_f.obj

echo "INFO:      -- Generating data/sphere_f.obj"
../../tools/objFaceShift.awk -1 ./data/sphere.obj         | ../../tools/objScale.awk 1.25  > ./data/sphere_f.obj

echo "INFO:      -- Generating data/town_f.obj"
../../tools/objFaceShift.awk -1 ./data/town.obj           | ../../tools/objScale.awk 1.5   | ../../tools/objMove.awk 0 -0.75 0 > ./data/town_f.obj

echo "INFO:      -- Generating data/blender_monkey_f.obj"
../../tools/objFaceShift.awk -1 ./data/blender_monkey.obj | ../../tools/objScale.awk 0.9   > ./data/blender_monkey_f.obj

# kate: indent-mode cstyle; indent-width 3; replace-tabs on; 