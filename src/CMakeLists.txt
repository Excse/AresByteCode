set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(LibZip)
include_directories(${LIBZIP_INCLUDE_DIR})

add_library(aresbc SHARED
        reader/classreader.cpp
        reader/classreader.h
        utils/utils.cpp
        utils/utils.h
        visitor/visitor.h
        visitor/vmcheck.cpp
        visitor/vmcheck.h
        structure/classinfo.cpp
        structure/classinfo.h
        structure/methodinfo.cpp
        structure/methodinfo.h
        structure/fieldinfo.cpp
        structure/fieldinfo.h
        structure/attributeinfo.cpp
        structure/attributeinfo.h
        structure/constantinfo.cpp
        structure/constantinfo.h
        writer/classwriter.cpp
        writer/classwriter.h
        wrapper/classwrapper.cpp
        wrapper/classwrapper.h
        wrapper/classpool.cpp
        wrapper/classpool.h)

set_target_properties(aresbc PROPERTIES
        VERSION ${PROJECT_VERSION}
        PUBLIC_HEADER
        "writer/classwriter.h;reader/classreader.h;visitor/visitor.h;visitor/vmcheck.h;utils/utils.h;structure/attributeinfo.h;structure/classinfo.h;structure/constantinfo.h;structure/fieldinfo.h;structure/methodinfo.h;")
target_link_libraries(aresbc LINK_PUBLIC ${LIBZIP_LIBRARY})

include(GNUInstallDirs)
install(TARGETS aresbc
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/aresbc")