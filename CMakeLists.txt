cmake_minimum_required(VERSION 3.16.3)
project(aresbc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ARES_VERSION_MAJOR 0)
set(ARES_VERSION_MINOR 1)
set(ARES_VERSION_PATCH 0)
set(ARES_VERSION ${ARES_VERSION_MAJOR}.${ARES_VERSION_MINOR}.${ARES_VERSION_PATCH})

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(LibZip)

include_directories(${LIBZIP_INCLUDE_DIR})

add_executable(aresbc
        src/main.cpp
        src/reader/classreader.cpp
        src/reader/classreader.h
        src/utils/utils.cpp
        src/utils/utils.h
        src/visitor/visitor.h
        src/visitor/vmcheck.cpp
        src/visitor/vmcheck.h
        src/structure/classinfo.cpp
        src/structure/classinfo.h
        src/structure/methodinfo.cpp
        src/structure/methodinfo.h
        src/structure/fieldinfo.cpp
        src/structure/fieldinfo.h
        src/structure/attributeinfo.cpp
        src/structure/attributeinfo.h
        src/structure/constantinfo.cpp
        src/structure/constantinfo.h
        src/writer/classwriter.cpp
        src/writer/classwriter.h
        src/wrapper/classwrapper.cpp
        src/wrapper/classwrapper.h
        src/wrapper/classpool.cpp
        src/wrapper/classpool.h)

target_link_libraries(aresbc LINK_PUBLIC ${LIBZIP_LIBRARY})

install(TARGETS aresbc DESTINATION bin)