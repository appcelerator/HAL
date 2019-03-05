# HAL
#
# Copyright (c) 2019 by Axway. All Rights Reserved.
# Licensed under the terms of the Apache Public License.
# Please see the LICENSE included with this distribution for details.
cmake_minimum_required(VERSION 3.0.0)

# See https://github.com/pabloariasal/modern-cmake-sample
# for guidance

project(HAL VERSION 0.5.0 LANGUAGES CXX C)

option(HAL_DISABLE_TESTS "Disable compiling the tests" OFF)
option(HAL_DEFINE_JSCLASSDEFINITIONEMPTY "Define HAL_DEFINE_JSCLASSDEFINITIONEMPTY" ON)
option(HAL_RENAME_AXWAYHAL "Rename DLL to AXWAYHAL" OFF)
option(HAL_USE_STRING_BOOLEAN_CONVERSION "Use Java-like string-boolean conversion" ON)

# necessary to provide <LIBRARY>_EXPORT.h downstream
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

# Define helper functions and macros.
list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)
include(internal_utils)

# Target architecture, only available for iOS
set(ARCH "i386" CACHE STRING "Target Architecture")

# Target platform, only available on Xcode
# "macosx", "iphoneos" or "iphonesimulator"
set(PLATFORM "macosx" CACHE STRING "Target Platform")

# Build shared library by default
set(LIBRARY_BUILD_TYPE SHARED)

# Defined in internal_utils.cmake.
config_compiler_and_linker()

# Prepare SDK configuration for iOS
if (NOT WIN32 AND ${PLATFORM} STREQUAL "iphoneos" OR ${PLATFORM} STREQUAL "iphonesimulator")
  set(LIBRARY_BUILD_TYPE STATIC)
  config_cmake_system_framework_path(${PLATFORM})
endif()

# Allow "make test" to work.
enable_testing()

# We have a custom finder for JavaScriptCore
find_package(JavaScriptCore REQUIRED MODULE)

set(SOURCE_HAL
  include/HAL/HAL.hpp
  include/HAL/JSString.hpp
  src/JSString.cpp
)

set(SOURCE_HAL_detail
  include/HAL/detail/JSBase.hpp
  src/detail/JSBase.cpp
  include/HAL/detail/JSUtil.hpp
  src/detail/JSUtil.cpp
)

set(SOURCE_JSExport
  include/HAL/JSExport.hpp
  include/HAL/JSExportObject.hpp
  src/JSExportObject.cpp
)

set(SOURCE_JSClass
  include/HAL/JSClass.hpp
  src/JSClass.cpp
)
  
set(SOURCE_JSContext
  include/HAL/JSContextGroup.hpp
  src/JSContextGroup.cpp
  include/HAL/JSContext.hpp
  src/JSContext.cpp
)

set(SOURCE_JSValue
  include/HAL/JSValue.hpp
  src/JSValue.cpp
)

set(SOURCE_JSObject
  include/HAL/JSPropertyAttribute.hpp
  include/HAL/JSPropertyNameArray.hpp
  src/JSPropertyNameArray.cpp
  include/HAL/JSObject.hpp
  src/JSObject.cpp
  include/HAL/JSArray.hpp
  src/JSArray.cpp
  include/HAL/JSError.hpp
  src/JSError.cpp 
  include/HAL/JSFunction.hpp
  src/JSFunction.cpp
)
  
source_group(HAL                   FILES ${SOURCE_HAL})
source_group(HAL\\detail           FILES ${SOURCE_HAL_detail})
source_group(HAL\\JSExport         FILES ${SOURCE_JSExport})
source_group(HAL\\JSClass          FILES ${SOURCE_JSClass})
source_group(HAL\\JSContext        FILES ${SOURCE_JSContext})
source_group(HAL\\JSValue          FILES ${SOURCE_JSValue})
source_group(HAL\\JSObject         FILES ${SOURCE_JSObject})

set(SOURCE_ALL
  ${SOURCE_HAL}
  ${SOURCE_HAL_detail}
  ${SOURCE_JSExport}
  ${SOURCE_JSClass}
  ${SOURCE_JSContext}
  ${SOURCE_JSValue}
  ${SOURCE_JSObject}
)

# Turn on precompiled headers
include(precompiled_header)
enable_precompiled_headers(src/stdafx.h SOURCE_ALL)

add_library(HAL ${LIBRARY_BUILD_TYPE}
  ${SOURCE_ALL}
)

#Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(HAL::HAL ALIAS HAL)

include(GenerateExportHeader)
generate_export_header(HAL)
target_compile_definitions(HAL PRIVATE HAL_EXPORTS) # to mark that we're building the library
target_compile_definitions(HAL PRIVATE STATICALLY_LINKED_WITH_JavaScriptCore) # must be set to consume JSC!

