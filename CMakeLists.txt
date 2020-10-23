cmake_minimum_required(VERSION 3.14)

project(MyOpengl)

########### GLFW import code
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/)

########### GLAD import code
set(GLAD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/deps/glad)
add_library(glad ${GLAD_PATH}/src/glad.c )
target_include_directories(glad PUBLIC ${GLAD_PATH}/include/)

########### GLM import code
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/glm/)

########### Dear ImGui import code
set(IMGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui)
add_library(imgui ${IMGUI_PATH}/imgui.cpp
        ${IMGUI_PATH}/imgui_draw.cpp
        ${IMGUI_PATH}/imgui_demo.cpp
        ${IMGUI_PATH}/imgui_widgets.cpp
        ${IMGUI_PATH}/examples/imgui_impl_glfw.cpp
        ${IMGUI_PATH}/examples/imgui_impl_opengl3.cpp)
target_compile_definitions(imgui PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GLAD)
target_include_directories(imgui PUBLIC ${IMGUI_PATH}/ ${IMGUI_PATH}/examples/)
target_link_libraries(imgui glad glfw)


message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(MyOpengl
        src/main.cpp
        src/Pointcloud.cpp
        src/CarlaImuParser.cpp
        src/WindowEventPublisher.cpp
        src/Camera.cpp
        src/Application.cpp
        src/ImageDrawable.cpp src/TransformParser.cpp include/TransformParser.h deps/stb/stb_image_write.h)

target_compile_options(MyOpengl PRIVATE -Wall -Wextra -pedantic-errors)
target_include_directories(MyOpengl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_include_directories(MyOpengl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/stb/)

target_link_libraries(MyOpengl glfw glad glm imgui)

#Create links to the shader files
file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragmentShader.shader ${CMAKE_CURRENT_BINARY_DIR}/fragmentShader.shader)
file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/shaders/KNearest_fragment.shader ${CMAKE_CURRENT_BINARY_DIR}/KNearest_fragment.shader)
file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertexShader.shader ${CMAKE_CURRENT_BINARY_DIR}/vertexShader.shader)
