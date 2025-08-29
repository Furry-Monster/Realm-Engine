set(TARGET_NAME LearnOpenGL)

file(GLOB SOURCES "*.cpp" "*.c")
file(GLOB HEADERS "*.hpp" "*.h")

if(SOURCES)
    add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})

    target_link_libraries(${TARGET_NAME} PUBLIC reflibs)

    target_include_directories(${TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

endif()
