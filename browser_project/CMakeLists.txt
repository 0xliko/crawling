# Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

#
# Source files.
#

# CEFBrowser sources.
set(CEFBrowser_SRCS
  browser_app.cc
  browser_ipc.cc
  browser_app.h
  browser_handler.cc
  browser_handler.h
  handlers/display_handler.cc
  handlers/render_handler.cc
  handlers/load_handler.cc
  handlers/lifespan_handler.cc
  handlers/request_handler.cc
  handlers/resource_request_handler.cc
  browser_response_filter.h
  browser_response_filter.cc
  jsonxx.cc
  jsonxx.h
  global.h
  Base64.h
  )
set(CEFBrowser_SRCS_LINUX
  main.cc
  browser_handler_linux.cc
  )
set(CEFBrowser_SRCS_MACOSX
  CEFBrowser_mac.mm
  browser_handler_mac.mm
  )
set(CEFBrowser_SRCS_WINDOWS
  CEFBrowser.rc
  CEFBrowser_win.cc
  resource.h
  browser_handler_win.cc
  )
APPEND_PLATFORM_SOURCES(CEFBrowser_SRCS)
source_group(CEFBrowser FILES ${CEFBrowser_SRCS})

set(CEFBrowser_SRCS
  ${CEFBrowser_SRCS}
  )

# CEFBrowser helper sources.
set(CEFBrowser_HELPER_SRCS_MACOSX
  process_helper_mac.cc
  )
APPEND_PLATFORM_SOURCES(CEFBrowser_HELPER_SRCS)
source_group(CEFBrowser FILES ${CEFBrowser_HELPER_SRCS})

# CEFBrowser resources.
set(CEFBrowser_RESOURCES_MAC_SRCS_MACOSX
  mac/Info.plist
  mac/CEFBrowser.icns
  )
APPEND_PLATFORM_SOURCES(CEFBrowser_RESOURCES_MAC_SRCS)
source_group(CEFBrowser\\\\mac FILES ${CEFBrowser_RESOURCES_MAC_SRCS})

set(CEFBrowser_RESOURCES_MAC_ENGLISH_LPROJ_SRCS_MACOSX
  mac/English.lproj/InfoPlist.strings
  mac/English.lproj/MainMenu.xib
  )
APPEND_PLATFORM_SOURCES(CEFBrowser_RESOURCES_MAC_ENGLISH_LPROJ_SRCS)
source_group(CEFBrowser\\\\mac\\\\English.lproj FILES ${CEFBrowser_RESOURCES_MAC_ENGLISH_LPROJ_SRCS})

set(CEFBrowser_RESOURCES_SRCS
  ${CEFBrowser_RESOURCES_MAC_SRCS}
  ${CEFBrowser_RESOURCES_MAC_ENGLISH_LPROJ_SRCS}
  )


#
# Shared configuration.
#

# Target executable names.
set(CEF_TARGET "browser")
if(OS_MACOSX)
  set(CEF_HELPER_TARGET "CEFBrowser_Helper")
  set(CEF_HELPER_OUTPUT_NAME "browser Helper")
else()
  # Logical target used to link the libcef library.
  ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
endif()

# Determine the target output directory.
SET_CEF_TARGET_OUT_DIR()
set(CEF_TARGET_OUT_DIR "../../out/browser_project")



#
# Linux configuration.
#

