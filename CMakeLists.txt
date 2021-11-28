cmake_minimum_required(VERSION 3.18)

project ( Raytracing )

set (CMAKE_CXX_STANDARD 17)

# Import Libraries
find_package(OpenGL REQUIRED)

# Add Platform Specific Linker Options
set (linker "")
if (WIN32)
    set (linker "-lopengl32 -luser32 -limm32")
elseif(UNIX)
    set (linker "-ldl")
endif()

# Change Output Directory
set (CMAKE_BINARY_DIR "/bin")
set (OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}${CMAKE_BINARY_DIR}")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message ("Building Debug")
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}/debug")
    set (linker "${linker} -g")
else ()
    message ("Building Release")
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}/release")
endif ()

file (GLOB_RECURSE cppFiles CONFIGURE_DEPENDS "src/*.cpp")
add_executable(${PROJECT_NAME} ${cppFiles})

# Add Subdirectories
add_subdirectory(vendor)

# Add libraries and include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    KRE
    vendor/glad/include
    vendor/jsoncpp/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${linker} PUBLIC
    glad
    OpenGL::GL
    glm::glm
    glfw
    stb
    Imgui
    jsoncpp

    KRE
)

# # Copy Res Folder
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     "${CMAKE_SOURCE_DIR}/res"
#     "${EXECUTABLE_OUTPUT_PATH}/res"
# )