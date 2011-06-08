project(box2Dstuff)
cmake_minimum_required(VERSION 2.8)

add_subdirectory(Box2D)
add_subdirectory(pickling)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(corefall)



