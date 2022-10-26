cmake_minimum_required(VERSION 3.21)

project(vk_beet_engine)
add_subdirectory(third)

find_package(Vulkan REQUIRED)

#======== SOURCE ==========#

file(GLOB_RECURSE BEET_CORE_SOURCES
        core/src/*.cpp
        core/inc/**/*.h
        )

file(GLOB_RECURSE BEET_EDITOR_SOURCES
        editor/**/*.cpp
        editor/**/*.h
        )

#==========================#

add_executable(beet
        ${BEET_CORE_SOURCES}
        ${BEET_EDITOR_SOURCES}
        )

include_directories(beet
        core/inc/
        third/glfw/include/
        third/glm/
        third/stb/
        third/assimp/include/
        third/VulkanMemoryAllocator/include/
        )

target_link_libraries(beet
        glfw
        spdlog
        glm
        stb
        imgui
        imguizmo
        Vulkan::Vulkan
        assimp
        vma
        )


#======= PROPERTIES ========#

set_target_properties(beet PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
set_target_properties(beet PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dist/bin")
set_target_properties(beet PROPERTIES OUTPUT_NAME beet_build)

#==========================#

#========= SHADER =========#

#https://gist.github.com/evilactually/a0d191701cb48f157b05be7f74d79396

if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
    set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
else ()
    set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin32/glslangValidator.exe")
endif ()

#==========================#

#======= RESOURCES ========#

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist/res)

file(GLOB_RECURSE BEET_RESOURCES LIST_DIRECTORIES false "${CMAKE_SOURCE_DIR}/res/*")
add_custom_target(beet_resources ALL DEPENDS ${BEET_RESOURCES})

foreach (RESOURCE_FILE ${BEET_RESOURCES})
    get_filename_component(FILE_NAME ${RESOURCE_FILE} NAME)
    get_filename_component(PARENT_DIR ${RESOURCE_FILE} DIRECTORY)
    get_filename_component(FILE_EXT ${RESOURCE_FILE} LAST_EXT)
    string(REGEX REPLACE "^${CMAKE_SOURCE_DIR}/res" "" PARENT_DIR ${PARENT_DIR})
    set(TARGET_FILE_NAME "${CMAKE_BINARY_DIR}/dist/res${PARENT_DIR}/${FILE_NAME}")

    if (${FILE_EXT} STREQUAL ".vert" OR ${FILE_EXT} STREQUAL ".frag")
        # COMPILE SPIRV to BUILD (.vert/.frag)
        # This is the fallback until hot reloading shaders in engine
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist/res${PARENT_DIR}/)

        set(SPIRV "${TARGET_FILE_NAME}.spv")
        exec_program(
                ${GLSL_VALIDATOR}
                ARGS -V ${RESOURCE_FILE} -o ${SPIRV}
        )
    else ()
        #COPY RESOURCE FILES TO BUILD
        configure_file(${RESOURCE_FILE} ${TARGET_FILE_NAME} COPYONLY)
    endif ()
endforeach ()

#==========================#

#========= DEBUG ==========#


if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(beet PUBLIC BEET_DEBUG=1)
    target_compile_definitions(beet PUBLIC _DEBUG=1)
else ()
    target_compile_definitions(beet PUBLIC NDEBUG=1)
endif ()

#==========================#



