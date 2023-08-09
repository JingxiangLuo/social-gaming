add_library(gameclient GameChatWindow.cpp)

set_target_properties(gameclient
    PROPERTIES 
    CXX_STANDARD 20
    CMAKE_C_COMPILER clang
    CMAKE_CXX_COMPILER clang++
)

target_include_directories(gameclient
PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

find_package(Curses REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries( gameclient
    ${CURSES_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    user
)

