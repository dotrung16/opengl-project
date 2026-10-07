add_library(glad STATIC src/gl.c)
target_include_directories(glad SYSTEM PUBLIC include)

target_compile_options(glad PRIVATE
  $<$<AND:$<CONFIG:Release>,$<C_COMPILER_ID:GNU,Clang,AppleClang>>:-ffunction-sections -fdata-sections>
)
