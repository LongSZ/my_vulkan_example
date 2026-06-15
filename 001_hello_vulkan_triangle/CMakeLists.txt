set(SOURCES
    hello_vulkan_triangle.cpp
)

add_executable(001_hello_vulkan_triangle ${SOURCES})

target_link_libraries(001_hello_vulkan_triangle PRIVATE 
    Vulkan::Vulkan
    glfw
    glm::glm
    tinyobjloader::tinyobjloader
)

set_target_properties(001_hello_vulkan_triangle PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
)
install(DIRECTORY  "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION bin)

set(DLL_PATHS
    "${CMAKE_SOURCE_DIR}/3partylib/GLFW/bin/"
)

set_debug_paths(001_hello_vulkan_triangle ${DLL_PATHS})

foreach(DLL_PATH ${DLL_PATHS})
    install(DIRECTORY  ${DLL_PATH} DESTINATION bin)
endforeach()

install(TARGETS 001_hello_vulkan_triangle RUNTIME DESTINATION bin)