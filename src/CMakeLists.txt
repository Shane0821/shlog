# Define the library
add_library(shlog)

aux_source_directory(${SRC_DIR} SRC)
target_sources(shlog PRIVATE ${SRC})

target_link_libraries(shlog pthread fmt::fmt uring)