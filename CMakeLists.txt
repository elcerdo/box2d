project(robot)
cmake_minimum_required(VERSION 2.8)

set(BOX2D_BUILD_STATIC TRUE)
add_subdirectory(Box2D)
#add_subdirectory(pickling)

#add_subdirectory(robot)
add_subdirectory(volley)
add_subdirectory(pallet)


