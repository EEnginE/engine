#!/bin/bash

installTools() {
    TOOLS_DIR=$1
    OUT_INSTALL_TOOLS=$2
    
    echo "INFO: Generating file $OUT_INSTALL_TOOLS for tools to install..."

cat > $OUT_INSTALL_TOOLS << EOF
# WARNING automatically generated file

install(
   PROGRAMS
EOF

    for I in $(ls $TOOLS_DIR); do
        if [ ! -f $TOOLS_DIR/$I ]; then
            continue
        fi
        echo "INFO:    -- Found tool: $I"
        echo "      \${$VAR_FOR_CMAKE_ROOT}/$TOOLS_DIR/$I" >> $OUT_INSTALL_TOOLS
    done

cat >> $OUT_INSTALL_TOOLS << 'EOF'
   DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

EOF
}

# kate: indent-mode shell; indent-width 4; replace-tabs on;