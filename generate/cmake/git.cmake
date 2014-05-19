
set( CM_CURRENT_COMMIT "GIT_FAILED" )

if( UNIX )
   execute_process( 
      COMMAND which git
      OUTPUT_VARIABLE GIT_EXECUTABLE
      RESULT_VARIABLE GIT_FOUND
   )
      
   if( ${GIT_FOUND} MATCHES 0 )
      # which adds a newline (what is bad)
      string( REGEX REPLACE "\n" "" GIT_EXECUTABLE ${GIT_EXECUTABLE} )

      execute_process( 
         COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
         OUTPUT_VARIABLE CM_CURRENT_COMMIT
         WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
         RESULT_VARIABLE GIT_RESULT
      )
      if( NOT ${GIT_RESULT} MATCHES 0 )
         set( CM_CURRENT_COMMIT "GIT_FAILED" )
      else( NOT ${GIT_RESULT} MATCHES 0 )
         # git adds a newline (what is bad)
         string( REGEX REPLACE "\n" "" CM_CURRENT_COMMIT ${CM_CURRENT_COMMIT} )
      endif( NOT ${GIT_RESULT} MATCHES 0 )
   endif( ${GIT_FOUND} MATCHES 0 )
endif( UNIX )
