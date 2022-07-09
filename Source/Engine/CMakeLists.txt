set(TARGET_NAME Engine)

add_executable(${TARGET_NAME} main.cpp)
DEPENDENCY(${TARGET_NAME} Core)
DEPENDENCY(${TARGET_NAME} Render)
target_include_directories(${TARGET_NAME} PRIVATE ${SOURCE_DIR})
