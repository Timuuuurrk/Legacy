if (POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif ()
set(CTPG_ENABLE_TESTS OFF)

include(FetchContent)
message(STATUS "Fetching ctpg")
FetchContent_Declare(
        ctpg
        GIT_REPOSITORY https://github.com/peter-winter/ctpg.git
        GIT_TAG v1.3.7
)
FetchContent_MakeAvailable(ctpg)
target_include_directories(ctpg SYSTEM INTERFACE $<BUILD_INTERFACE:${ctpg_SOURCE_DIR}/include>)

