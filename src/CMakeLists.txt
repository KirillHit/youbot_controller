ADD_DEFINITIONS(-DSOURCE_DIR="${PROJECT_SOURCE_DIR}/")

add_executable(YouBotLidarNav)

target_compile_features(YouBotLidarNav PUBLIC cxx_std_20)

target_link_libraries(YouBotLidarNav YouBotDriver yaml-cpp)

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
    logger.cpp
    driver.cpp
)