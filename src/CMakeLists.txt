add_executable(YouBotLidarNav)

target_compile_features(YouBotLidarNav PUBLIC cxx_std_11)

target_link_libraries(YouBotLidarNav YouBotDriver)

target_include_directories(
    YouBotLidarNav
  PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<TARGET_PROPERTY:YouBotDriver,INCLUDE_DIRECTORIES>
)

target_sources(
    YouBotLidarNav
  PRIVATE 
    main.cpp
)