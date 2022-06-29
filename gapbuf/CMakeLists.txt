# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found. Any number of sources could be listed here.
add_library (GapBuffer STATIC gapbuf.c gapbuf.h)

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
target_include_directories (GapBuffer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})