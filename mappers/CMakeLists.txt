project(mappers)

file(GLOB source_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(mappers "${source_files}")