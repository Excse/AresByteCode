include_directories(../src)
link_directories(../src)

add_executable(aresbc_test main.cpp)
target_link_libraries(aresbc_test aresbc)

add_test(NAME simple COMMAND aresbc_test)