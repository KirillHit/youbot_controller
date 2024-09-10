add_subdirectory(youbot_driver)
add_subdirectory(yaml-cpp)

# installing lidar driver
find_program(MAKE_EXE NAMES gmake nmake make)
ExternalProject_Add(urg_library
  GIT_REPOSITORY    ${CMAKE_CURRENT_SOURCE_DIR}/urg_library
  GIT_TAG           ver.1.2.7
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/urg_library-1.2.7
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ${MAKE_EXE}
  BUILD_IN_SOURCE   true
  INSTALL_COMMAND   ""
)
ExternalProject_Get_Property(urg_library SOURCE_DIR)
set(urg_library_LIB ${SOURCE_DIR}/current/src/liburg_cpp.so PARENT_SCOPE)
set(urg_library_INCLUDE ${SOURCE_DIR}/current/include/c PARENT_SCOPE)
message("+++++" ${SOURCE_DIR})