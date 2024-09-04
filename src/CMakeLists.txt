add_executable(main)

target_compile_features(main PUBLIC cxx_std_11)

target_include_directories(
    main
  PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>"
)

target_sources(
    main
  PRIVATE 
    main.cpp
)