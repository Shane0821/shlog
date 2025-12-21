cmake_minimum_required(VERSION 3.2)
PROJECT(shlog VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options to build demo and test
option(SHLOG_BUILD_DEMO "Build the demo" OFF)
option(SHLOG_BUILD_TEST "Build the test" OFF)

# ============================
# Directories
# ============================

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SRC_DIR "${ROOT_DIR}/src")
set(INCLUDE_DIR "${ROOT_DIR}/include")
set(DEMO_DIR "${ROOT_DIR}/demo")
set(TEST_DIR "${ROOT_DIR}/test")
set(THIRD_PARTY_DIR "${ROOT_DIR}/3rd")
set(CONFIG_DIR "${ROOT_DIR}/config")
set(CMAKE_DIR "${ROOT_DIR}/cmake")

# ============================
# Libraries
# ============================

include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)

# ============================
# Pakcages
# ============================


# ============================
# Files
# ============================


# ============================
# Build
# ============================

# add_subdirectory(libs)
add_subdirectory(src)

# Conditionally build demo
if (SHLOG_BUILD_DEMO)
    add_subdirectory(demo)
endif()

# Conditionally build test
if (SHLOG_BUILD_TEST)
    add_subdirectory(test)
endif()

# ============================
# Export
# ============================

include(GNUInstallDirs)

# Install library and headers
install(TARGETS shlog
    EXPORT shlog-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Export targets (to build tree for development) and install tree for consumers
export(EXPORT shlog-targets
    NAMESPACE shlog::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/shlog-targets.cmake"
)

# Generate Config and Version files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/shlog-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_DIR}/shlog-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/shlog-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shlog
)

# Install exported targets and config files
install(EXPORT shlog-targets
    NAMESPACE shlog::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shlog
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/shlog-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/shlog-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shlog
)