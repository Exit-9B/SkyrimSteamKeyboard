cmake_minimum_required(VERSION 3.24)

project(
	SteamInputHelper
	VERSION 1.0.0
	LANGUAGES CXX
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_IGNORE_PATH "${PROJECT_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}/tools/SKSE-CMakeModules/Modules"
)
include(SKSEPlugin)

set(SKSE_USE_XBYAK ON)

SKSEPlugin_Add(
	${PROJECT_NAME}
	SOURCE_DIR src
	SOURCES
		.clang-format
		.editorconfig
		README.md
		vcpkg.json
	PRECOMPILE_HEADERS
		src/PCH/PCH.h
)

set(STEAMWORKS_SDK_DIR "${PROJECT_SOURCE_DIR}/external/steamworks_sdk_134/sdk")

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		"${STEAMWORKS_SDK_DIR}/public"
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
		"${STEAMWORKS_SDK_DIR}/redistributable_bin/win64/steam_api64.lib"
)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

set(CPACK_GENERATOR "7Z")
set(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}-${PROJECT_VERSION})
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
include(CPack)