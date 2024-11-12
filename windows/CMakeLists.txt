cmake_minimum_required(VERSION 3.30)
project(typing-trainer-windows)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

set(SRC_DIR "../src")
file(GLOB SRC_FILES "${SRC_DIR}/*.c")

include_directories(include)

add_executable(typing_trainer ${SRC_FILES})

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
target_link_libraries(typing_trainer PRIVATE SDL2::SDL2 SDL2_ttf::SDL2_ttf)

set_target_properties(typing_trainer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
)

# Add custom 'run' and 'clean' targets
add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/build/typing_trainer
    DEPENDS typing_trainer
    COMMENT "Running the application..."
)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/build"
    COMMENT "Cleaning build files..."
)
