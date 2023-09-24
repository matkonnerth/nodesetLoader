
set(CMAKE_CXX_STANDARD 17)

add_executable(parserDemo main.c dump.c)
target_link_libraries(parserDemo PRIVATE NodesetLoader)
target_link_libraries(parserDemo PRIVATE open62541::open62541)

add_executable(cppDemo main.cpp dump.cpp)
target_link_libraries(cppDemo PRIVATE NodesetLoader)
target_link_libraries(cppDemo PRIVATE open62541::open62541)
