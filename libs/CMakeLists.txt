cmake_minimum_required(VERSION 3.15)  # for CMAKE_MSVC_RUNTIME_LIBRARY

project("C API for Source SDK 2013")

if(NOT DEFINED SOURCE_SDK_2013)
	message(FATAL_ERROR "SDK path not set, use '-D SOURCE_SDK_2013=.../source-sdk-2013/mp/src'")
endif()

# Include Source SDK.
include_directories(${SOURCE_SDK_2013}/common)
include_directories(${SOURCE_SDK_2013}/public)
include_directories(${SOURCE_SDK_2013}/public/tier0)
include_directories(${SOURCE_SDK_2013}/public/tier1)
include_directories(${SOURCE_SDK_2013}/public/tier2)
include_directories(${SOURCE_SDK_2013}/game/client)
include_directories(${SOURCE_SDK_2013}/game/shared)

# Link SDK libraries.
link_libraries(${SOURCE_SDK_2013}/lib/public/tier0.lib)
link_libraries(${SOURCE_SDK_2013}/lib/public/tier1.lib)
link_libraries(${SOURCE_SDK_2013}/lib/public/tier2.lib)

# Build with static runtime (required to link against SDK libs).
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)

function(LIBRARY NAME)
	add_library(${NAME} SHARED ${NAME}.cpp lib.hpp)
  # Copy built DLL to mod directory.
	add_custom_command(
		TARGET ${NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			$<TARGET_FILE:${NAME}>
			"${PROJECT_SOURCE_DIR}/../mod/lua/sdk"
	)
endfunction()

library(cvar)
