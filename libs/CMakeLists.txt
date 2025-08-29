message(STATUS "Building libraries...")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libs" FORCE)


if(NOT TARGET glfw)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "..." FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "..." FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "..." FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "..." FORCE)
    add_subdirectory(glfw)
endif()

if(NOT TARGET glad)
    add_library(glad INTERFACE)
    target_include_directories(glad INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)
endif()

if(NOT TARGET glm)
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/glm)
endif()

add_library(reflibs INTERFACE)
target_link_libraries(reflibs INTERFACE glad glfw glm)

if(UNIX AND NOT APPLE)
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
    target_link_libraries(reflibs INTERFACE
        OpenGL::GL
        ${X11_LIBRARIES}
    )
endif()

target_include_directories(reflibs INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/glm
)
