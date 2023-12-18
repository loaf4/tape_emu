add_subdirectory(tape_emu)

set(PROJECT_SRC ${CMAKE_CURRENT_LIST_DIR}/main.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SRC})

target_link_libraries(${PROJECT_NAME} PRIVATE TapeDrive)
