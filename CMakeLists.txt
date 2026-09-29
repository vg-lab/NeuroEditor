cmake_minimum_required(VERSION 3.25)

project(NeuroEditor
  VERSION 1.0.0
  LANGUAGES CXX
  DESCRIPTION "Neuro morphological tracings editor"
)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

# Prevent in-source builds
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# Include custom modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Options
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(BUILD_DOCS "Build documentation" ON)
# option(NEUROEDITOR_WITH_EXAMPLES "Build examples" ON)
# option(NEUROEDITOR_WITH_TESTS "Build tests" ON)
option(NEUROEDITOR_OPTIONALS_AS_REQUIRED "Force optional dependencies as required" OFF)

# Dependencies
# find_package(Boost CONFIG COMPONENTS unit_test_framework)
find_package(Qt5 REQUIRED CONFIG COMPONENTS Core Gui Widgets OpenGL Xml)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)

include(FetchContent)

FetchContent_Declare(
    pygems
    GIT_REPOSITORY https://github.com/vg-lab/pygems.git
    GIT_TAG bdb06a56d2d400bd9872bfc6656210ef3b0ac265 # v1.0.0
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(pygems)

option(NSOL_USE_BRION "" OFF)
FetchContent_Declare(
    nsol
    GIT_REPOSITORY https://github.com/vg-lab/nsol.git
    GIT_TAG d5ca1e20592847b9475c277d7cc79ff915bc99d2 # v1.0.0
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(nsol)

FetchContent_Declare(
    neurolots
    GIT_REPOSITORY https://github.com/vg-lab/neurolots.git
    GIT_TAG b8b21e78bd9df2a12fd50433348d7e95a8ce8e25 # v1.0.0
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(neurolots)

FetchContent_Declare(
    libQGLViewer
    GIT_REPOSITORY https://github.com/GillesDebunne/libQGLViewer.git
    GIT_TAG c8bb8810e70f0c902cbf5c222ee6fc1e4cd11d34 # v3.0.0
    GIT_SUBMODULES_RECURSE OFF
)
FetchContent_MakeAvailable(libQGLViewer)

# Optional dependencies
set(NEUROEDITOR_OPTS_FIND_ARGS "")
if(NEUROEDITOR_OPTIONALS_AS_REQUIRED)
  set(NEUROEDITOR_OPTS_FIND_ARGS REQUIRED)
endif()

# find_package(Foo ${NEUROEDITOR_OPTS_FIND_ARGS} QUIET)

# Definitions
# option(NEUROEDITOR_USE_Foo "" ${Foo_FOUND})

# Add subdirectories
add_subdirectory(${PROJECT_NAME})

# Define installation directory variables
include(GNUInstallDirs)

# Create an export
install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Generate documentation
find_package(Doxygen)
if(DOXYGEN_FOUND AND BUILD_DOCS)
  set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs_doxygen)
  set(DOXYFILE_IN ${PROJECT_SOURCE_DIR}/cmake/Doxyfile.in)
  set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

  doxygen_add_docs(
    ${PROJECT_NAME}_doxygen
    CONFIG_FILE ${DOXYFILE_OUT}
  )

  install(DIRECTORY
    ${DOXYGEN_OUTPUT_DIR}/html
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    COMPONENT ${PROJECT_NAME}_documentation
    OPTIONAL
  )
endif()

# Packaging
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
  /.git
  /.cache
  /build
  /.DS_Store
  /.idea
  /.vscode
)
include(CPack)
