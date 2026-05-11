include(FetchContent)


message(STATUS "Fetching replxx")
FetchContent_Declare(
        replxx
        GIT_REPOSITORY https://github.com/AmokHuginnsson/replxx.git
        GIT_TAG        release-0.0.4
        GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(replxx)
