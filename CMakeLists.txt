# much of this pulled from the cmake wiki
# https://cmake.org/Wiki/CMake/Tutorials/How_to_create_a_ProjectConfig.cmake_file
cmake_minimum_required(VERSION 2.8)
project(traylang)

set(TRAYLANG_MAJOR_VERSION 1)
set(TRAYLANG_MINOR_VERSION 0)
set(TRAYLANG_PATCH_VERSION 0)
set(TRAYLANG_VERSION
    ${TRAYLANG_MAJOR_VERSION}.${TRAYLANG_MINOR_VERSION}.${TRAYLANG_PATCH_VERSION})

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib
    CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin
    CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR
    include CACHE PATH
    "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
    set(DEF_INSTALL_CMAKE_DIR cmake)
else()
    set(DEF_INSTALL_CMAKE_DIR lib/cmake/TrayLang)
endif()
set(INSTALL_CMAKE_DIR
    ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
    "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
    set(var INSTALL_${p}_DIR)
    if(NOT IS_ABSOLUTE "${${var}}")
        set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif()
endforeach()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

include_directories(include)
include_directories(lib/include)

add_subdirectory(lib)
add_subdirectory(main)

option (WITH_TESTS "build tests" ON)

if (WITH_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Add all targets to the build-tree export set
export(TARGETS traylang
    FILE "${PROJECT_BINARY_DIR}/TrayLangTargets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE TrayLang)

# Create the TrayLangConfig.cmake and TrayLangConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
    "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(TrayLangConfig.cmake.in
    "${PROJECT_BINARY_DIR}/TrayLangConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${INSTALL_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(TrayLangConfig.cmake.in
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/TrayLangConfig.cmake" @ONLY)
# ... for both
configure_file(TrayLangConfigVersion.cmake.in
    "${PROJECT_BINARY_DIR}/TrayLangConfigVersion.cmake" @ONLY)

# Install the TrayLangConfig.cmake and TrayLangConfigVersion.cmake
install(FILES
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/TrayLangConfig.cmake"
    "${PROJECT_BINARY_DIR}/TrayLangConfigVersion.cmake"
    DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT TrayLangTargets DESTINATION
    "${INSTALL_CMAKE_DIR}" COMPONENT dev)

