
set( CM_VERSION_GIT "GIT_FAILED" )

# Set here the Version of this project
set( CM_VERSION_MAJOR    0 )
set( CM_VERSION_MINOR    0 )
set( CM_VERSION_SUBMINOR 0 )
set( CM_COMMIT_IS_TAGGED 0 )

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
         OUTPUT_VARIABLE CM_VERSION_GIT
         WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
         RESULT_VARIABLE GIT_RESULT
      )
      
      # git adds a newline (what is bad)
      string( REGEX REPLACE "\n" "" CM_VERSION_GIT ${CM_VERSION_GIT} )
      
      execute_process( 
         COMMAND ${GIT_EXECUTABLE} describe --abbrev=0 --tags
         OUTPUT_VARIABLE GIT_TAG_1
         WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
      )
      
      execute_process( 
         COMMAND ${GIT_EXECUTABLE} describe --tags
         OUTPUT_VARIABLE GIT_TAG_2
         WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
      )
      
      string( REGEX REPLACE "\n" "" GIT_TAG_1 ${GIT_TAG_1} )
      string( REGEX REPLACE "\n" "" GIT_TAG_2 ${GIT_TAG_2} )
      
      string( REGEX REPLACE "^v([0-9]+)[\\.0-9]+$" "\\1" CM_VERSION_MAJOR ${GIT_TAG_1} )
      string( REGEX REPLACE "^v[\\.0-9]+([0-9]+)[\\.0-9]+$" "\\1" CM_VERSION_MINOR ${GIT_TAG_1} )
      string( REGEX REPLACE "^v[\\.0-9]+([0-9]+)$" "\\1" CM_VERSION_SUBMINOR ${GIT_TAG_1} )
         
      if( ${GIT_TAG_1} STREQUAL ${GIT_TAG_2} )
         set( CM_COMMIT_IS_TAGGED 1 )
         message( STATUS "VERSION: ${GIT_TAG_1} [RELEASE] ${CM_VERSION_GIT}" )
      else()
         message( STATUS "VERSION: ${GIT_TAG_1} +GIT ${CM_VERSION_GIT}" )
      endif( ${GIT_TAG_1} STREQUAL ${GIT_TAG_2} )
   
      
      
   endif( ${GIT_FOUND} MATCHES 0 )
endif( UNIX )
