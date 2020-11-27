# AresByteCode

AresByteCode is a library for reading Jar files up to version 15 and is also based on
official JVM documentation.
  - Tools for parsing whole Jar archives
  - Used as a basis for other projects like AresObfuscator
  - Fully functional up to Java version 15

### Installation

Download the project or use git to clone it:
```sh
$ git clone https://github.com/Excse/AresByteCode.git
```

Navigate to the directory where the project is located and execute these commands:
```sh
$ mkdir build && cd build
$ cmake ..
$ sudo make install
```

When you are finished, you are ready to use the library. If you want to use it in a
cmake project, use the following code to implement it.

FindAresBC.cmake:
```cmake
FIND_PATH(LIBARESBC_INCLUDE_DIR aresbc/classwriter.h
        /usr/local/include
        /usr/include)
if (LIBARESBC_INCLUDE_DIR)
    SET(LIBARESBC_INCLUDE_DIR ${LIBARESBC_INCLUDE_DIR}/aresbc)
endif ()

FIND_LIBRARY(LIBARESBC_LIBRARY
        NAMES libaresbc aresbc
        PATHS
        /usr/local/lib
        /usr/lib)
```

and in CMakeLists.txt:
```cmake
find_package(AresBC)
include_directories(${LIBARESBC_INCLUDE_DIR})

target_link_libraries(!!PROJECT_NAME!! LINK_PUBLIC ${LIBARESBC_LIBRARY})
```