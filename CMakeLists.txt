PROJECT(logger)
cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options to build demo and test
option(BUILD_DEMO "Build the demo" OFF)
option(BUILD_TEST "Build the test" OFF)

# ============================
# Directories
# ============================

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SRC_DIR "${ROOT_DIR}/src")
set(INCLUDE_DIR "${ROOT_DIR}/include")
set(LIBS_DIR "${ROOT_DIR}/libs")
set(DEMO_DIR "${ROOT_DIR}/demo")
set(TEST_DIR "${ROOT_DIR}/test")
set(THIRD_PARTY_DIR "${ROOT_DIR}/3rd")
set(CONFIG_DIR "${ROOT_DIR}/config")

# ============================
# Includes
# ============================

include(FetchContent)

include_directories(
    ${INCLUDE_DIR}
    ${LIBS_DIR}
)

# ============================
# Libraries
# ============================

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
if (BUILD_DEMO)
    add_subdirectory(demo)
    add_custom_target(demo ALL DEPENDS shlog)
endif()

# Conditionally build test
if (BUILD_TEST)
    add_subdirectory(test)
    add_custom_target(test ALL DEPENDS shlog)
endif()