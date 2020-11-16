find_library(LIBCXX_LIB c++)
find_package(Threads)
find_library(LIBCXXABI_LIB c++abi)

link_libraries(
    ${LIBCXX_LIB}
    ${LIBCXXABI_LIB}
)

add_library(m19
    m03_context_array.cpp
    m19_context_array.cpp
    m19_encoder.cpp
    m19_run_length_array.cpp
    m19_suffix_index_array.cpp
    m19.cpp
)

target_link_libraries(m19 ${CMAKE_THREAD_LIBS_INIT} io msufsort)

target_include_directories(m19
    PUBLIC
        $<BUILD_INTERFACE:${_m19_include_dir}>
        $<INSTALL_INTERFACE:include/m19>
)

target_compile_features(m19 PUBLIC cxx_std_17)
