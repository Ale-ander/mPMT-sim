cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(mpmt_sim)

option(WITH_GEANT4_UIVIS "Build with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

# We presume the existence of three directories
file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB_RECURSE headers ${PROJECT_SOURCE_DIR}/include/*.hh)
file(GLOB_RECURSE macros RELATIVE ${PROJECT_SOURCE_DIR} macros/*.mac)

# Enable macros for out-of-source build
# foreach(_file ${macros})
#   configure_file(
#     ${_file}
#     ${PROJECT_BINARY_DIR}/${_file}
#     COPYONLY
#     )
# endforeach()
 
add_executable(mpmt_sim main.cc ${sources} ${headers})
target_link_libraries(mpmt_sim ${Geant4_LIBRARIES})

install(TARGETS mpmt_sim DESTINATION bin)
