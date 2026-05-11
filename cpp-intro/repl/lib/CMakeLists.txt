project(repl-lib LANGUAGES CXX)

# Setup a library target
file(GLOB_RECURSE REPL_LIB_SOURCES CONFIGURE_DEPENDS *.cpp)
add_library(${PROJECT_NAME} ${REPL_LIB_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/Schema/query_parser"
        "${CMAKE_CURRENT_SOURCE_DIR}/Schema/schema_preparation"
)
ct_set_compiler_warnings(${PROJECT_NAME})

# Link with third-party dependencies
find_package(httplib CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC httplib)

target_link_libraries(${PROJECT_NAME} PUBLIC CLI11::CLI11)
target_link_libraries(${PROJECT_NAME} PUBLIC ctpg)
target_link_libraries(${PROJECT_NAME} PUBLIC xxHash::xxhash)
