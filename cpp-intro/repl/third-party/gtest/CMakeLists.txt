include(FetchContent)

message(STATUS "Fetching GTest...")
FetchContent_Declare(
  GTest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
  FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(GTest)
