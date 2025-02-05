cmake_minimum_required(VERSION 3.29.6)

set(ARES_VERSION_MAJOR 0)
set(ARES_VERSION_MINOR 1)
set(ARES_VERSION_PATCH 0)
set(ARES_VERSION ${ARES_VERSION_MAJOR}.${ARES_VERSION_MINOR}.${ARES_VERSION_PATCH})

set(ARES_DESCRIPTION "AresByteCode is a library to interact with java bytecode and provides also utils to load/write jar files.")

project(aresbc VERSION ${ARES_VERSION} DESCRIPTION ${ARES_DESCRIPTION})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wswitch -fsanitize=address,undefined,leak")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

# =====================
# Google Test
# =====================

FetchContent_Declare(
        google_test
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(google_test)

# =====================
# LibZip
# =====================

FetchContent_Declare(
        libzip
        GIT_REPOSITORY https://github.com/nih-at/libzip.git
        GIT_TAG main
)

FetchContent_MakeAvailable(libzip)

# =====================
# Boost
# =====================

# You need to have boost installed. E.g. via sudo apt-get install libboost-all-dev
find_package(Boost REQUIRED)

# =====================
# Library
# =====================

include_directories(include)

add_library(${PROJECT_NAME}_lib
        src/class_reader.cpp
        src/attribute_info.cpp
        src/class_file.cpp
        src/constant_info.cpp
        src/field_info.cpp
        src/method_info.cpp
        src/utils.cpp
        src/vm_check.cpp
        src/class_writer.cpp)

target_link_libraries(${PROJECT_NAME}_lib PRIVATE zip Boost::boost)

# =====================
# Tests
# =====================

enable_testing()

add_executable(${PROJECT_NAME}_tests
        tests/main.cpp)

target_link_libraries(${PROJECT_NAME}_tests gtest gtest_main ${PROJECT_NAME}_lib)
target_compile_definitions(${PROJECT_NAME}_tests PRIVATE TEST_PATH="${CMAKE_SOURCE_DIR}/tests")

add_test(NAME test_${PROJECT_NAME} COMMAND test_${PROJECT_NAME})