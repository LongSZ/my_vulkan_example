set(SOURCES
    color_cube_with_camera.cpp
    
    camera.h
    cube_data.h
    SampleBase.h
    SampleBase.cpp
    samples_platform.h
    util.h
    util.cpp
    util_init.h
    util_init.cpp
)

add_executable(002_color_cube_with_camera ${SOURCES})

target_include_directories(002_color_cube_with_camera PRIVATE 
    ${CMAKE_SOURCE_DIR}/3partylib/stb
)

target_link_libraries(002_color_cube_with_camera PRIVATE 
    Vulkan::Vulkan
    glfw
    glm::glm
    glslang::glslang
    glslang::SPIRV
    glslang::OSDependent 
    glslang::MachineIndependent 
    glslang::GenericCodeGen
)

# 放在 target_link_libraries 之后, 查找 include 路径
# if(TARGET glslang::SPIRV)
#     get_target_property(INC_DIRS glslang::SPIRV INTERFACE_INCLUDE_DIRECTORIES)
#     message(STATUS "=== glslang::SPIRV Include Dirs: ${INC_DIRS} ===")
# else()
#     message(FATAL_ERROR "Target glslang::SPIRV does not exist! Did you call find_package?")
# endif()


#set_target_properties(002_color_cube_with_camera PROPERTIES
#    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
#)

install(DIRECTORY  "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION bin)

set(DLL_PATHS
    "${CMAKE_SOURCE_DIR}/3partylib/GLFW/bin/"
    "${CMAKE_SOURCE_DIR}/3partylib/glslang/bin"
)

set_debug_paths(002_color_cube_with_camera ${DLL_PATHS})

foreach(DLL_PATH ${DLL_PATHS})
    install(DIRECTORY  ${DLL_PATH} DESTINATION bin)
endforeach()

install(TARGETS 002_color_cube_with_camera RUNTIME DESTINATION bin)