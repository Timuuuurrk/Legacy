include(FetchContent)


message(STATUS "Fetching xxHash")
FetchContent_Declare(
        xxhash
        GIT_REPOSITORY https://github.com/Cyan4973/xxHash.git
        GIT_TAG        v0.8.3
        GIT_SHALLOW    TRUE
        SOURCE_SUBDIR  cmake_unofficial
)
FetchContent_MakeAvailable(xxhash)

if(NOT TARGET xxHash::xxhash AND TARGET xxhash)
    add_library(xxHash::xxhash ALIAS xxhash)
endif ()
