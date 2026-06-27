set(SOURCES
    test_vk-bootstrap_compute.cpp
)

add_executable(004_test_vk-bootstrap_compute ${SOURCES})

target_link_libraries(004_test_vk-bootstrap_compute PRIVATE 
    Vulkan::Vulkan
    glfw
    glm::glm
    vk-bootstrap::vk-bootstrap
)

set_target_properties(004_test_vk-bootstrap_compute PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
)
install(DIRECTORY  "${CMAKE_CURRENT_SOURCE_DIR}/assets/" DESTINATION bin)

set(DLL_PATHS
    "${CMAKE_SOURCE_DIR}/3partylib/GLFW/bin/"
)

set_debug_paths(004_test_vk-bootstrap_compute ${DLL_PATHS})

foreach(DLL_PATH ${DLL_PATHS})
    install(DIRECTORY  ${DLL_PATH} DESTINATION bin)
endforeach()

install(TARGETS 004_test_vk-bootstrap_compute RUNTIME DESTINATION bin)