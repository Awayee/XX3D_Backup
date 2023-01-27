# CMakeList.txt : CMake project for xxEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(PROJECT_NAME "XX3D")

project (${PROJECT_NAME})


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

set(THIRD_PARTY ${${PROJECT_NAME}_SOURCE_DIR}/ThirdParty/)
set(SOURCE_DIR  ${${PROJECT_NAME}_SOURCE_DIR}/Source/)
set(ASSETS_DIR  ${${PROJECT_NAME}_SOURCE_DIR}/Assets/)
set(SHADER_DIR  ${${PROJECT_NAME}_SOURCE_DIR}/Shaders/)

add_definitions(
    -DASSETS_PATH="${ASSETS_DIR}/" 
    -DUSE_VMA
    -DPROJECT_NAME="${PROJECT_NAME}"
    -DSHADER_PATH="${SHADER_DIR}"
)

macro (DEPENDENCY TARGET_NAME DEPENDENCY_NAME)
add_dependencies(${TARGET_NAME} ${DEPENDENCY_NAME})
target_link_libraries(${TARGET_NAME} PUBLIC ${DEPENDENCY_NAME})
endmacro(DEPENDENCY)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory("ThirdParty")
add_subdirectory("Source")
add_subdirectory("Editor")
add_subdirectory("Runtime")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "Editor")
