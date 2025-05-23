# CMakeLists.txt KEBI project.
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(KEBI)

#---Configuration
set(ACTIVATE_EVE      ON  CACHE INTERNAL "") # Activate Root Eve package
set(CREATE_GIT_LOG    ON  CACHE INTERNAL "") # Create Git log
set(BUILD_GEANT4_SIM  ON  CACHE INTERNAL "") # Build Geant4 simulation
set(BUILD_DOXYGEN_DOC OFF CACHE INTERNAL "") # Build Doxygen documentation with HTML
set(KEBI_PROJECT_LIST     CACHE INTERNAL "") # List of KEBI projects to build

#---Configuration
include(configure.cmake)

if(ACTIVATE_EVE)
  add_definitions(-DACTIVATE_EVE)
endif(ACTIVATE_EVE)

message(STATUS "Activate Eve package   : " ${ACTIVATE_EVE})
message(STATUS "Create Git log         : " ${CREATE_GIT_LOG})
message(STATUS "Build Geant4 simulation: " ${BUILD_GEANT4_SIM})
message(STATUS "Build Doxygen document : " ${BUILD_DOXYGEN_DOC})

#---Add source directories. Directory should be under "/path/to/KEBI/"
set(KEBI_SOURCE_DIRECTORY_LIST
  ${CMAKE_SOURCE_DIR}/source/fit
  ${CMAKE_SOURCE_DIR}/source/tool
  ${CMAKE_SOURCE_DIR}/source/geometry
  ${CMAKE_SOURCE_DIR}/source/base
  ${CMAKE_SOURCE_DIR}/source/format
  ${CMAKE_SOURCE_DIR}/source/tracklet
  ${CMAKE_SOURCE_DIR}/source/detector
  ${CMAKE_SOURCE_DIR}/source/reco/PSA
  ${CMAKE_SOURCE_DIR}/source/reco/TrackFinder
  ${CMAKE_SOURCE_DIR}/source/analysis/PlotChannels
  ${CMAKE_SOURCE_DIR}/GETDecoder/source/GETCore
  ${CMAKE_SOURCE_DIR}/GETDecoder/source/GETFormat
  ${CMAKE_SOURCE_DIR}/GETDecoder/source/GETUtil
  CACHE INTERNAL ""
)

#---Add geant4 source directories. Directory should be under "/path/to/KEBI/".
set(KEBI_GEANT4_DIRECTORY_LIST
  ${CMAKE_SOURCE_DIR}/source/geant4
  CACHE INTERNAL ""
)

#---Add geant4 main programs. Directory should be under "/path/to/KEBI/"
set(KEBI_GEANT4_EXECUTABLE_LIST
  CACHE INTERNAL ""
)

foreach(_kbproject ${KEBI_PROJECT_LIST})
  message(STATUS "Project directory: " ${_kbproject})
  add_subdirectory(${_kbproject})
endforeach(_kbproject)


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
#=====================================================================================
# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
#---libs: Core RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread MultiProc pthread
#---evelibs: Eve EG TreePlayer Geom Ged RGL Gui Core RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread MultiProc pthread
if(ACTIVATE_EVE)
  # find_package(ROOT REQUIRED)
  find_package(ROOT REQUIRED COMPONENTS RIO Net Eve EG RGL Geom Gui Graf Graf3d Ged Matrix MathCore)
else(ACTIVATE_EVE)
  # find_package(ROOT REQUIRED)
  find_package(ROOT REQUIRED COMPONENTS RIO Net Geom Gui Graf Graf3d Ged Matrix MathCore)

endif(ACTIVATE_EVE)
include(${ROOT_USE_FILE})

set(TOOLS_LIBRARIES_LIST ${ROOT_LIBRARIES} CACHE INTERNAL "")

#find_package(Boost   REQUIRED)
#Set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${Boost_LIBRARY_DIR})

#find_package(Eigen3  REQUIRED)
#find_package(Rave    REQUIRED)
#find_package(GENFIT2 REQUIRED)

#include_directories(${EIGEN3_INCLUDE_DIR})
#include_directories(${GENFIT2_INCLUDE_DIR})

#set(LINK_DIRECTORIES
#  ${ROOT_LIBRARY_DIR}
  #  ${RAVE_LIBRARY_DIR}
  # ${GENFIT2_LIBRARY_DIR}
  #)

link_directories(${LINK_DIRECTORIES})

