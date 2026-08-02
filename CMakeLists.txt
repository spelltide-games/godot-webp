# Using the same minimum as the godot-cpp project
cmake_minimum_required(VERSION 3.17)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

##############################################
# Silence unused variable warning when specified from toolchain
if(CMAKE_C_COMPILER)
endif()

set(LIBNAME "godot-webp" CACHE STRING "The name of the library")
set(GODOT_PROJECT_DIR "demo" CACHE STRING "The directory of a Godot project folder")

set(GODOTCPP_USE_STATIC_CPP OFF CACHE BOOL "")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(GODOTCPP_DEBUG_CRT ON CACHE BOOL "")
endif()

find_package(Python3 3.10 REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(godot-cpp SYSTEM)

# Add godot-cpp's module path and include the exported functions.
# This is made available for documentation generation
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${godot-cpp_SOURCE_DIR}/cmake")
include(GodotCPPModule)

# The godot-cpp target has some of useful properties attached that can be retrieved like so.
get_target_property(GODOTCPP_SUFFIX godot::cpp GODOTCPP_SUFFIX)
get_target_property(GODOTCPP_PLATFORM godot::cpp GODOTCPP_PLATFORM)

# Now we can specify our own project which will inherit any global cmake properties or variables that have been defined.
project(${LIBNAME}
    VERSION 1.0
    DESCRIPTION "Bindings of libwebp for Godot 4.5 or later."
    HOMEPAGE_URL "https://github.com/spelltide-games/godot-webp"
    LANGUAGES CXX
)

add_library(${LIBNAME} SHARED)

file(GLOB_RECURSE ALL_SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

target_sources(${LIBNAME}
    PRIVATE
    ${ALL_SRC}
)

if(WIN32)
  set(BUILD_TOOLS_DEFAULT ON)
else()
  set(BUILD_TOOLS_DEFAULT OFF)
endif()

option(WEBP_LINK_STATIC "" ON)
option(WEBP_BUILD_ANIM_UTILS "Build animation utilities." ${BUILD_TOOLS_DEFAULT})
option(WEBP_BUILD_CWEBP "Build the cwebp command line tool." ${BUILD_TOOLS_DEFAULT})
option(WEBP_BUILD_DWEBP "Build the dwebp command line tool." ${BUILD_TOOLS_DEFAULT})
option(WEBP_BUILD_GIF2WEBP "Build the gif2webp conversion tool." OFF)
option(WEBP_BUILD_IMG2WEBP "Build the img2webp animation tool." OFF)
option(WEBP_BUILD_VWEBP "Build the vwebp viewer tool." ${BUILD_TOOLS_DEFAULT})
option(WEBP_BUILD_WEBPINFO "Build the webpinfo command line tool." ${BUILD_TOOLS_DEFAULT})
option(WEBP_BUILD_LIBWEBPMUX "Build the libwebpmux library." ON)
option(WEBP_BUILD_WEBPMUX "Build the webpmux command line tool." OFF)
option(WEBP_BUILD_EXTRAS "Build extras." OFF)
option(WEBP_BUILD_WEBP_JS "Emscripten build of webp.js." OFF)
option(WEBP_BUILD_FUZZTEST "Build the fuzztest tests." OFF)
option(WEBP_USE_THREAD "Enable threading support" ON)
option(WEBP_NEAR_LOSSLESS "Enable near-lossless encoding" ON)
add_subdirectory(libwebp)

target_link_libraries(${LIBNAME} PRIVATE
    godot-cpp
    webp
    webpdemux
)

set(GODOT_PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/${GODOT_PROJECT_DIR}/addons/godot-webp/bin/${GODOTCPP_PLATFORM}")

set_target_properties(${LIBNAME}
    PROPERTIES
    # The generator expression here prevents msvc from adding a Debug or Release subdir.
    RUNTIME_OUTPUT_DIRECTORY "$<1:${GODOT_PROJECT_BINARY_DIR}>"
    
    OUTPUT_NAME "${LIBNAME}${GODOTCPP_SUFFIX}"
)

add_custom_command(TARGET ${LIBNAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${GODOT_PROJECT_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "Copying $<TARGET_FILE_NAME:${LIBNAME}> to ${GODOT_PROJECT_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:${LIBNAME}>"
            "${GODOT_PROJECT_BINARY_DIR}/$<TARGET_FILE_NAME:${LIBNAME}>"
    COMMENT "POST_BUILD begin for ${LIBNAME}"
)
