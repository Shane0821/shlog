# Define the library
add_library(shlog)

aux_source_directory(${SRC_DIR} SRC)
target_sources(shlog PRIVATE ${SRC})

target_include_directories(shlog
    PUBLIC
    $<BUILD_INTERFACE:${INCLUDE_DIR}/shlog/libs>
    $<BUILD_INTERFACE:${INCLUDE_DIR}/shlog>
    $<INSTALL_INTERFACE:include/shlog>
)

set_target_properties(shlog PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)

target_link_libraries(shlog pthread fmt::fmt uring)