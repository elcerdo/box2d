find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR})

add_definitions(-DLINUX_ -DOC_NEW_STYLE_INCLUDES)

## GUI executable
set(sources_gui world.cpp printer.cpp robot.cpp logic.cpp drawer.cpp
    common.cpp
    simulateRobotGui.cpp
    )
set(headers_gui world.h   printer.h   robot.h   logic.h   drawer.h
    )

qt4_wrap_cpp(sources_moc_gui ${headers_gui})

add_executable(simulateRobotGui
    ${sources_gui}
    ${sources_moc_gui}
    )
target_link_libraries(simulateRobotGui ${QT_LIBRARIES} Box2D pickling)

## batch executable
set(sources world.cpp printer.cpp robot.cpp logic.cpp
    common.cpp
    simulateRobot.cpp
    )
set(headers world.h   printer.h   robot.h   logic.h
    )

qt4_wrap_cpp(sources_moc ${headers})

add_executable(simulateRobot
    ${sources}
    ${sources_moc}
    )
target_link_libraries(simulateRobot QtCore Box2D pickling)

install(TARGETS simulateRobot simulateRobotGui
    RUNTIME DESTINATION bin
    )

install(PROGRAMS generateRobot.py parseRobot.py
    DESTINATION bin
    )
