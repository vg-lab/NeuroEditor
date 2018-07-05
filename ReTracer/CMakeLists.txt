
set(UIDIR ${PROJECT_SOURCE_DIR}/ui)
set(QRCDIR ${PROJECT_SOURCE_DIR}/qrc)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

set(RETRACER_HEADERS 
  glmSpline.hpp
  imppsimpl.h
  manipulatedFrameSetConstraint.h
  object.h
  batchbuilder.h
  Utils.h
  nsolRenderer.h
  viewer.h
  TreeItem.h
  TreeModel.h
  #qtskeletonoptimizer.h
  ReTracer.h
)

set(RETRACER_SOURCES 
  manipulatedFrameSetConstraint.cpp
  object.cpp
  nsolRenderer.cpp
  viewer.cpp
  batchbuilder.ui
  Utils.cpp
  #qtskeletonoptimizer.ui
  ReTracer.ui
  batchbuilder.cpp
  #qtskeletonoptimizer.cpp  
  ReTracer.cpp
  TreeItem.cpp
  TreeModel.cpp
  main.cpp
)

set(RETRACER_LINK_LIBRARIES 
  #PyGEmS
  Qt5::Core
  Qt5::Xml
  Qt5::OpenGL
  nsol
  ${QGLVIEWER_LIBRARY}
  ${OPENGL_gl_LIBRARY}
  ${OPENGL_glu_LIBRARY}
  ${GLUT_LIBRARIES}
  ${Boost_LIBRARIES} 
  ${PYTHON_LIBRARIES}  
)

#batchbuilder.ui
#qtskeletonoptimizer.ui

#common_application(ReTracer GUI ${COMMON_APP_ARGS} NOHELP)
common_application(ReTracer GUI NOHELP)

#cmake_minimum_required (VERSION 3.2)
#project (ReTracer)
#
#if(NOT CMAKE_BUILD_TYPE)
#  set(CMAKE_BUILD_TYPE Release)
#endif(NOT CMAKE_BUILD_TYPE)
#
##OpenGL
#find_package(OpenGL REQUIRED)
#include_directories(${OpenGL_INCLUDE_DIRS})
#link_directories(${OpenGL_LIBRARY_DIRS})
#
#find_package(GLUT REQUIRED) 
#include_directories(${GLUT_INCLUDE_DIRS})
#link_directories(${GLUT_LIBRARY_DIRS})
#
##Qt
#find_package (Qt5Widgets)
#find_package (Qt5Core)
#find_package (Qt5Gui)
#find_package (Qt5OpenGL)
#find_package (Qt5Xml)
#
###Boost and Boost-Python
##if ( NOT Boost_FOUND )
##  message(STATUS "This project requires the Boost library, and will not be compiled.")
##  return()  
##endif()
#
#FIND_PACKAGE(PythonInterp)
#FIND_PACKAGE(PythonLibs)
#
#if (${PYTHON_VERSION_MAJOR} EQUAL 3)
#    FIND_PACKAGE(Boost COMPONENTS python3)
#else()
#    FIND_PACKAGE(Boost COMPONENTS python)
#endif()
#
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC ")
#
#include_directories( ../../libs/libSysNeuroUtils )
#include_directories( ../../libs/libNeuroUtils )
#include_directories( ../../libs/libGLNeuroUtils )
#include_directories( ../../libs/libQtNeuroUtils )
#include_directories( ../../libs/libPyGEmS )
#
#INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
#
#file (GLOB skel_src "*.cpp")
#file (GLOB skel_hdr "*.h")
#
#set (skel_ui  ${PROJECT_SOURCE_DIR}/qtskeletonoptimizer.ui)
#set (bb_ui  ${PROJECT_SOURCE_DIR}/batchbuilder.ui)
#
##Apli
#set (MyProjectBin_src ${PROJECT_SOURCE_DIR}/main.cxx)
#
##MOC invocation
#qt5_wrap_cpp(skel_hdr_moc ${skel_hdr})
#qt5_wrap_ui(skel_ui_moc  ${skel_ui})
#qt5_wrap_ui(bb_ui_moc  ${bb_ui})
#
#include_directories (${PROJECT_SOURCE_DIR})
#include_directories (${PROJECT_BINARY_DIR})
#
#include_directories(${QT_INCLUDES} 
#					${CMAKE_CURRENT_BINARY_DIR}
#					${CMAKE_CURRENT_SOURCE_DIR} 
#					)
#
##Lib
#add_library (ReTracerLib SHARED 
#    ${skel_src}
#    ${skel_hdr_moc}
#    ${skel_ui_moc}
#    ${bb_ui_moc}
#    ${UI_RESOURCES_RCC}
#)
#
#target_link_libraries ( ReTracerLib	SysNeuroUtils
#								NeuroUtils
#								GLNeuroUtils 
#								QtNeuroUtils 
#                PyGEmS                
#								OpenMeshCore 
#								OpenMeshTools
#								QGLViewer
#								Qt5::Widgets
#								Qt5::Xml
#								Qt5::OpenGL								
#								${OPENGL_LIBRARIES} 
#								${GLUT_LIBRARY}
#                ${Boost_LIBRARIES} 
#                ${PYTHON_LIBRARIES}
#						)
#
##Apli
#add_executable(ReTracer ${MyProjectBin_src} )
#target_link_libraries (ReTracer ReTracerLib)
#
#set_target_properties( ReTracerLib
#  PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
#
#set_target_properties( ReTracer
#  PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
