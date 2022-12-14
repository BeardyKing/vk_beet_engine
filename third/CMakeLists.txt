#======== GLFW ========#

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw)

#======= SPDLOG =======#

set(SPDLOG_MASTER_PROJECT OFF CACHE BOOL "" FORCE)
add_subdirectory(spdlog)

#======== GLM =========#

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)

add_subdirectory(glm)

#======= ASSIMP =======#

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(assimp)

#======== STB =========#

project(stb)
add_library(stb INTERFACE)

#======= IMGUI ========#

project(imgui)
file(GLOB IMGUI_SOURCES
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_demo.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
        )
add_library(imgui ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends)
target_include_directories(imgui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include/)
target_include_directories(${PROJECT_NAME} PRIVATE ${IMGUI_SOURCES})

#======= IMGUI ========#

project(imguizmo)
file(GLOB IMGUIZMO_SOURCES
        imguizmo/GraphEditor.cpp
        imguizmo/ImCurveEdit.cpp
        imguizmo/ImGradient.cpp
        imguizmo/ImGuizmo.cpp
        imguizmo/ImSequencer.cpp
        )
add_library(imguizmo ${IMGUIZMO_SOURCES})
target_link_libraries(imguizmo PRIVATE imgui)
target_include_directories(imguizmo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imguizmo)
target_include_directories(${PROJECT_NAME} PRIVATE ${IMGUIZMO_SOURCES})

#======== VMA =========#

project(vma)
add_library(vma INTERFACE)

#==== VK_BOOTSTRAP ====#

add_subdirectory(vk-bootstrap)

