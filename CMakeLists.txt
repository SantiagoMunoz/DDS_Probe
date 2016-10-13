cmake_minimum_required(VERSION 2.8.12)

project("dds_probe")

option(THIRDPARTY "Activates the build of internal thirdparties (Fast-RTPS)" OFF)

include(${PROJECT_SOURCE_DIR}/cmake/eprosima_libraries.cmake)
find_eprosima_package(fastrtps)

include(CheckCXXCompilerFlag)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG OR
        CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    check_cxx_compiler_flag(--std=c++11 SUPPORTS_CXX11)
    if(SUPPORTS_CXX11)
        add_compile_options(--std=c++11)
    else()
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

file(GLOB DDSPROBE_CPP "*.cpp")
add_executable(dds_probe ${DDSPROBE_CPP})
target_link_libraries(dds_probe fastrtps fastcdr ${Boost_LIBRARIES})

