# Buffer where text is kept during editing, before being flushed to file
add_library (Buffer STATIC buffer.c buffer.h gap/gap.c gap/gap.h)

target_include_directories (Buffer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})