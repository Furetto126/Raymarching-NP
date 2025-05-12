cmake_minimum_required(VERSION 3.5)
project(RayMarcher LANGUAGES C CXX)

message("Using ${CMAKE_CXX_COMPILER_ID} compiler.")
if (WIN32 AND MSVC)
        message("Building in parallel.")
        add_compile_options("/cgthreads1")
        add_compile_options("/MP$ENV{NUMBER_OF_PROCESSORS}")
else ()
        add_compile_options("-Ofast")
        message("To build with ${CMAKE_GENERATOR}, it is recommended to use the --parallel or the -j flag")
endif ()

if (MSVC)
        add_compile_options("/Zc:preprocessor")
        add_compile_options("/utf-8")
endif ()

# ----------------------------------------------------------------------------------------------------------------------
# FLAGS
# ----------------------------------------------------------------------------------------------------------------------

if (NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRTD")
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------------------------------------------------------------------------------------
# DIRECTORIES
# ----------------------------------------------------------------------------------------------------------------------

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(LIBS_DIR "${CMAKE_SOURCE_DIR}/libs")
set(RESOURCES_DIR "${CMAKE_SOURCE_DIR}/resources")
set(SHADERS_DIR "${SRC_DIR}/shaders")

set(GET_PROJECT_DIR "${LIBS_DIR}/GetProject")
set(GLFW_DIR "${LIBS_DIR}/glfw")
set(GLM_DIR "${LIBS_DIR}/glm")
set(STB_DIR "${LIBS_DIR}/stb")
set(GLAD_DIR "${LIBS_DIR}/glad")
set(ASSIMP_DIR "${LIBS_DIR}/assimp")
set(FMT_DIR "${LIBS_DIR}/fmt")
set(YAML_DIR "${LIBS_DIR}/yaml-cpp")

# ----------------------------------------------------------------------------------------------------------------------
# PROJECT
# ----------------------------------------------------------------------------------------------------------------------

# Sources files
file(GLOB_RECURSE SOURCES
        "${SRC_DIR}/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
# target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_EXPOSE_NATIVE_WIN32)

# ----------------------------------------------------------------------------------------------------------------------
# SHADERS
# ----------------------------------------------------------------------------------------------------------------------

if (NOT EXISTS "${CMAKE_BINARY_DIR}/force-shader-copy.stamp")
        file(WRITE "${CMAKE_BINARY_DIR}/force-shader-copy.stamp")
endif ()

file(GLOB SHADERS
        "${RESOURCES_DIR}/*.vert"
        "${RESOURCES_DIR}/*.frag"
)

# Must be run before the target 'shaders_copy'
add_custom_target(check_copy COMMAND ${CMAKE_COMMAND} -P "${SHADERS_DIR}/check_shaders_copied.cmake")
add_dependencies(${PROJECT_NAME} check_copy)

add_custom_command(OUTPUT "shaders.stamp"
        COMMAND ${CMAKE_COMMAND} -P "${SHADERS_DIR}/copy_shaders.cmake"
        COMMAND ${CMAKE_COMMAND} -E touch "shaders.stamp"
        DEPENDS "${CMAKE_BINARY_DIR}/force-shader-copy.stamp"
        DEPENDS ${SHADERS})

add_custom_target(shaders_copy DEPENDS "shaders.stamp")
add_dependencies(${PROJECT_NAME} shaders_copy)

# ----------------------------------------------------------------------------------------------------------------------
# DEPENDENCIES
# ----------------------------------------------------------------------------------------------------------------------

if (NOT EXISTS ${LIBS_DIR})
        file(MAKE_DIRECTORY ${LIBS_DIR})
endif ()

if (NOT EXISTS ${GET_PROJECT_DIR})
        execute_process(COMMAND git clone "https://github.com/Storterald/GetProject" ${GET_PROJECT_DIR})
else ()
        execute_process(COMMAND git pull WORKING_DIRECTORY ${GET_PROJECT_DIR})
endif ()

include("${GET_PROJECT_DIR}/GetProject.cmake")

get_project(
        TARGET ${PROJECT_NAME}
        DIRECTORY ${LIBS_DIR}
        GIT_REPOSITORY "https://github.com/glfw/glfw.git"
        INSTALL_ENABLED ON
        VERSION LATEST
        BUILD_ARGS
                -DGLFW_BUILD_EXAMPLES=OFF
                -DGLFW_BUILD_TESTS=OFF
)

target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_link_libraries(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX}")

get_project(
        TARGET ${PROJECT_NAME}
        GIT_REPOSITORY "https://github.com/g-truc/glm.git"
        DIRECTORY ${LIBS_DIR}
        VERSION LATEST
        INSTALL_ENABLED ON
        BUILD_ARGS
                -DGLM_BUILD_TESTS=OFF
                -DBUILD_SHARED_LIBS=OFF
)
target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_FORCE_RADIANS)
target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_ENABLE_EXPERIMENTAL)
target_include_directories(${PROJECT_NAME} PRIVATE "${GLM_DIR}/include")

get_project(
        TARGET ${PROJECT_NAME}
        GIT_REPOSITORY "https://github.com/jbeder/yaml-cpp.git"
        DIRECTORY ${LIBS_DIR}
        INSTALL_ENABLED ON
        VERSION LATEST
)

set(YAML_NAME "yaml-cpp")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(YAML_NAME "${YAML_NAME}d")
endif ()

target_compile_definitions(${PROJECT_NAME} PRIVATE YAML_CPP_STATIC_DEFINE)
target_include_directories(${PROJECT_NAME} PRIVATE "${YAML_DIR}/include")
target_link_libraries(${PROJECT_NAME} PRIVATE "${YAML_DIR}/lib/${YAML_NAME}.lib")

get_project(
        TARGET ${PROJECT_NAME}
        GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
        DIRECTORY ${LIBS_DIR}
        VERSION LATEST
        INSTALL_ENABLED ON
        BUILD_ARGS
                -DFMT_TEST=OFF
)

set(FMT_NAME "fmt")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(FMT_NAME "${FMT_NAME}d")
endif ()

target_include_directories(${PROJECT_NAME} PRIVATE "${FMT_DIR}/include")
target_link_libraries(${PROJECT_NAME} PRIVATE "${FMT_DIR}/lib/${FMT_NAME}.lib")

file(DOWNLOAD
        "https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h"
        "${STB_DIR}/include/stb/stb_image.h"
)
file(DOWNLOAD
        "https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image_write.h"
        "${STB_DIR}/include/stb/stb_image_write.h"
)
target_compile_definitions(${PROJECT_NAME} PRIVATE STB_IMAGE_IMPLEMENTATION)
target_include_directories(${PROJECT_NAME} PRIVATE "${STB_DIR}/include")

get_project(
        DOWNLOAD_ONLY ON
        DIRECTORY ${LIBS_DIR}
        LIBRARY_NAME "glad"
        URL "https://glad.dav1d.de/generated/tmpw_uejb6yglad/glad.zip")

target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}")
target_sources(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/glad.c")