find_package(Qt4)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTMULTIMEDIA TRUE)
include(${QT_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR})

set(sources
    world.cpp drawer.cpp
    gamestate.cpp
    main.cpp
    )
set(headers
    world.h   drawer.h
    gamestate.h
    )
set(ressources
    )

qt4_wrap_cpp(sources_moc ${headers})
qt4_add_resources(sources_rcc ${ressources})

add_executable(pallet
    ${sources}
    ${sources_moc}
    ${sources_rcc}
    )
target_link_libraries(pallet ${QT_LIBRARIES} Box2D)

install(TARGETS pallet
    RUNTIME DESTINATION bin
    )

