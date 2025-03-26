ADD_DEFINITIONS(-DSOURCE_DIR="${PROJECT_SOURCE_DIR}/")

add_executable(youbot_lidar_nav)

target_compile_features(youbot_lidar_nav PUBLIC cxx_std_20)

add_dependencies(youbot_lidar_nav urg_library)

target_link_libraries(youbot_lidar_nav 
  YouBotDriver 
  yaml-cpp 
  udp_streamer 
  ${urg_library_LIB} 
  "${CMAKE_CURRENT_SOURCE_DIR}/../lib/onnxruntime/lib/libonnxruntime.so"
)

target_include_directories(
    youbot_lidar_nav
  PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<TARGET_PROPERTY:YouBotDriver,INCLUDE_DIRECTORIES>
    $<BUILD_INTERFACE:${urg_library_INCLUDE}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../lib/onnxruntime/include>
)

target_sources(
    youbot_lidar_nav
  PRIVATE
    utils/task_pool.cpp
    utils/logger.cpp
    tasks/driver_task.cpp
    tasks/tcp_server_task.cpp
    tasks/lidar_task.cpp
    tasks/planner_task.cpp
    tasks/streamer_task.cpp
    youbot_lidar_nav.cpp
)