cmake_minimum_required(VERSION 2.8)
add_definitions(-DJEMALLOC_NO_DEMANGLE) 
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++17 -O3 -fno-stack-protector")
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
file(GLOB_RECURSE TEST_SOURCES "${MMS_SOURCE_DIR}/test/*.h" "${MMS_SOURCE_DIR}/test/*.c" "${MMS_SOURCE_DIR}/test/*.cpp" "${MMS_SOURCE_DIR}/test/*.cc")

add_executable(test_cert ${TEST_SOURCES})
add_dependencies(
    test_cert
    libopenssl-1.1.1
)
target_link_libraries(test_cert
    ssl.a 
    crypto.a
    dl
    pthread
)
install(TARGETS test_cert RUNTIME DESTINATION bin)