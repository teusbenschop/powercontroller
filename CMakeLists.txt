# Copyright (©) 2026-2026 Teus Benschop.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


# Set the minimum CMake version required.
cmake_minimum_required(VERSION 3.25)


# Print info.
message(STATUS "Processing ${CMAKE_CURRENT_SOURCE_DIR}")


# Start a project by setting the project name and version.
project(powercontroller VERSION 0.1.000)
message(STATUS "Power controller version ${powercontroller_VERSION}")


if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILER_GCC ON)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(COMPILER_CLANG ON)
endif()


# Handle the install prefix.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "/usr")
endif()
option(CMAKE_INSTALL_PREFIX "Install prefix" "/usr")
message(STATUS "Install prefix is ${CMAKE_INSTALL_PREFIX} (change: $cmake -DCMAKE_INSTALL_PREFIX=path)")
# Set it during install: $ cmake --install . --prefix "/path"

# Compiler to emit info for the debugger.
add_compile_options( -g)

# Enable compiler warnings.
add_compile_options(-Wall -Wextra -Wpedantic -Wno-unknown-pragmas -Wno-pragmas -Wshadow -Warray-bounds -Wcast-align -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wdelete-non-virtual-dtor -Weffc++ -Wextra-semi -Wfloat-equal -Wformat=2 -Wmissing-include-dirs -Wmissing-noreturn -Wnull-dereference -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wsign-promo -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wuninitialized -Wunused-but-set-parameter -Wunused-macros -Wunused-parameter -Wvla -Wzero-as-null-pointer-constant -Wmismatched-tags)
if (COMPILER_GCC)
    add_compile_options(-Wcatch-value=2 -Wconditionally-supported -Wduplicated-branches -Wduplicated-cond -Wformat-overflow=2 -Wlogical-op -Wnoexcept -Wno-non-template-friend -Wrestrict -Wstrict-null-sentinel -Wuseless-cast -Wcomma-subscript -Wredundant-tags -Wvolatile)
endif()
if (COMPILER_CLANG)
endif()

# Compilers on some systems may treat warnings as errors.
# Pass a flag to the compiler to treat a warning not as an error.
add_compile_options(-fno-var-tracking -Wno-error)

add_link_options(-Wall -lpthread -L. -g -rdynamic -ldl)

# The C++ standard to use, and it is mandatory.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# Finding and using libraries.
find_package(PkgConfig REQUIRED)
include(CheckIncludeFile)

# Build the app's library, the kernel of the system.
add_library(library STATIC
        library.cpp
        dsmr/datagram.cpp
        exception.h
)


# The service executable.
add_executable(powercontrold main.cpp)
target_link_libraries(powercontrold library)


# Unit testing
add_executable(unittest
        unittests/main.cpp
        unittests/dsmr.cpp
)
target_link_libraries(unittest library)


# The include directories for all items
include_directories( . )


# Where to look for the built libraries.
link_directories(build)


# Finding and using libraries.
find_package(PkgConfig REQUIRED)
include(CheckIncludeFile)


pkg_check_modules(LIBCURL REQUIRED libcurl>=7.20.0)
target_include_directories(powercontrold PUBLIC ${LIBCURL_INCLUDE_DIRS})
target_compile_options(powercontrold PUBLIC ${LIBCURL_CFLAGS_OTHER})
target_link_directories(powercontrold PUBLIC ${LIBCURL_LIBRARY_DIRS})
target_link_libraries(unittest ${LIBCURL_LIBRARIES})


pkg_check_modules(GTEST gtest>=1.10.0)
target_include_directories(unittest PUBLIC ${GTEST_INCLUDE_DIRS})
target_compile_options(unittest PUBLIC ${GTEST_CFLAGS_OTHER})
target_link_directories(unittest PUBLIC ${GTEST_LIBRARY_DIRS})
target_link_libraries(unittest ${GTEST_LIBRARIES})


set(PACKAGE_DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/powercontroller")

set(PACKAGE_PREFIX_DIR "${CMAKE_INSTALL_PREFIX}")


# Write the variables in the template to config.h.
configure_file(config.h.in ${PROJECT_SOURCE_DIR}/config.h)


# Installation.
install(TARGETS powercontrold DESTINATION bin)


# Create tarball with target "dist"
set(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION})
add_custom_target(dist
        COMMAND git archive --prefix=${ARCHIVE_NAME}/ HEAD --format=tar.gz ":(exclude).idea" ":(exclude).github" ":(exclude).gitignore" ":(exclude)cloud-macos.entitlements" > ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.gz
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        VERBATIM
)