target_include_directories(HAL
  PUBLIC
    $<INSTALL_INTERFACE:include>    
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(HAL
  PUBLIC
    JavaScriptCore::JavaScriptCore
)

if (WIN32)
  # Silence this warning when linking with the JavaScriptCore static
  # library:
  #
  # "warning LNK4099: PDB 'vc120.pdb' was not found with
  # 'JavaScriptCore-Debug.lib(...)' ...".
  set_property(TARGET HAL APPEND PROPERTY LINK_FLAGS "/IGNORE:4099")
  
  # Silence this warning when lnking with the JavaScriptCore static
  # library for the i386 emulator:
  #
  # "JavaScriptCore-Debug.lib(JSStringRef.obj) : warning LNK4075:
  # ignoring '/EDITANDCONTINUE' due to '/SAFESEH' specification".
  #
  # According to "/IGNORE (Ignore Specific Warnings)" (see
  # http://msdn.microsoft.com/en-us/library/dn782850.aspx) some linker
  # warnings cannot be ignored and LNK4075 is one of them. However,
  # according to "/SAFESEH (Image has Safe Exception Handlers)" (see
  # http://msdn.microsoft.com/en-us/library/9a89h429.aspx):
  #
  # "/SAFESEH is only valid when linking for x86 targets."
  #
  # Therefore we test for the processor architecture we are targeting
  # and if its i386 (i.e. the emulator) then we pass /SAFESEH:NO to
  # the linker.
  # 
  # If target platform is Windows Phone/Store,
  # we should not disable SAFESEH otherwise Windows App Certification will fail.
  # 
  if (NOT CMAKE_SYSTEM_NAME MATCHES "^Windows(Phone|Store)$")
    include(${PROJECT_SOURCE_DIR}/cmake/TargetArch.cmake)
    target_architecture(target_architecture)
    if (${target_architecture} STREQUAL i386)
      set_property(TARGET HAL APPEND_STRING PROPERTY LINK_FLAGS " /SAFESEH:NO")
    endif()
  endif()

  # Silence this warning when lnking the Debug configuration:
  #
  # "LINK : warning LNK4075: ignoring '/INCREMENTAL' due to '/OPT:ICF'
  # specification ..."
  #
  # According to "/IGNORE (Ignore Specific Warnings)" (see
  # http://msdn.microsoft.com/en-us/library/dn782850.aspx) some linker
  # warnings cannot be ignored and LNK4075 is one of them. However,
  # according to "/INCREMENTAL (Link Incrementally)" (see
  # http://msdn.microsoft.com/en-us/library/4khtbfyf.aspx):
  #
  # "Most programs can be linked incrementally. However, some changes
  # are too great, and some options are incompatible with incremental
  # linking. LINK performs a full link if any of the following options
  # are specified:
  # ...
  # /OPT:REF is selected
  # /OPT:ICF is selected
  # ..."
  #
  # And according to "/OPT (Optimizations)" (see
  # http://msdn.microsoft.com/en-us/library/bxwfs976.aspx):
  #
  # "If /DEBUG is specified, the default for /OPT is NOREF, and all
  # functions are preserved in the image."
  #
  # As well as:
  #
  # "... we do not recommend that you use /OPT:ICF in debug builds ..."
  #
  # Therefore we disable /OPT:REF and /OPT:ICF for the Debug
  # configuration.
  set_property(TARGET HAL APPEND_STRING PROPERTY LINK_FLAGS_DEBUG "/OPT:NOREF /OPT:NOICF")
endif()

if (HAL_DEFINE_JSCLASSDEFINITIONEMPTY)
  target_compile_definitions(HAL PRIVATE HAL_DEFINE_JSCLASSDEFINITIONEMPTY) 
endif()

if (NOT HAL_DISABLE_TESTS)
  include(${PROJECT_SOURCE_DIR}/cmake/test.cmake)
  add_subdirectory(examples)
  add_subdirectory(test)
endif()

if (HAL_RENAME_AXWAYHAL)
  set_target_properties(HAL PROPERTIES OUTPUT_NAME "AXWAYHAL")
endif()

if (HAL_USE_STRING_BOOLEAN_CONVERSION)
  target_compile_definitions(HAL PRIVATE HAL_USE_STRING_BOOLEAN_CONVERSION) 
endif()

# Support find_package(HAL 0.5 REQUIRED)

set_property(TARGET HAL PROPERTY VERSION ${HAL_VERSION})
set_property(TARGET HAL PROPERTY SOVERSION 0)
set_property(TARGET HAL PROPERTY INTERFACE_HAL_MAJOR_VERSION 0)
set_property(TARGET HAL APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING HAL_MAJOR_VERSION
)

##############################################
# Installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/HAL)

install(TARGETS HAL
  EXPORT HALTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

#Export the targets to a script
install(EXPORT HALTargets
  FILE
    HALTargets.cmake
  NAMESPACE
    HAL::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/HALConfigVersion.cmake"
  VERSION ${HAL_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/HALConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/HALConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

#Install the config, configversion and custom find modules
install(FILES
  "${CMAKE_CURRENT_LIST_DIR}/cmake/FindJavaScriptCore.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/HALConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/HALConfigVersion.cmake"
  DESTINATION ${INSTALL_CONFIGDIR}
  COMPONENT Devel
)

##############################################
## Exporting from the build tree
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/FindJavaScriptCore.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/FindJavaScriptCore.cmake
    COPYONLY)

export(EXPORT HALTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/HALTargets.cmake"
)

#Register package in user's package registry
export(PACKAGE HAL)
