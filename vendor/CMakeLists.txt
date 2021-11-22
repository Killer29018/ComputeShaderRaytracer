# GLAD
add_subdirectory(glad/cmake)
glad_add_library(glad STATIC API gl:core=4.6)

# GLM
set(GLM_QUIET OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory(glm)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
set_target_properties(glfw PROPERTIES FOLDER vendor)

# STB
add_subdirectory(STB)

# KRE
set (KRE_BUILD_STATIC_LIBRARY ON CACHE BOOL "" FORCE)
add_subdirectory(KRE)

# Imgui
add_subdirectory(Imgui)