cmake_minimum_required(VERSION 2.8.12)

project("dds_probe")

option(THIRDPARTY "Activates the build of internal thirdparties (Fast-RTPS)" OFF)

endif()

#Check libraries
include(${PROJECT_SOURCE_DIR}/cmake/dev/boost_libraries.cmake)
set(BOOST_LIBRARIES_DEFAULT thread system date_time chrono regex atomic)
check_boost(${BOOST_LIBRARIES_DEFAULT})


#Set install dirs
set(BIN_INSTALL_DIR bin/ CACHE PATH "Installation directory for binaries")
set(INCLUDE_INSTALL_DIR include/ CACHE PATH "Installation directory for C++ Headers")
set(LIB_INSTALL_DIR lib/ CACHE PATH "Installation directory for libraries")
set(DATA_INSTALL_DIR share/ CACHE PATH "Installation directory for data")
set(DOC_DIR "${DATA_INSTALL_DIR}/doc")
set(DOC_INSTALL_DIR ${DOC_DIR} CACHE PATH "Installation directory for documentation")
set(LICENSE_INSTALL_DIR ${DATA_INSTALL_DIR}/${PROJECT_NAME} CACHE PATH "Installation directory for licenses")

set(CMAKE_BUILD_TYPE Release CACHE STRING "Build Type")

#Load external projects

include(${PROJECT_SOURCE_DIR}/cmake/dev/eprosima_libraries.cmake)
find_eprosima_package(fastcdr)
find_eprosima_package(fastrtps USE_BOOST)

include(${PROJECT_SOURCE_DIR}/cmake/dev/set_sources.cmake)

# Find requirements
find_package(Boost REQUIRED COMPONENTS thread system chrono regex)
find_package(fastrtps REQUIRED)


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

if(THIRDPARTY)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

add_executable(dds_probe ${DDSPROBE_CPP})
target_link_libraries(dds_probe fastrtps fastcdr ${Boost_LIBRARIES})

