cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project("static-stl")

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(compilation_flags "-std=c++11 -Wall -g")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(compilation_flags "-std=c++11 -Wall -g")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(compilation_flags "/W4 /EHsc")
else (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(FATAL_ERROR "not supported toolchain" )
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${compilation_flags}")

set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories("include")
include_directories("unittest-cpp")
include_directories("Catch/include")
include_directories("test")

if (${UNIX})
    file(GLOB unittestcpp_srcs   "unittest-cpp/UnitTest++/*.cpp"
                                 "unittest-cpp/UnitTest++/*.h"
                                 "unittest-cpp/UnitTest++/Posix/*.cpp"
                                 "unittest-cpp/UnitTest++/Posix/*.h")
elseif (${WIN32})
    file(GLOB unittestcpp_srcs   "unittest-cpp/UnitTest++/*.cpp"
                                 "unittest-cpp/UnitTest++/*.h"
                                 "unittest-cpp/UnitTest++/Win32/*.cpp"
                                 "unittest-cpp/UnitTest++/Win32/*.h")
else (${UNIX})
    message(FATAL_ERROR "not supported OS" )
endif()
add_library(unittestcpp ${unittestcpp_srcs})

file(GLOB sstl_srcs "include/sstl/*.cpp" "include/sstl/*.h" "include/sstl/__internal/*.cpp" "include/sstl/__internal/*.h")
file(GLOB test_srcs "test/*.cpp" "test/*.h" ${sstl_srcs})

add_executable(test-sstl ${test_srcs})
target_link_libraries(test-sstl unittestcpp)

add_executable(test-sstl-noexceptions ${test_srcs})
set_target_properties(test-sstl-noexceptions PROPERTIES COMPILE_DEFINITIONS "_SSTL_NOEXCEPTIONS_TEST")
target_link_libraries(test-sstl-noexceptions unittestcpp)
