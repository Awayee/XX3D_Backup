set(TARGET_NAME Render)

file(GLOB_RECURSE h_files "*.h")
file(GLOB_RECURSE cpp_files "*.cpp")
add_library (${TARGET_NAME} ${h_files} ${cpp_files})

target_include_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY}/VulkanSDK/include)
target_link_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY}/VulkanSDK/lib/Win64)
target_link_libraries(${TARGET_NAME} PUBLIC ${THIRD_PARTY}/VulkanSDK/lib/Win64/vulkan-1.lib)