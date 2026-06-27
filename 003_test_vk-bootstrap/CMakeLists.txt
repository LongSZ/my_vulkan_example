set(SOURCES
    test_vk-bootstrap.cpp
)

add_executable(003_test_vk-bootstrap ${SOURCES})

add_subdirectory(
    ${CMAKE_SOURCE_DIR}/vk-bootstrap 
    ${CMAKE_BINARY_DIR}/vk-bootstrap/build
)

target_link_libraries(003_test_vk-bootstrap PRIVATE 
    Vulkan::Vulkan
    glfw
    glm::glm
    vk-bootstrap::vk-bootstrap
)

set_target_properties(003_test_vk-bootstrap PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
)
install(DIRECTORY  "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION bin)

set(DLL_PATHS
    "${CMAKE_SOURCE_DIR}/3partylib/GLFW/bin/"
)

set_debug_paths(003_test_vk-bootstrap ${DLL_PATHS})

foreach(DLL_PATH ${DLL_PATHS})
    install(DIRECTORY  ${DLL_PATH} DESTINATION bin)
endforeach()

install(TARGETS 003_test_vk-bootstrap RUNTIME DESTINATION bin)