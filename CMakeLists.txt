﻿##################################################
# This file has been authored by and belongs to: #
# Copyright (c) 2022 Daniel Fuerlinger           #
# All rights reserved.                           #
##################################################

cmake_minimum_required (VERSION 3.9)

# Sets up the toolchain for using vcpkg to download the packages in vcpkg.json
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")
endif()

# vcpkg triplet to specify the hostadd
# (ex. x64-windows, x64-osx, etc)
if(DEFINED ENV{VCPKG_DEFAULT_HOST_TRIPLET} AND NOT DEFINED VCPKG_HOST_TRIPLET)
	set(VCPKG_HOST_TRIPLET "$ENV{VCPKG_DEFAULT_HOST_TRIPLET}" CACHE STRING "vcpkg host triplet")
endif()

# vcpkg triplet to specify the architecture, CRT linking, and whether to make a static/dynamic lib
# (ex. x64-windows-static, x64-windows-static-md)
if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
	set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "vcpkg target triplet")
endif()

# We want to export an executable that is dynamically linked against the CRT
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" CACHE STRING "msvc runtime")

project("cpp-opengl-game-framework"
	VERSION 1.0.0
	DESCRIPTION "A starter project for making OpenGL games or simulations with C++"
	HOMEPAGE_URL "https://github.com/Fuerdinger/cpp-opengl-game-framework"
	LANGUAGES CXX)

# The project currently doesn't use C++17 features but it may in the future
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# All these packages are available through vcpkg, and are specified in vcpkg.json
find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(SFML COMPONENTS audio REQUIRED)
find_package(stb REQUIRED)

# Specify source/header files and create the executable
# These are specified as vars to keep them separate from ${SOURCES} and ${INCLUDE_DIRECTORIES}
set(SRC_FILES
		"src/ext/stb_image.cpp"
		"src/ext/stb_image_write.cpp"
	"src/Image.cpp"
	"src/main.cpp"
	"src/Scene.cpp"
	"src/Sound.cpp"
	"src/Util.cpp"
	"src/Window.cpp"
)
set(INCLUDE_DIRS
	"include"
)
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
target_precompile_headers(${PROJECT_NAME} PRIVATE "include/pch.h")

# Windows release builds should not be console apps
set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)

# These libraries have source code which must be linked
target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-audio)

# These libraries are header only
target_include_directories(${PROJECT_NAME} PRIVATE glm)
target_include_directories(${PROJECT_NAME} PRIVATE stb)

# Use doxygen to generate the documentation
find_package(Doxygen)
if (DOXYGEN_FOUND)
	set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/docs")
	set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_CURRENT_LIST_DIR}/README.md")
	doxygen_add_docs(doxy
		${INCLUDE_DIRS}
		${SRC_FILES}
		"${CMAKE_CURRENT_LIST_DIR}/README.md"
		ALL
	)
endif()

# Copy asset data to the executable directory
# Deleting a file in root/data/ and then building again will NOT delete it from build/data/
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/data/")
	install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/data/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/untracked_data/")
	install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/untracked_data/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# Copy runtimes from System32 folder for Release builds
if(${CMAKE_BUILD_TYPE} MATCHES "Rel")
	if(DEFINED ENV{WINDIR})
		file(COPY
			"$ENV{WINDIR}/System32/VCRUNTIME140.dll"
			"$ENV{WINDIR}/System32/VCRUNTIME140_1.dll"
			"$ENV{WINDIR}/System32/MSVCP140.dll"
			DESTINATION
			"${CMAKE_CURRENT_BINARY_DIR}"
			)
	endif()
endif()