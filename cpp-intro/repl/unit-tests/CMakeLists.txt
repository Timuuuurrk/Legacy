project(repl-unit-tests LANGUAGES CXX)

# Setup a test target
file(GLOB REPL_TESTS_SOURCES CONFIGURE_DEPENDS *.cpp)
add_executable(${PROJECT_NAME} ${REPL_TESTS_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
ct_set_compiler_warnings(${PROJECT_NAME})

# Link with 'repl-lib'
target_link_libraries(${PROJECT_NAME} PRIVATE repl-lib)

# Link with third-party dependencies
find_package(GTest CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest_main)
