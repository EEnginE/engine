#!/bin/bash


tests() {
    CMAKE_MIN_V=$(cat CMakeLists.txt | grep cmake_minimum_required | sed 's/[a-zA-Z\_() ]*//g')
    echo -e "cmake_minimum_required(VERSION $CMAKE_MIN_V )\n" > $TESTS_DIR/CMakeLists.txt

    TEST_PROJECTS=$(ls -d $TESTS_DIR/*/)

    for I in $TEST_PROJECTS; do
        I=$( echo $I | sed 's/\/$//g' )
        TEST_BASENAME=$(basename $I)
        echo "INFO: Found test module $TEST_BASENAME"
        echo "add_subdirectory( $TEST_BASENAME )" >> $TESTS_DIR/CMakeLists.txt
        
        if [ ! -f MANUAL_CMAKE ]; then
        
        echo "INFO:    -- Generating its CMakeLists.txt"
cat > ${I}/CMakeLists.txt << EOF
# Automatically generated file; DO NOT EDIT

cmake_minimum_required(VERSION $CMAKE_MIN_V )

set( CSD \${CMAKE_HOME_DIRECTORY}/$I )

testSources()

if( EXISTS \${CSD}/custom.cmake )
   include( \${CSD}/custom.cmake )
endif( EXISTS \${CSD}/custom.cmake )

if( EXISTS \${CSD}/config.in.hpp )
   configure_file("\${CSD}/config.in.hpp" "\${CSD}/config.hpp")
endif( EXISTS \${CSD}/config.in.hpp )

include( sources.cmake )

add_executable( $TEST_BASENAME \${\${CURRENT_BASENAME}_SRC} )

target_link_libraries(
 $TEST_BASENAME

 \${PROJECT_NAME}
 \${ENGINE_LINK}
)

set_target_properties(
 $TEST_BASENAME
 PROPERTIES
  INSTALL_RPATH \${CMAKE_INSTALL_PREFIX}/lib
  INSTALL_RPATH_USE_LINK_PATH TRUE
)


install( TARGETS $TEST_BASENAME RUNTIME DESTINATION \${CMAKE_INSTALL_PREFIX}/bin )

if( EXISTS \${CSD}/data AND IS_DIRECTORY \${CSD}/data )
   install( DIRECTORY \${CSD}/data DESTINATION \${CMAKE_INSTALL_PREFIX}/share/engineTests/$TEST_BASENAME )
endif( EXISTS \${CSD}/data AND IS_DIRECTORY \${CSD}/data )

EOF

        fi
        
        finSources        ${I} ${I}/$SOURCE_FILE $X11 $WAYLAND $MIR $WINDOWS ${TEST_BASENAME^^}
    done    

}