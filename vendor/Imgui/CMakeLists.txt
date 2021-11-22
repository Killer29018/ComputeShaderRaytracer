cmake_minimum_required(VERSION 3.18)

project (Imgui)

file (GLOB IMGUI_INCLUDE CONFIGURE_DEPENDS "*.h" "*.cpp" "backends/*.h" "backends/*.cpp")

add_library(Imgui STATIC ${IMGUI_INCLUDE})

target_include_directories(Imgui
    PUBLIC
    "/"
    "backends/"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(Imgui PUBLIC
    glfw
)