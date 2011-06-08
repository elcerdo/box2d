
add_definitions(-DLINUX_ -DOC_NEW_STYLE_INCLUDES)

set(sources world.cpp printer.cpp drawer.cpp robot.cpp logic.cpp common.cpp corefall.cpp)
set(headers world.h   printer.h   drawer.h   robot.h   logic.h)

qt4_wrap_cpp(sources_moc ${headers})

add_executable(corefall ${sources} ${sources_moc} )
target_link_libraries(corefall ${QT_LIBRARIES} Box2D pickling)