if(BUILD_GEANT4_SIM)
  find_package(Geant4 REQUIRED ui_all vis_all)
  include(${Geant4_USE_FILE})
endif(BUILD_GEANT4_SIM)

foreach(_directory ${KEBI_SOURCE_DIRECTORY_LIST})
  include_directories(${_directory})
  if(BUILD_GEANT4_SIM)
    foreach(_g4directory ${KEBI_GEANT4_DIRECTORY_LIST})
      include_directories(${_g4directory})
    endforeach(_g4directory)
  endif(BUILD_GEANT4_SIM)

  string(REPLACE / _ _name ${_directory})
  list(APPEND KEBI_SOURCE_NAME_LIST ${_name})
  set(KEBI_${_name}_G_NAME G__${_name}Dict)
  file(GLOB KEBI_${_name}_LINKDEF_FILE ${_directory}/LinkDef.h)
  file(GLOB KEBI_${_name}_SOURCE_FILES ${_directory}/*.cc)
  file(GLOB KEBI_${_name}_HEADER_FILES ${_directory}/*.hh)
endforeach(_directory)

include_directories(${CMAKE_SOURCE_DIR}/log)

add_definitions(${ROOT_CXX_FLAGS})

foreach(_name ${KEBI_SOURCE_NAME_LIST})
  ROOT_GENERATE_DICTIONARY(${KEBI_${_name}_G_NAME}
    ${KEBI_${_name}_HEADER_FILES}
    LINKDEF ${KEBI_${_name}_LINKDEF_FILE}
  )
endforeach(_name)

foreach(_name ${KEBI_SOURCE_NAME_LIST})
  foreach(_file ${KEBI_${_name}_SOURCE_FILES})
    list(APPEND KEBI_LIB_LIST ${_file})
  endforeach(_file)
  list(APPEND KEBI_LIB_LIST ${KEBI_${_name}_G_NAME}.cxx)
endforeach(_name)

add_library(KEBI SHARED ${KEBI_LIB_LIST})
target_link_libraries(KEBI ${TOOLS_LIBRARIES_LIST})

if(BUILD_GEANT4_SIM)
  foreach(_directory ${KEBI_GEANT4_DIRECTORY_LIST})
    set(KEBI_GEANT4_SOURCE_FILES_TEMP)
    file(GLOB KEBI_GEANT4_SOURCE_FILES_TEMP ${_directory}/*.cc)
    foreach(_file ${KEBI_GEANT4_SOURCE_FILES_TEMP})
      list(APPEND KEBI_GEANT4_SOURCE_FILES ${_file})
    endforeach(_file)

    set(KEBI_GEANT4_HEADER_FILES_TEMP)
    file(GLOB KEBI_GEANT4_HEADER_FILES_TEMP ${_directory}/*.hh)
    foreach(_file ${KEBI_GEANT4_HEADER_FILES_TEMP})
      list(APPEND KEBI_GEANT4_HEADER_FILES ${_file})
    endforeach(_file)
  endforeach(_directory)

  foreach(_g4sim ${KEBI_GEANT4_EXECUTABLE_LIST})
    get_filename_component(_g4sim_name ${_g4sim} NAME)
    get_filename_component(_g4sim_directory ${_g4sim} DIRECTORY)
    string(REPLACE .cc "" _executable_head ${_g4sim_name})
    string(REPLACE .cc .g4sim _executable_name ${_g4sim_name})

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${_g4sim_directory})
    add_executable(${_executable_name} ${_g4sim} ${KEBI_GEANT4_SOURCE_FILES} ${KEBI_GEANT4_HEADER_FILES})
    execute_process(COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(CMAKE_EXE_LINKER_FLAGS ${ROOT_LD_FLAGS})
    target_link_libraries(${_executable_name} ${Geant4_LIBRARIES} KEBI)
  endforeach(_g4sim)
endif(BUILD_GEANT4_SIM)

set(KEBI_PATH ${CMAKE_SOURCE_DIR})
configure_file(${KEBI_PATH}/log/KBLog.cmake.in ${KEBI_PATH}/log/KBLog.cmake @ONLY)
add_custom_target(KEBI_LOG ALL
                  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/log/KBLog.cmake
                  VERBATIM)

add_dependencies(KEBI KEBI_LOG)

file(MAKE_DIRECTORY ${KEBI_PATH}/data)
