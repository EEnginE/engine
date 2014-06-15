
macro( testSources )
    if( NOT EXISTS ${PROJECT_SOURCE_DIR}/init.cmake )
        message( STATUS "Can not find init.cmake ==> Running ${PROJECT_SOURCE_DIR}/generate.sh" )
        execute_process( COMMAND ${PROJECT_SOURCE_DIR}/generate.sh )
    endif( NOT EXISTS ${PROJECT_SOURCE_DIR}/init.cmake )
    
    if( NOT EXISTS ${PROJECT_SOURCE_DIR}/render.cmake )
        message( STATUS "Can not find render.cmake ==> Running ${PROJECT_SOURCE_DIR}/generate.sh" )
        execute_process( COMMAND ${PROJECT_SOURCE_DIR}/generate.sh )
    endif( NOT EXISTS ${PROJECT_SOURCE_DIR}/render.cmake )
    
    if( NOT EXISTS ${PROJECT_SOURCE_DIR}/utils.cmake )
        message( STATUS "Can not find utils.cmake ==> Running ${PROJECT_SOURCE_DIR}/generate.sh" )
        execute_process( COMMAND ${PROJECT_SOURCE_DIR}/generate.sh )
    endif( NOT EXISTS ${PROJECT_SOURCE_DIR}/utils.cmake )
    
    include( ${PROJECT_SOURCE_DIR}/init.cmake )

    foreach( TO_TEST ${${CURRENT_BASENAME}_SRC} )
        if( NOT EXISTS ${TO_TEST} )
            message( STATUS "Can not find ${TO_TEST} ==> Running ${PROJECT_SOURCE_DIR}/generate.sh" )
            execute_process( COMMAND ${PROJECT_SOURCE_DIR}/generate.sh )
            break()
        endif( NOT EXISTS ${TO_TEST} )
    endforeach( TO_TEST ${${CURRENT_BASENAME}_SRC} )
        
    foreach( TO_TEST ${${CURRENT_BASENAME}_INC} )
        if( NOT EXISTS ${TO_TEST} )
            message( STATUS "Can not find ${TO_TEST} ==> Running ${PROJECT_SOURCE_DIR}/generate.sh" )
            execute_process( COMMAND ${PROJECT_SOURCE_DIR}/generate.sh )
            break()
        endif( NOT EXISTS ${TO_TEST} )
    endforeach( TO_TEST ${${CURRENT_BASENAME}_INC} )
    
    
    include( ${PROJECT_SOURCE_DIR}/utils.cmake )
                
    foreach( TO_TEST ${${CURRENT_BASENAME}_SRC} )
        if( NOT EXISTS ${TO_TEST} )
            message( STATUS "Can not find ${TO_TEST} ==> Running ${PROJECT_SOURCE_DIR}/generate.sh" )
            execute_process( COMMAND ${PROJECT_SOURCE_DIR}/generate.sh )
            break()
        endif( NOT EXISTS ${TO_TEST} )
    endforeach( TO_TEST ${${CURRENT_BASENAME}_SRC} )
        
    foreach( TO_TEST ${${CURRENT_BASENAME}_INC} )
        if( NOT EXISTS ${TO_TEST} )
            message( STATUS "Can not find ${TO_TEST} ==> Running ${PROJECT_SOURCE_DIR}/generate.sh" )
            execute_process( COMMAND ${PROJECT_SOURCE_DIR}/generate.sh )
            break()
        endif( NOT EXISTS ${TO_TEST} )
    endforeach( TO_TEST ${${CURRENT_BASENAME}_INC} )
    
    if( NOT EXISTS ${PROJECT_SOURCE_DIR}/utils/log/uMacros.hpp )
        message( STATUS "Can not find ${PROJECT_SOURCE_DIR}/utils/log/uMacros.hpp ==> Running ${PROJECT_SOURCE_DIR}/generate.sh" )
        execute_process( COMMAND ${PROJECT_SOURCE_DIR}/generate.sh )
    endif( NOT EXISTS ${PROJECT_SOURCE_DIR}/utils/log/uMacros.hpp )
    
endmacro( testSources )