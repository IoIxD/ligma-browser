cmake_minimum_required(VERSION 3.0)
project(ligma-browser LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE sources   
    src/**/**/**/*.c src/**/**/**/*.h src/**/**/**/*.cpp src/**/**/**/*.hpp 
    src/**/**/*.c src/**/**/*.h src/**/**/*.cpp src/**/**/*.hpp 
    src/**/*.c src/**/*.h src/**/*.cpp src/**/*.hpp 
    src/*.c src/*.h src/*.cpp src/*.hpp
)
file(GLOB_RECURSE data resources/*)


add_compile_definitions(PLATFORM_EMBED)
add_compile_definitions(GRAPHICS_API_OPENGL_33)
add_compile_definitions(BUILD_LIBTYPE_SHARED)
add_compile_definitions(EPOXY)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED QUIET gtk+-3.0 gtkmm-3.0)
pkg_check_modules(glfw3 REQUIRED QUIET )
pkg_check_modules(glew REQUIRED QUIET )
pkg_check_modules(glad REQUIRED QUIET )
pkg_check_modules(curl REQUIRED QUIET )
PKG_CHECK_MODULES(PC_LIBSOUP REQUIRED QUIET libsoup-3.0)

FIND_PATH(LIBSOUP_INCLUDE_DIRS
    NAMES libsoup/soup.h
    HINTS ${PC_LIBSOUP_INCLUDEDIR}
          ${PC_LIBSOUP_INCLUDE_DIRS}
    PATH_SUFFIXES libsoup-3.0
)

FIND_LIBRARY(LIBSOUP_LIBRARIES
    NAMES soup-3.0
    HINTS ${PC_LIBSOUP_LIBDIR}
          ${PC_LIBSOUP_LIBRARY_DIRS}
)

# i don't even care anymore i fucking hate c and c++ and cmake and if this isn't where your webkitgtk is then too bad!!
include_directories(/usr/include/webkitgtk-4.1)
link_directories(/usr/lib/webkit2gtk-4.1)
add_link_options(-lwebkit2gtk-4.1 -lepoxy -lGL)

add_definitions(-Wno-incompatible-pointer-types -Wno-int-conversion)
include_directories("include/")

message("Found OpenGL " ${OPENGL_VERSION_STRING})
message("Found GTK3 " ${GTK3_VERSION_STRING})
message("Found GLFW " ${GLFW_VERSION_STRING})
message("Found GLEW " ${GLEW_VERSION_STRING})
message("Found glad " ${glad_VERSION_STRING})
message("Found libsoup " ${LIBSOUP_VERSION_STRING})
message("Found Webkit " ${WEBKIT_VERSION_STRING})
message("Found Curl " ${CURL_VERSION_STRING})
message("Found epoxy " ${epoxy_VERSION_STRING})

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GTKMM_INCLUDE_DIRS})
include_directories(${GTK3_INCLUDE_DIRS})
include_directories(${GTK_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${glad_INCLUDE_DIRS})
include_directories(${LIBSOUP_INCLUDE_DIRS})
include_directories(${WEBKIT_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})
include_directories(${epoxy_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${sources} ${data}
        src/GUI.cpp
        src/GUI.h)
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib )
target_compile_definitions(${PROJECT_NAME} PRIVATE ${GTKMM_CFLAGS_OTHER})
target_link_directories(${PROJECT_NAME} PRIVATE ${GTKMM_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${epoxy_LIBRARIES} curl ${WEBKIT_LIBRARIES} ${LIBSOUP_LIBRARIES} ${glad_LIBRARIES} ${GLEW_LIBRARIES} ${GTK3_LIBRARIES} ${GTKMM_LIBRARIES} ${OPENGL_LIBRARIES} glfw ${glad_LIBRARY})


set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_OPTIONS -ffunction-sections)

file(COPY ${data} DESTINATION resources)