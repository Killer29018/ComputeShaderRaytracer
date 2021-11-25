cmake_minimum_required(VERSION 3.18)

project (Imgui)

file (GLOB IMGUI_INCLUDE CONFIGURE_DEPENDS 
    "Imgui/*.cpp"
    "Imgui/Backends/*.cpp"
    "Imgui/*.h"
    "Imgui/Backends/*.h"
)

find_package(OpenGL REQUIRED)

add_library(Imgui STATIC ${IMGUI_INCLUDE})

target_include_directories(Imgui
    PUBLIC
    "Imgui"
    "Imgui/Backends/"
)

set (linker "")
if (WIN32)
    set (linker "-limm32")
elseif (UNIX)
    set (linker "")
endif()

target_link_libraries(Imgui PRIVATE ${linker}
    PUBLIC
    glfw
    OpenGL::GL
)