# Buffer where text is kept during editing, before being flushed to file
add_library(Buffer gap.c gap.h)
target_include_directories(Buffer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})