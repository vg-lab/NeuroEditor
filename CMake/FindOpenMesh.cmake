find_path(_openMesh_INCLUDE_DIR OpenMesh/Core/Geometry/Config.hh
  HINTS ${OPENMESH_ROOT}/include $ENV{OPENMESH_ROOT}/include
  PATHS /usr/include /usr/local/include /opt/local/include)

find_library(_openMesh_CORE_LIBRARY NAMES OpenMeshCore
  HINTS ${OPENMESH_ROOT}/lib $ENV{OPENMESH_ROOT}/lib
  PATHS /usr/lib/OpenMesh /usr/local/lib/OpenMesh /opt/local/lib/OpenMesh)

find_library(_openMesh_TOOLS_LIBRARY NAMES OpenMeshTools
  HINTS ${OPENMESH_ROOT}/lib $ENV{OPENMESH_ROOT}/lib
  PATHS /usr/lib/OpenMesh /usr/local/lib/OpenMesh /opt/local/lib/OpenMesh)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenMesh
  "OpenMesh NOT FOUND. Specific location can be provided by OPENMESH_ROOT (via cmake or env. var)"
  _openMesh_INCLUDE_DIR _openMesh_CORE_LIBRARY _openMesh_TOOLS_LIBRARY)

set(OPENMESH_INCLUDE_DIRS ${_openMesh_INCLUDE_DIR})
set(OPENMESH_LIBRARIES ${_openMesh_CORE_LIBRARY} ${_openMesh_TOOLS_LIBRARY})
if(OPENMESH_FOUND AND NOT OPENMESH_FIND_QUIETLY)
  message(STATUS "Found OpenMesh in ${OPENMESH_INCLUDE_DIRS};${OPENMESH_LIBRARIES}")
endif()
