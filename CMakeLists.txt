cmake_minimum_required(VERSION 3.28)

project(youbot_lidar_nav)

include(GNUInstallDirs)
include(ExternalProject)

# installing lidar driver
find_program(MAKE_EXE NAMES gmake nmake make)
ExternalProject_Add(urg_library
  GIT_REPOSITORY    https://github.com/UrgNetwork/urg_library.git
  GIT_TAG           ver.1.2.7
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/urg_library-1.2.7
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ${MAKE_EXE}
  BUILD_IN_SOURCE   true
  INSTALL_COMMAND   ""
)
ExternalProject_Get_Property(urg_library SOURCE_DIR)
message("+++++" ${SOURCE_DIR})

add_subdirectory(lib)

add_subdirectory(src)