include(FetchContent)


message(STATUS "Fetching CLI 11")
FetchContent_Declare(
        CLI11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG        v2.5.0
        GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(CLI11)
