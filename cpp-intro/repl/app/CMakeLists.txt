project(repl-app LANGUAGES CXX)

# Setup an executable target
add_executable(${PROJECT_NAME} main.cpp teammates.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
ct_set_compiler_warnings(${PROJECT_NAME})

# Link with 'repl-lib'
target_link_libraries(${PROJECT_NAME} PRIVATE repl-lib)
target_link_libraries(${PROJECT_NAME} PUBLIC replxx::replxx)
