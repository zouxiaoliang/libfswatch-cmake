cmake_minimum_required(VERSION 3.5)

project(NERVd LANGUAGES CXX C)

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

function (toolchainMain)
    add_library(thirdparty_toolchain INTERFACE)
    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
        target_link_libraries(thirdparty_toolchain PUBLIC INTERFACE libunwind.a libc++abi.a libz.a dl rt)
    endif()
endfunction()

function(libfswatchMain)
    add_library(_libfswatch STATIC IMPORTED GLOBAL)
    set_target_properties(_libfswatch PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/fswatch/libfswatch/libfswatch.a)

    add_library(thirdparty_libfswatch INTERFACE)
    target_include_directories(thirdparty_libfswatch INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/fswatch/libfswatch/src)
    target_link_libraries(thirdparty_libfswatch INTERFACE _libfswatch)

endfunction(libfswatchMain)

function(libfswatchEasyMain)
    add_library(thirdparty_libfswatch_easy easy/FileTailF.cpp)
    target_link_libraries(thirdparty_libfswatch_easy thirdparty_libfswatch)
    target_include_directories(thirdparty_libfswatch_easy INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endfunction(libfswatchEasyMain)

function(tailfMain)
    add_executable(tailf test/tailf.cpp)
    target_link_libraries(tailf thirdparty_libfswatch_easy thirdparty_toolchain)
endfunction(tailfMain)

toolchainMain()
libfswatchMain()
libfswatchEasyMain()
tailfMain()
