cmake_minimum_required(VERSION 2.8)
add_definitions(-DJEMALLOC_NO_DEMANGLE) 
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++17 -O0 -fno-stack-protector -g")
# rpath
set(CMAKE_SKIP_BUILD_RPATH FALSE)                 # 编译时加上RPATH  
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)          # 编译时RPATH不使用安装的RPATH  
set(CMAKE_INSTALL_RPATH "${CMAKE_BINARY_DIR}/lib")# 安装RPATH为空  
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)       # 安装的执行文件加上RPATH
#include link
include_directories("${MMS_SOURCE_DIR}/src" "${CMAKE_BINARY_DIR}/include")
link_directories("${CMAKE_BINARY_DIR}/lib")
link_directories("${CMAKE_BINARY_DIR}/lib64")
#source files
file(GLOB_RECURSE MMS_SOURCES "${MMS_SOURCE_DIR}/src/*.h" "${MMS_SOURCE_DIR}/src/*.c" "${MMS_SOURCE_DIR}/src/*.cpp" "${MMS_SOURCE_DIR}/src/*.cc")

add_executable(mms ${MMS_SOURCES})
add_dependencies(mms 
    libboost-1.75.0
    libjemalloc
    libjsoncpp
    libopenssl-1.1.1
    libwebsocket++-0.8.2
    libsrtp
)

target_link_libraries(mms
    libsrtp2.a
    boost_system.a
    boost_serialization.a
    boost_date_time.a
    boost_regex.a
    boost_coroutine.a
    boost_thread.a
    boost_context.a
    boost_program_options.a
    ssl.a 
    crypto.a
    libjemalloc.a
    libjsoncpp.a
    dl
    pthread
)
install(TARGETS mms RUNTIME DESTINATION bin)
