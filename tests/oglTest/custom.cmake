set( CSD ${CMAKE_HOME_DIRECTORY}/tests/oglTest )

if( EXISTS ${CSD}/oglTestBindings.sh )
   install( PROGRAMS ${CSD}/oglTestBindings.sh DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
endif( EXISTS ${CSD}/oglTestBindings.sh )