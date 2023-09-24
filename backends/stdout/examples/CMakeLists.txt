
set(CMAKE_CXX_STANDARD 17)

add_executable(parserDemo main.cpp dump.cpp)
target_link_libraries(parserDemo PRIVATE NodesetLoader)
target_link_libraries(parserDemo PRIVATE open62541::open62541)
