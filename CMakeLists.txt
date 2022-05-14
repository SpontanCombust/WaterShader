cmake_minimum_required(VERSION 3.14.0)
project(WaterShader VERSION 0.1.0)


# ======================= Dependencies =======================
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 3.3.7
)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 0.9.9.8
)
FetchContent_Declare(
    objloader
    GIT_REPOSITORY https://github.com/Bly7/OBJ-Loader
    GIT_TAG 03b06bf50bca2953a0ccd2d2ba373b27f46eecf2
)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb
    GIT_TAG af1a5bc352164740c1cc1354942b1c6b72eacb8a
)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.87
)

FetchContent_MakeAvailable(glfw glm objloader stb imgui)


find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_library(imgui)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_sources(imgui PRIVATE
    ${imgui_SOURCE_DIR}/imconfig.h
    ${imgui_SOURCE_DIR}/imgui.h
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_internal.h
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_link_libraries(imgui PRIVATE 
    glfw
    ${CMAKE_DL_LIBS}
)

add_library(objloader INTERFACE)
target_include_directories(objloader INTERFACE ${objloader_SOURCE_DIR}/Source)
target_sources(objloader INTERFACE ${objloader_SOURCE_DIR}/Source/OBJ_Loader.h)

add_library(stb_image INTERFACE)
target_sources(stb_image INTERFACE ${stb_SOURCE_DIR}/stb_image.h)
target_include_directories(stb_image INTERFACE ${stb_SOURCE_DIR})


# ======================= Program =======================
add_executable(WaterShader)

target_sources(WaterShader PRIVATE
${WaterShader_SOURCE_DIR}/src/utils/camera.hpp
${WaterShader_SOURCE_DIR}/src/utils/camera.cpp
${WaterShader_SOURCE_DIR}/src/utils/framebuffer.hpp
    ${WaterShader_SOURCE_DIR}/src/utils/framebuffer.cpp
    ${WaterShader_SOURCE_DIR}/src/utils/light.hpp
    ${WaterShader_SOURCE_DIR}/src/utils/material.hpp
    ${WaterShader_SOURCE_DIR}/src/utils/mesh.hpp
    ${WaterShader_SOURCE_DIR}/src/utils/mesh.cpp
    ${WaterShader_SOURCE_DIR}/src/utils/shader_program.hpp
    ${WaterShader_SOURCE_DIR}/src/utils/shader_program.cpp
    ${WaterShader_SOURCE_DIR}/src/utils/stb_image.c
    ${WaterShader_SOURCE_DIR}/src/utils/texture2d.hpp
    ${WaterShader_SOURCE_DIR}/src/utils/texture2d.cpp
    ${WaterShader_SOURCE_DIR}/src/utils/textureCube.hpp
    ${WaterShader_SOURCE_DIR}/src/utils/textureCube.cpp
    ${WaterShader_SOURCE_DIR}/src/main.cpp
)

target_include_directories(WaterShader PRIVATE
    ${WaterShader_SOURCE_DIR}
)

target_link_libraries(WaterShader PRIVATE
    glfw
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    glm
    objloader
    stb_image
    imgui
)