# Gather our source files in this directory.
set(
  gazprea_rt_files
  "${CMAKE_CURRENT_SOURCE_DIR}/runtime.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/matrix.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/vectors.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/casting.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/filters.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/copy_ops.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/matrix_literals.c"
)

# Build our executable from the source files.
add_library(gazrt SHARED ${gazprea_rt_files})
target_include_directories(gazrt PUBLIC ${RUNTIME_INCLUDE})

# Symbolic link our library to the base directory so we don't have to go searching for it.
symlink_to_bin("gazrt")
