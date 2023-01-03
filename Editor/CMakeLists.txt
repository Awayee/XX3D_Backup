# CMakeList.txt : CMake project for xxEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(TARGET_NAME "Editor")

file(GLOB_RECURSE H_FILES "*.h")
file(GLOB_RECURSE CPP_FILES "*.cpp")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${H_FILES} ${CPP_FILES})

add_executable(${TARGET_NAME} ${H_FILES} ${CPP_FILES})
target_link_libraries(${TARGET_NAME} "Source")
target_include_directories(${TARGET_NAME} PRIVATE ${SOURCE_DIR})

#stb image
target_include_directories(${TARGET_NAME} PRIVATE ${THIRD_PARTY}/stb_image)

#imgui
target_link_libraries(${TARGET_NAME} "imgui")