if(OS_LINUX)
  # Executable target.
  FIND_LINUX_LIBRARIES("gmodule-2.0  gthread-2.0 xi")
  add_executable(${CEF_TARGET} ${CEFBrowser_SRCS})
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
  include_directories(./handlers)
  target_link_libraries(${CEF_TARGET} libcef_lib Xcomposite Xrender png libcef_dll_wrapper ${CEF_STANDARD_LIBS})

  # Set rpath so that libraries can be placed next to the executable.
  set_target_properties(${CEF_TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
  set_target_properties(${CEF_TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
  set_target_properties(${CEF_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CEF_TARGET_OUT_DIR})

  # Copy binary and resource files to the target output directory.
  COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")
  if (EXISTS "${CEF_BINARY_DIR}/libminigbm.so")
    COPY_FILES("${CEF_TARGET}" "libminigbm.so" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  endif()

  # Set SUID permissions on the chrome-sandbox target.
  SET_LINUX_SUID_PERMISSIONS("${CEF_TARGET}" "${CEF_TARGET_OUT_DIR}/chrome-sandbox")
endif()


#
# Mac OS X configuration.
#

if(OS_MACOSX)
  option(OPTION_USE_ARC "Build with ARC (automatic Reference Counting) on macOS." ON)
  if(OPTION_USE_ARC)
    list(APPEND CEF_COMPILER_FLAGS
      -fobjc-arc
      )
    set_target_properties(${target} PROPERTIES
      CLANG_ENABLE_OBJC_ARC "YES"
      )
  endif()

  # Output path for the main app bundle.
  set(CEF_APP "${CEF_TARGET_OUT_DIR}/${CEF_TARGET}.app")

  # Variables referenced from the main Info.plist file.
  set(EXECUTABLE_NAME "${CEF_TARGET}")
  set(PRODUCT_NAME "${CEF_TARGET}")

  if(USE_SANDBOX)
    # Logical target used to link the cef_sandbox library.
    ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
  endif()

  # Main app bundle target.
  add_executable(${CEF_TARGET} MACOSX_BUNDLE ${CEFBrowser_RESOURCES_SRCS} ${CEFBrowser_SRCS})
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
  target_link_libraries(${CEF_TARGET} libcef_dll_wrapper ${CEF_STANDARD_LIBS})
  set_target_properties(${CEF_TARGET} PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/mac/Info.plist
    )

  # Copy the CEF framework into the Frameworks directory.
  add_custom_command(
    TARGET ${CEF_TARGET}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
            "${CEF_APP}/Contents/Frameworks/Chromium Embedded Framework.framework"
    VERBATIM
    )

  # Create the multiple Helper app bundle targets.
  foreach(_suffix_list ${CEF_HELPER_APP_SUFFIXES})
    # Convert to a list and extract the suffix values.
    string(REPLACE ":" ";" _suffix_list ${_suffix_list})
    list(GET _suffix_list 0 _name_suffix)
    list(GET _suffix_list 1 _target_suffix)
    list(GET _suffix_list 2 _plist_suffix)

    # Define Helper target and output names.
    set(_helper_target "${CEF_HELPER_TARGET}${_target_suffix}")
    set(_helper_output_name "${CEF_HELPER_OUTPUT_NAME}${_name_suffix}")

    # Create Helper-specific variants of the helper-Info.plist file. Do this
    # manually because the configure_file command (which is executed as part of
    # MACOSX_BUNDLE_INFO_PLIST) uses global env variables and would insert the
    # wrong values with multiple targets.
    set(_helper_info_plist "${CMAKE_CURRENT_BINARY_DIR}/helper-Info${_target_suffix}.plist")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/mac/helper-Info.plist" _plist_contents)
    string(REPLACE "\${EXECUTABLE_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
    string(REPLACE "\${PRODUCT_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
    string(REPLACE "\${BUNDLE_ID_SUFFIX}" "${_plist_suffix}" _plist_contents ${_plist_contents})
    file(WRITE ${_helper_info_plist} ${_plist_contents})

    # Create Helper executable target.
    add_executable(${_helper_target} MACOSX_BUNDLE ${CEFBrowser_HELPER_SRCS})
    SET_EXECUTABLE_TARGET_PROPERTIES(${_helper_target})
    add_dependencies(${_helper_target} libcef_dll_wrapper)
    target_link_libraries(${_helper_target} libcef_dll_wrapper ${CEF_STANDARD_LIBS})
    set_target_properties(${_helper_target} PROPERTIES
      MACOSX_BUNDLE_INFO_PLIST ${_helper_info_plist}
      OUTPUT_NAME ${_helper_output_name}
      )

    if(USE_SANDBOX)
      target_link_libraries(${_helper_target} cef_sandbox_lib)
    endif()

    # Add the Helper as a dependency of the main executable target.
    add_dependencies(${CEF_TARGET} "${_helper_target}")

    # Copy the Helper app bundle into the Frameworks directory.
    add_custom_command(
      TARGET ${CEF_TARGET}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
              "${CEF_TARGET_OUT_DIR}/${_helper_output_name}.app"
              "${CEF_APP}/Contents/Frameworks/${_helper_output_name}.app"
      VERBATIM
      )
  endforeach()

  # Manually process and copy over resource files.
  # The Xcode generator can support this via the set_target_properties RESOURCE
  # directive but that doesn't properly handle nested resource directories.
  # Remove these prefixes from input file paths.
  set(PREFIXES "mac/")
  COPY_MACOSX_RESOURCES("${CEFBrowser_RESOURCES_SRCS}" "${PREFIXES}" "${CEF_TARGET}" "${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_APP}")
endif()


#
# Windows configuration.
#

if(OS_WINDOWS)
  # Executable target.
  add_executable(${CEF_TARGET} WIN32 ${CEFBrowser_SRCS})
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

  if(USE_SANDBOX)
    # Logical target used to link the cef_sandbox library.
    ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
    target_link_libraries(${CEF_TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
  endif()

  # Add the custom manifest files to the executable.
  ADD_WINDOWS_MANIFEST("${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_TARGET}" "exe")

  # Copy binary and resource files to the target output directory.
  COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")
endif()
