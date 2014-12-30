#!/bin/bash
OBJFILES=(./data/obj_raw/*.obj)

if [ ! -x ../oglTest/oglTestBindings.sh ]; then
   error "     -- Unable to find ../oglTest/oglTestBindings.sh"
   exit 2
fi


TOOLSPATH=$ENGINE_ROOT
if [ -z "$ENGINE_ROOT" ]; then
   warning "   -- ENGINE_ROOT not set. Fallback to relative paths."
   TOOLSPATH="../.."
   export ENGINE_ROOT=$(readlink -m ../../)
fi

if [ ! -d $(pwd)/data/obj ]; then
   mkdir -p $(pwd)/data/obj
fi

COUNTER=1
msg3 "Processing OBJs..."

for f in ${OBJFILES[@]}; do
   processBar $COUNTER ${#OBJFILES[@]} $f
   ((COUNTER++))
   ${TOOLSPATH}/tools/objParsePrint.awk $(readlink -m $f) > $(pwd)/data/obj/$(basename $(readlink -m $f))
done

msg3 "Generating oglTestBind.hpp and oglTestBind.cpp"
../oglTest/oglTestBindings.sh oglTestBind.hpp oglTestBind.cpp oglTestBind

# kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
