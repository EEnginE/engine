#!/bin/bash
OBJFILES=./data/*.obj

if [ ! -x ../oglTest/oglTestBindings.sh ]; then
   echo "ERROR:     -- Unable to find ../oglTest/oglTestBindings.sh"
   exit 2
fi


TOOLSPATH=$ENGINE_ROOT
if [ -z "$ENGINE_ROOT" ]; then
   echo "WARNING:   -- ENGINE_ROOT not set. Fallback to relative paths."
   TOOLSPATH="../.."
   export ENGINE_ROOT=$(readlink -m ../../)
fi

for f in $OBJFILES
do
   echo "INFO:      -- Processing on $f"
   ${TOOLSPATH}/tools/objParse.awk $(readlink -m $f)
done

echo "INFO:      -- Generating oglTestBind.hpp and oglTestBind.cpp"
../oglTest/oglTestBindings.sh oglTestBind.hpp oglTestBind.cpp oglTestBind

# kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
