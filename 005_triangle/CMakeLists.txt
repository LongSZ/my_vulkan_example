file(GLOB SOURCES 
    "*.cpp"
    "*.h"
)

add_executable(005_triangle ${SOURCES})

target_link_libraries(005_triangle PRIVATE 
    Vulkan::Vulkan
    glfw
    glm::glm
    log4cplus::log4cplusSU
    glslang::glslang
    glslang::SPIRV
    glslang::OSDependent 
    glslang::MachineIndependent 
    glslang::GenericCodeGen
    glslang::glslang-default-resource-limits
)

set_target_properties(005_triangle PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
)
install(DIRECTORY  "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION bin)

set(DLL_PATHS
    "${CMAKE_SOURCE_DIR}/3partylib/GLFW/bin/"
    "${CMAKE_SOURCE_DIR}/3partylib/glslang/bin"
)

set_debug_paths(005_triangle ${DLL_PATHS})

foreach(DLL_PATH ${DLL_PATHS})
    install(DIRECTORY  ${DLL_PATH} DESTINATION bin)
endforeach()

install(TARGETS 005_triangle RUNTIME DESTINATION bin)