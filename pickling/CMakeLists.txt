
add_definitions(-DLINUX_ -DOC_NEW_STYLE_INCLUDES)

find_package(Threads REQUIRED)

add_library(pickling 
    m2pythontools.cc
    valpython.cc
    midassocket.cc
    valprotocol2.cc
    m2ser.cc
    m2streamdataenc.cc
    m2convertrep.cc
    timeconv.cc
    )
target_link_libraries(pickling
    ${CMAKE_THREAD_LIBS_INIT}
    ) 

