cmake_minimum_required(VERSION 3.5)

project(libfswatch-cmake LANGUAGES CXX C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FindGettext)
include(FindIntl)
include(CheckIncludeFileCXX)
include(CheckStructHasMember)
include(CheckCXXSymbolExists)

include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR}/fswatch/)

add_subdirectory(fswatch/libfswatch)

function(libfswatchMain)
    add_library(_libfswatch STATIC IMPORTED GLOBAL)
    add_dependencies(_libfswatch libfswatch)
    set_target_properties(_libfswatch PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/fswatch/libfswatch/libfswatch.a)

    add_library(thirdparty_libfswatch INTERFACE)
    add_dependencies(thirdparty_libfswatch _libfswatch)
    target_include_directories(thirdparty_libfswatch INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/fswatch/libfswatch/src ${CMAKE_CURRENT_BINARY_DIR}/fswatch/)
    target_link_libraries(thirdparty_libfswatch INTERFACE _libfswatch)

endfunction(libfswatchMain)

function(libfswatchEasyMain)
    message(STATUS "import libfswatch_easy")
    add_library(thirdparty_libfswatch_easy easy/FileTailF.cpp)
    target_link_libraries(thirdparty_libfswatch_easy thirdparty_libfswatch)
    target_include_directories(thirdparty_libfswatch_easy INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endfunction(libfswatchEasyMain)

function(tailfMain)
    add_executable(tailf test/tailf.cpp)
    target_link_libraries(tailf thirdparty_libfswatch_easy)
endfunction(tailfMain)

function(auditdlogWatcherMain)
    add_executable(auditdlog_watcher test/auditdlog_watcher.cpp)
    target_link_libraries(auditdlog_watcher thirdparty_libfswatch_easy)
endfunction(auditdlogWatcherMain)


libfswatchMain()
libfswatchEasyMain()
tailfMain()
auditdlogWatcherMain()
