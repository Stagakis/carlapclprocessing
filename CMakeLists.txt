cmake_minimum_required(VERSION 3.14)

project(MyOpengl)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

########### Windows may not have pthreads, use whatever is available
if (WIN32)
        add_compile_definitions(WINDOWS)
        message(STATUS "Using Win32 for threading")
        option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)
endif (WIN32)

########### TBB Lib for par_unseq policies
find_package(TBB QUIET)
if(NOT TBB_FOUND)
        message(STATUS "TBB was not found with find_package, trying with using deps/TBB/")
        set(TBB_BUILD_TESTS OFF CACHE BOOL "Save compiling" FORCE)
        #set(TBB_BUILD_STATIC OFF CACHE BOOL "Save compiling" FORCE)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/TBB/)

endif()

########### GLFW import
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/)

########### GLAD import (manual)
set(GLAD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/deps/glad)
add_library(glad ${GLAD_PATH}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_PATH}/include/)

########### GLM import
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/glm/)

########### ImGui import (manual)
set(IMGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui)
add_library(imgui ${IMGUI_PATH}/imgui.cpp
        ${IMGUI_PATH}/imgui_draw.cpp
        ${IMGUI_PATH}/imgui_demo.cpp
        ${IMGUI_PATH}/imgui_widgets.cpp
        ${IMGUI_PATH}/imgui_tables.cpp
        ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp
        ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp)
target_compile_definitions(imgui PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GLAD)
target_include_directories(imgui PUBLIC ${IMGUI_PATH}/ ${IMGUI_PATH}/examples/ ${IMGUI_PATH}/backends/)
target_link_libraries(imgui glad glfw)

add_executable(MyOpengl
        src/main.cpp
        src/Pointcloud.cpp
        src/ImuParser.cpp
        src/WindowEventPublisher.cpp
        src/Camera.cpp
        src/Application.cpp
        src/ImageDrawable.cpp 
        src/TransformParser.cpp 
        src/SteeringParser.cpp 
        src/ImguiManager.cpp 
        src/helpers.cpp
        src/Ego.cpp
        src/Server.cpp
        )

target_include_directories(MyOpengl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_include_directories(MyOpengl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/stb/)

if(CMAKE_COMPILER_IS_GNUCXX)
        message(STATUS "GCC detected, adding compile flags")
        target_compile_options(MyOpengl PRIVATE -Wall -Wextra -pedantic-errors)
else()
        target_compile_options(MyOpengl PRIVATE -pedantic-errors)
endif()

target_link_libraries(MyOpengl glfw glad glm imgui tbb)

#Create links to the shader files
file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragmentShader.shader ${CMAKE_CURRENT_BINARY_DIR}/fragmentShader.shader)
file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/shaders/KNearest_fragment.shader ${CMAKE_CURRENT_BINARY_DIR}/KNearest_fragment.shader)
file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertexShader.shader ${CMAKE_CURRENT_BINARY_DIR}/vertexShader.shader)
