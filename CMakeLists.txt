# Copyright (c) 2016 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

# See the included README.md file for usage instructions.

# For VS2022 and Xcode 12+ support.
cmake_minimum_required(VERSION 3.21)

# Only generate Debug and Release configuration types.
set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Project name.
project(ligma-browser)


# Use folders in the resulting project files.
set_property(GLOBAL PROPERTY OS_FOLDERS ON)

# Specify the CEF distribution version.
set(CEF_VERSION "127.3.1+g6cbb30e+chromium-127.0.6533.100")

# Determine the platform.
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm")
    set(CEF_PLATFORM "linuxarm")
  elseif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
    set(CEF_PLATFORM "linuxarm64")
  elseif(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(CEF_PLATFORM "linux64")
  else()
    message(FATAL_ERROR "Linux x86 32-bit builds are discontinued.")
  endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  if("${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}" STREQUAL "ARM64")
    set(CEF_PLATFORM "windowsarm64")
  elseif(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(CEF_PLATFORM "windows64")
  else()
    set(CEF_PLATFORM "windows32")
  endif()
endif()

# Add this project's cmake/ directory to the module path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Download and extract the CEF binary distribution (executes DownloadCEF.cmake).
include(DownloadCEF)
DownloadCEF("${CEF_PLATFORM}" "${CEF_VERSION}" "${CMAKE_SOURCE_DIR}/third_party/cef")

# Add the CEF binary distribution's cmake/ directory to the module path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

# Load the CEF configuration (executes FindCEF.cmake).
find_package(CEF REQUIRED)

find_package(CURL REQUIRED) 

#
# Target configuration.
#

# Include the libcef_dll_wrapper target (executes libcef_dll/CMakeLists.txt).
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

# Allow includes relative to the current source directory.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CEF_ROOT})
include_directories(${CEF_ROOT}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)


find_package(glm REQUIRED)
find_package(GLEW REQUIRED)

include_directories(${GLM_INCLUDE_DIRS})
link_directories(${GLM_LIBRARY_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIR})

# Main executable sources.
file(GLOB_RECURSE sources
  src/*.cpp 
  src/*.hpp
  src/*.c
  src/*.h
)

file(GLOB CHROME_RESOURCES
  ${CEF_ROOT}/Resources/*
)

add_executable(${PROJECT_NAME} ${sources})

file(COPY ${CHROME_RESOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CEF_ROOT}/Resources/locales DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/locales)
add_compile_options(-DCHECK_OPENGL -DCEF_USE_SANDBOX -DNDEBUG -D_FILE_OFFSET_BITS=64 -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS)

file(COPY ${CHROME_RESOURCES} DESTINATION ${CEF_ROOT}/${CMAKE_BUILD_TYPE}/)
file(COPY ${CEF_ROOT}/Resources/locales DESTINATION ${CEF_ROOT}/${CMAKE_BUILD_TYPE}/locales)

file(COPY ${CMAKE_SOURCE_DIR}/src/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 

ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} libcef_lib libcef_dll_wrapper)
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES} ${CURL_LIBRARIES} )

include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "5.0"
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
    rs_systemtime
    GIT_REPOSITORY "https://github.com/IoIxD/rs_systemtime.git"
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(rs_systemtime)
include_directories(${RS_SYSTEMTIME_INCLUDE_DIRECTORY})

add_subdirectory("rs_image")
include_directories("rs_image/include")

target_link_libraries(${PROJECT_NAME} raylib rs_image rs_systemtime)

PRINT_CEF_CONFIG()
