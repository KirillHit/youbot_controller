cmake_minimum_required(VERSION 3.28)

project(youbot_lidar_nav)

include(GNUInstallDirs)
include(ExternalProject)

add_subdirectory(lib)

add_subdirectory(src)