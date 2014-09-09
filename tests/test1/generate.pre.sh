#!/bin/bash

if [ ! -x ../oglTest/oglTestBindings.sh ]; then
   echo "ERROR:      -- Unable to ind ../oglTest/oglTestBindings.sh"
   exit 2
fi

../oglTest/oglTestBindings.sh oglTestBind.hpp oglTestBind.cpp oglTestBind

# kate: indent-mode cstyle; indent-width 3; replace-tabs on; 