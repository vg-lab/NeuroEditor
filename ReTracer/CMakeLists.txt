
set(UIDIR ${PROJECT_SOURCE_DIR}/ui)
set(QRCDIR ${PROJECT_SOURCE_DIR}/qrc)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(RETRACER_HEADERS
  glmSpline.hpp
  imppsimpl.h
  manipulatedFrameSetConstraint.h
  object.h
  batchbuilder.h
  Utils.h
  nsolRenderer.h
  Scene.h
  viewer.h
  TreeItem.h
  TreeModel.h
  ReTracer.h
  ViewDock.h
  SelectDock.h
  EditDock.h
  CorrectDock.h
  SimplifyDock.h
  Fixer.h
  Tester.h
  TraceModifier.h
  ColorSelectionWidget.h
)

set(RETRACER_SOURCES
  resources.qrc
  manipulatedFrameSetConstraint.cpp
  object.cpp
  nsolRenderer.cpp
  Scene.cpp
  viewer.cpp
  batchbuilder.ui
  Utils.cpp
  ReTracer.ui
  batchbuilder.cpp
  ReTracer.cpp
  TreeItem.cpp
  TreeModel.cpp
  main.cpp
  ViewDock.cpp
  SelectDock.cpp
  EditDock.cpp
  CorrectDock.cpp
  SimplifyDock.cpp
  Fixer.cpp
  Tester.cpp
  TraceModifier.cpp
  ColorSelectionWidget.cpp
)

set(RETRACER_LINK_LIBRARIES
  Qt5::Core
  Qt5::Xml
  Qt5::OpenGL
  ${QGLVIEWER_LIBRARY}
  ${OPENGL_gl_LIBRARY}
  ${OPENGL_glu_LIBRARY}
  ${GLUT_LIBRARIES}
  ${Boost_LIBRARIES}
  ${PYTHON_LIBRARIES}
  nsol
  nlgeometry
  nlgenerator
  nlrender
  PyGEmS
  PyGEmSManager
)

common_application(ReTracer GUI NOHELP)
