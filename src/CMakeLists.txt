set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(LibZip)
include_directories(${LIBZIP_INCLUDE_DIR})

add_library(aresbc SHARED
        ../include/visitor.h
        reader/classreader.cpp
        ../include/classreader.h
        utils/utils.cpp
        ../include/utils.h
        visitor/vmcheck.cpp
        ../include/vmcheck.h
        structure/classinfo.cpp
        ../include/classinfo.h
        structure/methodinfo.cpp
        ../include/methodinfo.h
        structure/fieldinfo.cpp
        ../include/fieldinfo.h
        structure/attributeinfo.cpp
        ../include/attributeinfo.h
        structure/constantinfo.cpp
        ../include/constantinfo.h
        writer/classwriter.cpp
        ../include/classwriter.h)

set_target_properties(aresbc PROPERTIES
        VERSION ${PROJECT_VERSION})
target_link_libraries(aresbc LINK_PUBLIC ${LIBZIP_LIBRARY})

include(GNUInstallDirs)
install(TARGETS aresbc
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/aresbc")