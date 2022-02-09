# ==============================
# File:     CMakeLists.txt
# Project:  JustAddROM
#
# Copyright 2022 JustAddROM@matthiasm.com
# ==============================

cmake_minimum_required(VERSION 3.13)

project( "JustAddROM" VERSION "0.0.1" )

# ---- Configuration for all targets on all platforms

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

# ---- Configuration per platform

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" )

	set( CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO" )
	set( CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "" )

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )

	# Linux: nothing to do here

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )

	cmake_policy( SET CMP0091 NEW )
	set( CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" )

else ()

	message( FATAL_ERROR "JustAddROM is not configured to build on this platform: " ${CMAKE_SYSTEM_NAME} "." )

endif ()

#
# ==== External Dependencies ===================================================
#

#
# ---- FLTK library ------------------------------------------------------------
#

set( FLTK_SKIP_OPENGL true )
find_program( LOCAL_FLTK_FLUID_EXECUTABLE fluid
	HINTS
		${CMAKE_SOURCE_DIR}/fltk/build/bin/Release
		${CMAKE_SOURCE_DIR}/fltk/build/bin/Debug
		${CMAKE_SOURCE_DIR}/fltk/build/bin
)
find_package( FLTK REQUIRED NO_MODULE HINTS ${CMAKE_SOURCE_DIR}/fltk/build )

if ( ${LOCAL_FLTK_FLUID_EXECUTABLE} MATCHES ".*NOTFOUND" )
	message( WARNING "Fluid (FLTK User Interface Designer) not found!" )
else ()
	message( STATUS "Fluid (FLTK User Interface Designer) found at " ${LOCAL_FLTK_FLUID_EXECUTABLE})
endif ()

function ( build_with_fluid name dir )
	add_custom_command (
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/${dir}"
		COMMAND ${LOCAL_FLTK_FLUID_EXECUTABLE} -c ${name}.fl
		DEPENDS "${CMAKE_SOURCE_DIR}/${dir}/${name}.fl"
		DEPENDS "${ARGN}"
		OUTPUT "${CMAKE_SOURCE_DIR}/${dir}/${name}.cpp"
		OUTPUT "${CMAKE_SOURCE_DIR}/${dir}/${name}.h"
	)
	set_source_files_properties (
		${CMAKE_SOURCE_DIR}/${dir}/${name}.cpp
		PROPERTIES GENERATED TRUE
	)
	set_source_files_properties (
		${CMAKE_SOURCE_DIR}/${dir}/${name}.h
		PROPERTIES GENERATED TRUE
	)
endfunction ()

#
# ==== Configuring JustAddROM Build ==============================================
#
# ---- JustAddROM Source Files ---------------------------------------------------
#

set ( srcfiles 
	CMakeLists.txt
	src/main.cpp )

#if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" )
#	# additional source code
#	list ( APPEND app_sources
#		# Apple macOS required resources
#		Resources/macOS/Info.plist.in
#		Resources/macOS/JustAddROM.icns
#		Resources/macOS/Entitlements.plist
#	)
#endif ()

#
# ---- Common IDE setup --------------------------------------------------------
#

set_property( GLOBAL PROPERTY USE_FOLDERS ON )
source_group( TREE "${CMAKE_SOURCE_DIR}" PREFIX "Sources" FILES ${srcfiles} )

#
# ---- Platform specific rules ----------------------------------------------------------
#

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" )

	# take care of the app icon
#	set( MACOSX_BUNDLE_ICON_FILE JustAddROM.icns )
#	set_source_files_properties (
#		Resources/macOS/${MACOSX_BUNDLE_ICON_FILE}
#		PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
#	)

	# create the applications
	add_executable ( JustAddROM  MACOSX_BUNDLE
		${srcfiles}
	)

	# how to compile and link
	target_compile_options( JustAddROM PUBLIC
		-Wall -Wextra -Wpedantic -Wno-missing-field-initializers -Werror
	)
	target_compile_definitions ( JustAddROM PRIVATE
		TARGET_OS_MAC=1
	)
	target_link_libraries ( JustAddROM
		${system_libs}
		fltk
		fltk_images
		fltk_png
		fltk_z
		"-framework AppKit"
		"-framework Cocoa"
	)
#	set_target_properties ( JustAddROM PROPERTIES
#		MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Resources/macOS/Info.plist.in"
#	)
#	set_target_properties ( JustAddROM PROPERTIES
#		CPACK_BUNDLE_APPLE_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/Resources/macOS/Entitlements.plist"
#	)
	if ( XCODE )
		# Fix indentation settings that are cleared by CMake at every generator call.
		add_custom_target ( FixTabSettings
			COMMAND ${CMAKE_HOME_DIRECTORY}/cmake/xcodeFixIndentation
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	endif ()

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )

	# create the application
	add_executable ( JustAddROM
		${srcfiles}
	)

	# how to compile and link
	target_compile_options ( JustAddROM PUBLIC
		-Wall -Wno-multichar -Wno-misleading-indentation
		-Wno-missing-field-initializers 
		# -Wno-unused-result -Werror
	)
	target_compile_definitions ( JustAddROM PRIVATE
		TARGET_OS_LINUX=1
	)
	target_link_libraries ( JustAddROM
		${system_libs}
		fltk
		fltk_images
		fltk_png
		fltk_z
	)

elseif ( WIN32 )

	# Create a resources file for Windows
#	configure_file (
#		Resources/MSWindows/JustAddROM.rc.in
#		JustAddROM.rc
#	)

	# create the binaries
	add_executable ( JustAddROM WIN32
		${common_sources} ${app_sources} ${cmake_sources} ${data}
		${CMAKE_CURRENT_BINARY_DIR}/JustAddROM.rc
	)

	# how to compile and link
	target_compile_options( JustAddROM PUBLIC "/bigobj" )
	if ( $<CONFIG:Debug> )
		target_compile_options( JustAddROM PUBLIC "/ZI" )
	endif ()
	target_compile_definitions ( JustAddROM PRIVATE
		TARGET_OS_WIN32=1
		WIN32_LEAN_AND_MEAN=1 _CRT_SECURE_NO_WARNINGS=1
	)
	target_link_libraries ( JustAddROM
		${system_libs}
		fltk
		fltk_images
		fltk_png
		fltk_z
		winmm
		gdiplus
	)

endif ()

#
# ---- JustAddROM Platform Independent Build Instructions ------------------------
#

target_include_directories (
	JustAddROM PUBLIC
	${CMAKE_SOURCE_DIR}
	${FLTK_INCLUDE_DIRS}
)

target_compile_definitions ( JustAddROM PUBLIC "$<$<CONFIG:DEBUG>:_DEBUG>" USE_CMAKE )

# ---- Generated Files ---------------------------------------------------------

#set_source_files_properties (
#	/app/Version_CMake.h
#	PROPERTIES GENERATED TRUE
#)

#string ( TIMESTAMP COMPILE_TIME_YYYY "%Y" )
#string ( TIMESTAMP COMPILE_TIME_MM "%m" )
#string ( TIMESTAMP COMPILE_TIME_DD "%d" )

#configure_file (
#	${CMAKE_CURRENT_SOURCE_DIR}/app/Version_CMake.h.in
#	${CMAKE_CURRENT_SOURCE_DIR}/app/Version_CMake.h
#)



