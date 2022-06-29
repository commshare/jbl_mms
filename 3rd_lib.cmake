include(ExternalProject)

ExternalProject_Add(libboost-1.75.0
    EXCLUDE_FROM_ALL 1
    URL https://boostorg.jfrog.io/artifactory/main/release/1.75.0/source/boost_1_75_0.tar.gz
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./bootstrap.sh --prefix=${PROJECT_BINARY_DIR}
    BUILD_COMMAND ./b2 cxxflags="--std=c++11" --with-system --with-thread --with-date_time --with-regex --with-serialization --with-program_options --with-context --with-coroutine install
    INSTALL_COMMAND ""
)
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

ExternalProject_Add(libjemalloc
    EXCLUDE_FROM_ALL 1
    URL https://github.com/jemalloc/jemalloc/releases/download/5.2.1/jemalloc-5.2.1.tar.bz2
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./configure --with-jemalloc-prefix=je_ --prefix=${PROJECT_BINARY_DIR}
    BUILD_COMMAND make
    INSTALL_COMMAND make install
)

ExternalProject_Add(libjsoncpp
    EXCLUDE_FROM_ALL 1
    URL https://github.com/open-source-parsers/jsoncpp/archive/refs/tags/1.8.4.tar.gz
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND cmake . -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}
    BUILD_COMMAND make
    INSTALL_COMMAND make install
)

ExternalProject_Add(libopenssl-1.1.1
    EXCLUDE_FROM_ALL 1
    URL https://www.openssl.org/source/old/1.1.1/openssl-1.1.1.tar.gz
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./config --prefix=${PROJECT_BINARY_DIR}
    BUILD_COMMAND make
    INSTALL_COMMAND make install
)

ExternalProject_Add(libwebsocket++-0.8.2
    DEPENDS libopenssl-1.1.1 libboost-1.75.0
    EXCLUDE_FROM_ALL 1
    URL https://github.com/zaphoyd/websocketpp/archive/refs/tags/0.8.2.tar.gz
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND cmake . -DBOOST_ROOT=${PROJECT_BINARY_DIR} -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}
    BUILD_COMMAND make
    INSTALL_COMMAND make install
)

ExternalProject_Add(libsrtp
    EXCLUDE_FROM_ALL 1
    URL https://github.com/cisco/libsrtp/archive/refs/tags/v2.4.2.tar.gz
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./configure --prefix=${PROJECT_BINARY_DIR}
    BUILD_COMMAND make
    INSTALL_COMMAND make install
)

