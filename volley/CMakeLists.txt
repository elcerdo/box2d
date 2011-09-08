find_package(Qt4)
set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR})

#add_definitions(-DLINUX_ -DOC_NEW_STYLE_INCLUDES)
#
### GUI executable
#set(sources_gui world.cpp printer.cpp robot.cpp logic.cpp drawer.cpp
#    common.cpp
#    simulateRobotGui.cpp
#    )
#set(headers_gui world.h   printer.h   robot.h   logic.h   drawer.h
#    )
#
#qt4_wrap_cpp(sources_moc_gui ${headers_gui})
#
#add_executable(simulateRobotGui
#    ${sources_gui}
#    ${sources_moc_gui}
#    )
#target_link_libraries(simulateRobotGui ${QT_LIBRARIES} Box2D pickling)

## batch executable
set(sources
    world.cpp drawer.cpp
    gamedata.cpp
    common.cpp
    main.cpp
    )
set(headers
    world.h   drawer.h
    gamedata.h
    )
set(ressources
    data.qrc
    )

qt4_wrap_cpp(sources_moc ${headers})
qt4_add_resources(sources_rcc ${ressources})

add_executable(volley
    ${sources}
    ${sources_moc}
    ${sources_rcc}
    )
target_link_libraries(volley ${QT_LIBRARIES} Box2D)

install(TARGETS volley
    RUNTIME DESTINATION bin
    )

