cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ARES_VERSION_MAJOR 0)
set(ARES_VERSION_MINOR 1)
set(ARES_VERSION_PATCH 0)
set(ARES_VERSION ${ARES_VERSION_MAJOR}.${ARES_VERSION_MINOR}.${ARES_VERSION_PATCH})

set(ARES_DESCRIPTION "AresByteCode is a library to interact with java bytecode and provides also utils to load/write jar files.")

project(aresbc VERSION ${ARES_VERSION} DESCRIPTION ${ARES_DESCRIPTION})
add_subdirectory(src)

enable_testing()
add_subdirectory(tests)