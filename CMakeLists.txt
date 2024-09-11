cmake_minimum_required(VERSION 3.28)

project(YoubotLidarNav)

include(GNUInstallDirs)
include(ExternalProject)

add_subdirectory(lib EXCLUDE_FROM_ALL)

add_subdirectory(src)