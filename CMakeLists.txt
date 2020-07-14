cmake_minimum_required(VERSION 3.15)
project(Assignment_1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)

if(APPLE)
    add_definitions(-Wno-deprecated-declarations)
endif()


# OPENGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})


# GLUT
if (WIN32)
    #manual setting for windows, you may need to change the dir below
    set(GLUT_INCLUDE_DIR "win/glut/include")
    set(GLUT_LIBRARY_DIR "win/glut/lib")
endif()

if(APPLE)
    find_package(GLUT REQUIRED)
endif()

include_directories( ${GLUT_INCLUDE_DIR} )
link_directories( ${GLUT_LIBRARY_DIR} )


#source files
file( GLOB SRCFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

add_library(source_lib STATIC ${SRCFILES})


# Add OpenGL and GLUT
target_include_directories(source_lib PUBLIC SYSTEM)
target_link_libraries(source_lib ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})


#MESSAGE(status ${GLUT_INCLUDE_DIR})

add_executable(MAZE src/main.cpp)
target_link_libraries(MAZE PUBLIC source_lib)

