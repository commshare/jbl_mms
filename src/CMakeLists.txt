cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")
# rpath
set(CMAKE_SKIP_BUILD_RPATH FALSE)                 # 编译时加上RPATH  
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)          # 编译时RPATH不使用安装的RPATH  
set(CMAKE_INSTALL_RPATH "${CMAKE_BINARY_DIR}/lib")# 安装RPATH为空  
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)       # 安装的执行文件加上RPATH
#include link
include_directories("${MMS_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/include")
link_directories("${CMAKE_BINARY_DIR}/lib")
#source files
file(GLOB_RECURSE MMS_SOURCES "${MMS_SOURCE_DIR}/src/*.h" "${MMS_SOURCE_DIR}/src/*.c" "${MMS_SOURCE_DIR}/src/*.cpp" "${MMS_SOURCE_DIR}/src/*.cc")

add_executable(mms ${MMS_SOURCES})
add_dependencies(mms 
    libboost
)

# target_link_libraries(
#     MMS
#     libboost
# )

install(TARGETS mms RUNTIME DESTINATION bin)