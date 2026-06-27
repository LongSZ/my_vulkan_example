cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(my_vulkan_example)

list(APPEND CMAKE_PREFIX_PATH 
    "${CMAKE_SOURCE_DIR}/3partylib/tinyobjloader/lib/tinyobjloader/cmake"
    "${CMAKE_SOURCE_DIR}/3partylib/GLFW/lib/cmake/glfw3"
    "${CMAKE_SOURCE_DIR}/3partylib/glm/share/glm"
    "${CMAKE_SOURCE_DIR}/3partylib/glslang/lib/cmake/glslang"
    "${CMAKE_SOURCE_DIR}/3partylib/glslang/SPIRV-Tools-opt/cmake"
    "${CMAKE_SOURCE_DIR}/3partylib/glslang/SPIRV-Tools/cmake"
    "${CMAKE_SOURCE_DIR}/3partylib/log4cplus/lib/cmake/log4cplus"
)

find_package (Vulkan REQUIRED)
find_package (glfw3 REQUIRED)
find_package (glm REQUIRED)
find_package (tinyobjloader REQUIRED)
find_package (glslang CONFIG REQUIRED)
find_package (log4cplus CONFIG REQUIRED)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_definitions(-DROOT_DIR="${CMAKE_SOURCE_DIR}")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Default install path" FORCE)
endif()

# 定义一个函数，用于批量添加 DLL 搜索路径到 VS 调试环境
function(set_debug_paths target_name)
    set(DLL_PATHS ${ARGN})

    if(NOT DLL_PATHS)
        return()
    endif()

    get_target_property(CURRENT_ENV ${target_name} VS_DEBUGGER_ENVIRONMENT)
    if(NOT CURRENT_ENV)
        set(CURRENT_ENV "")
    endif()

    string(JOIN ";" DLL_PATH_STRING ${DLL_PATHS})

    set(NEW_ENV "PATH=$<TARGET_FILE_DIR:${target_name}>;${DLL_PATH_STRING};%PATH%")
    
    set_target_properties(${target_name} PROPERTIES
        VS_DEBUGGER_ENVIRONMENT "${NEW_ENV}"
    )
endfunction()

add_subdirectory(001_hello_vulkan_triangle)
add_subdirectory(002_color_cube_with_camera)
add_subdirectory(vk-bootstrap )
add_subdirectory(003_test_vk-bootstrap_triangle)
add_subdirectory(004_test_vk-bootstrap_compute)
add_subdirectory(005_triangle)