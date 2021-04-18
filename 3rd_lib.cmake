include(ExternalProject)

ExternalProject_Add(libboost
    EXCLUDE_FROM_ALL 1
    URL https://dl.bintray.com/boostorg/release/1.75.0/source/boost_1_75_0.tar.gz
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./bootstrap.sh --prefix=${PROJECT_BINARY_DIR}
    BUILD_COMMAND ./b2 cxxflags="--std=c++11" --with-system --with-thread --with-date_time --with-regex --with-serialization --with-context --with-coroutine install
    INSTALL_COMMAND ""
)

ExternalProject_Add(libjemalloc
    EXCLUDE_FROM_ALL 1
    URL https://github.com/jemalloc/jemalloc/releases/download/5.2.1/jemalloc-5.2.1.tar.bz2
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./configure --with-jemalloc-prefix=je_ --prefix=${PROJECT_BINARY_DIR}
    BUILD_COMMAND make
    INSTALL_COMMAND make install
)
