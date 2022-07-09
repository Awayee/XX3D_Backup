# CMakeList.txt : CMake project for xxEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)


project (xxEngine)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

set(THIRD_PARTY ${xxEngine_SOURCE_DIR}/ThirdParty)
set(SOURCE_DIR ${xxEngine_SOURCE_DIR}/Source)

macro (DEPENDENCY TARGET_NAME DEPENDENCY_NAME)
add_dependencies(${TARGET_NAME} ${DEPENDENCY_NAME})
target_link_libraries(${TARGET_NAME} PUBLIC ${DEPENDENCY_NAME})
endmacro(DEPENDENCY)

add_subdirectory(Source/Core)
add_subdirectory(Source/Render)
add_subdirectory(Source/Engine)
