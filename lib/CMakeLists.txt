add_subdirectory(youbot_driver)
add_subdirectory(yaml-cpp)
add_subdirectory(simple_socket)

# installing lidar driver
find_program(MAKE_EXE NAMES gmake nmake make)
ExternalProject_Add(urg_library
  GIT_REPOSITORY    ${CMAKE_CURRENT_SOURCE_DIR}/urg_library
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ${MAKE_EXE}
  BUILD_IN_SOURCE   true
  INSTALL_COMMAND   ""
)
ExternalProject_Get_Property(urg_library SOURCE_DIR)
set(urg_library_LIB ${SOURCE_DIR}/current/src/liburg_cpp.so PARENT_SCOPE)
set(urg_library_INCLUDE ${SOURCE_DIR}/current/include/c PARENT_SCOPE)