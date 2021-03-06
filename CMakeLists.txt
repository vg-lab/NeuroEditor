cmake_minimum_required (VERSION 3.2)
project (NeuroEditor VERSION 0.0.1)
set( NeuroEditor_VERSION_ABI 1 )

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC ")

if( "${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}" )
 message( FATAL "no in source building allowed." )
endif( )

list( APPEND CMAKE_MODULE_PATH
 ${PROJECT_SOURCE_DIR}/CMake
 ${CMAKE_SOURCE_DIR}/CMake/common )

if( NOT EXISTS ${CMAKE_SOURCE_DIR}/CMake/common/Common.cmake )
 message( FATAL_ERROR "CMake/common missing, run: git submodule update --init")
endif( )

include( GitExternal )

set( NEUROEDITOR_DESCRIPTION "NeuroEditor - Editor of neuro morphological tracings." )
set( NEUROEDITOR_LICENSE LGPL )
set( NEUROEDITOR_MAINTAINER "Juan Pedro Brito <juanpedro.brito@upm.es>" )
set( COMMON_PROJECT_DOMAIN es.gmrv )

include( Common )

add_definitions(-DQT_NO_KEYWORDS)

common_find_package(Qt5Widgets SYSTEM REQUIRED)
common_find_package(Qt5Core SYSTEM REQUIRED)
common_find_package(Qt5Gui SYSTEM REQUIRED)
common_find_package(OpenGL SYSTEM REQUIRED)
common_find_package(GLUT SYSTEM REQUIRED)
common_find_package(GLEW SYSTEM REQUIRED)
common_find_package(QGLViewer SYSTEM REQUIRED)
common_find_package(nsol REQUIRED)
common_find_package(neurolots REQUIRED)
common_find_package(Eigen3 REQUIRED SYSTEM)
common_find_package(pygems REQUIRED)

list(APPEND NEUROEDITOR_DEPENDENT_LIBRARIES Qt5Core Qt5Widget Qt5Gui GLUT GLEW
        OpenGL nsol neurolots Eigen3 pygems)

common_find_package_post( )


include_directories(
        ${Boost_INCLUDE_DIRS}
        ${PYTHON_INCLUDE_DIRS}
)

add_definitions( -DPYGEMS_USE_PYTHON3 )
add_subdirectory( NeuroEditor )

file(GLOB ICONS "NeuroEditor/icons/*")
file(COPY ${ICONS} DESTINATION "${CMAKE_BINARY_DIR}/bin/icons")

#if ( ${USE_BOOST_PYTHON_VERSION} EQUAL 3 )
#  file(COPY "${COMMON_SOURCE_DIR}/PyGEmS/scripts/Strategies-Python3.py" DESTINATION "${CMAKE_BINARY_DIR}/bin")
#endif( )

set(DOCS README.md LICENSE.txt)
install(FILES ${DOCS} DESTINATION share/NeuroEditor COMPONENT dev)

set(DOXYGEN_MAINPAGE_MD README.md)
set(DOXYGEN_EXTRA_INPUT ${PROJECT_SOURCE_DIR}/README.md)
include(DoxygenRule)

include(CPackConfig)
include(CTest)
