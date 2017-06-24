cmake_minimum_required (VERSION 3.3)
project ( rfl )

include_directories( . )

link_libraries( pthread )
link_libraries( z  )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11 -Wall")

set( RTL_SOURCES rfl.c global.c array.c )

if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	add_definitions( -DDEBUG_BUILD )
endif()

add_executable( rfl ${RTL_SOURCES} )
