# Filesystem.cmake - Adapted from standard CMake modules
# Provides Filesystem::Filesystem target

include(CMakeFindDependencyMacro)
include(CheckCXXSourceCompiles)

# Test standard filesystem
check_cxx_source_compiles("
#include <filesystem>
int main() {
    auto path = std::filesystem::path(\"/\");
    return 0;
}
" HAVE_STD_FILESYSTEM)

# Test experimental filesystem
if(NOT HAVE_STD_FILESYSTEM)
    check_cxx_source_compiles("
    #include <experimental/filesystem>
    int main() {
        auto path = std::experimental::filesystem::path(\"/\");
        return 0;
    }
    " HAVE_STD_EXPERIMENTAL_FILESYSTEM)
endif()

# Set up target
add_library(Filesystem::Filesystem INTERFACE IMPORTED)

if(HAVE_STD_FILESYSTEM)
    message(STATUS "Using std::filesystem")
    target_compile_definitions(Filesystem::Filesystem INTERFACE HAVE_STD_FILESYSTEM)
elseif(HAVE_STD_EXPERIMENTAL_FILESYSTEM)
    message(STATUS "Using std::experimental::filesystem")
    target_compile_definitions(Filesystem::Filesystem INTERFACE HAVE_STD_EXPERIMENTAL_FILESYSTEM)
    
    # Try to figure out if linking is necessary
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        target_link_libraries(Filesystem::Filesystem INTERFACE stdc++fs)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        target_link_libraries(Filesystem::Filesystem INTERFACE c++fs)
    endif()
else()
    message(FATAL_ERROR "No filesystem support available")
endif() 