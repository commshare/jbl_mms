
// #include <new>
// #include <iostream>
// #ifdef JEMALLOC_NO_DEMANGLE
//     # include <jemalloc/jemalloc.h>
//     # define mms_malloc     je_malloc
//     # define mms_realloc    je_realloc
//     # define mms_free       je_free
// #else
//     # include <malloc.h>
//     # define mms_malloc     ::malloc
//     # define mms_realloc    ::realloc
//     # define mms_free     ::free
// #endif

// void* operator new (std::size_t size)
// {
//     void *p = mms_malloc(size);
//     return p;
// }

// void* operator new (std::size_t size, const std::nothrow_t& nothrow_value) throw()
// {
//     void *p = mms_malloc(size);
//     return p;
// }

// void operator delete (void* ptr) noexcept
// {
//     mms_free(ptr);
// }

// void operator delete (void* ptr, const std::nothrow_t& nothrow_constant) throw()
// {
//     mms_free(ptr);
// }