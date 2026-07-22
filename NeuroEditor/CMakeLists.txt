
# Define headers
set(NEUROEDITOR_HEADERS
  glmSpline.hpp
  imppsimpl.h
  manipulatedFrameSetConstraint.h
  object.h
  Utils.h
  nsolRenderer.h
  Scene.h
  viewer.h
  TreeItem.h
  TreeModel.h
  NeuroEditor.h
  ViewDock.h
  SelectDock.h
  EditDock.h
  CorrectDock.h
  Fixer.h
  Tester.h
  TraceModifier.h
  ColorSelectionWidget.h
  SimplificationHelpDialog.h
)

# Define source files
set(NEUROEDITOR_SOURCES
  resources.qrc
  manipulatedFrameSetConstraint.cpp
  object.cpp
  nsolRenderer.cpp
  Scene.cpp
  viewer.cpp
  Utils.cpp
  NeuroEditor.ui
  NeuroEditor.cpp
  TreeItem.cpp
  TreeModel.cpp
  main.cpp
  ViewDock.cpp
  SelectDock.cpp
  EditDock.cpp
  CorrectDock.cpp
  Fixer.cpp
  Tester.cpp
  TraceModifier.cpp
  ColorSelectionWidget.cpp
  SimplificationHelpDialog.cpp
)

# # Create the includes strings
# set(HEADER_INCLUDES "")
# foreach(header ${NEUROEDITOR_PUBLIC_HEADERS})
#   string(APPEND HEADER_INCLUDES "#include <${PROJECT_NAME_LOWER}/${header}>\n")
# endforeach()

# # Generate the unified header
# configure_file(
#   "${PROJECT_SOURCE_DIR}/cmake/unified_header.h.in"
#   "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}.h"
# )

# Get the current Git commit for the version header
find_package(Git)
if(Git_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} --no-pager log -n 1 HEAD "--pretty=format:%h"
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    RESULT_VARIABLE git_result
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(git_result EQUAL 0)
    message(STATUS "Last ${PROJECT_NAME} Git commit: ${GIT_COMMIT_HASH}")
  endif()
endif()

# Generate the version header
configure_file (
  "${PROJECT_SOURCE_DIR}/cmake/version.h.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}/version.h"
)

# Enable Qt generators
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Create the target
add_executable(${PROJECT_NAME})

# set_target_properties(${PROJECT_NAME} PROPERTIES
#   VERSION ${PROJECT_VERSION}
#   SOVERSION ${PROJECT_VERSION_MAJOR}
# )

# Add sources to the target
target_sources(${PROJECT_NAME}
  PRIVATE
    ${NEUROEDITOR_SOURCES}
  PRIVATE
    FILE_SET public_headers
      TYPE HEADERS
      BASE_DIRS
        ${PROJECT_SOURCE_DIR}
      FILES
        ${NEUROEDITOR_HEADERS}
)

# Add generated sources to the target
target_sources(${PROJECT_NAME}
  PRIVATE
    FILE_SET public_headers_generated
      TYPE HEADERS
      BASE_DIRS
        ${PROJECT_BINARY_DIR}
      FILES
        # Generated headers
        # ${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}/${PROJECT_NAME_LOWER}.h
        # ${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}/${PROJECT_NAME_LOWER}_export.h
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}/version.h
)

# Add compile definitions 
target_compile_definitions(${PROJECT_NAME}
  PUBLIC
  PRIVATE
    QT_NO_KEYWORDS
    # DEFAULT_CONTEXT_OPENGL_MAJOR=${DEFAULT_CONTEXT_OPENGL_MAJOR}
    # DEFAULT_CONTEXT_OPENGL_MINOR=${DEFAULT_CONTEXT_OPENGL_MINOR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
  PUBLIC
    Qt5::Core
    Qt5::Widgets
    Qt5::OpenGL
    Qt5::Xml
    neurolots::nlgeometry
    neurolots::nlrender
    neurolots::nlgenerator
    nsol::nsol
    OpenGL::GL
    OpenGL::GLU
    QGLViewer
    pygems::pygems
    # GLUT::GLUT
    # GLEW::GLEW
    # ${QGLVIEWER_LIBRARY}
    # Eigen3::Eigen
  PRIVATE
)
