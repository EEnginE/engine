#!/bin/bash
OBJFILES=./data/*.obj

if [ ! -x ../oglTest/oglTestBindings.sh ]; then
   echo "ERROR:      -- Unable to find ../oglTest/oglTestBindings.sh"
   exit 2
fi

for f in $OBJFILES
do
   echo "INFO:      -- Processing on $f"
   ../../tools/objParse.awk $f
done

echo "INFO:      -- Generating oglTestBind.hpp and oglTestBind.cpp"
../oglTest/oglTestBindings.sh oglTestBind.hpp oglTestBind.cpp oglTestBind

# kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
