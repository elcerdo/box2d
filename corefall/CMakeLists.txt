
set(sources world.cpp printer.cpp drawer.cpp common.cpp corefall.cpp)
set(headers world.h   printer.h   drawer.h   common.h)

qt4_wrap_cpp(sources_moc ${headers})

add_executable(corefall ${sources} ${sources_moc} )
target_link_libraries(corefall ${QT_LIBRARIES} Box2D)
