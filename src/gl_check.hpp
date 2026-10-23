#pragma once

#include <glad/gl.h>

#include <cstdio>

#ifndef NDEBUG
inline void check_gl_errors(const char *where) {
  for (GLenum err; (err = glGetError()) != GL_NO_ERROR;)
    std::fprintf(stderr, "GL error 0x%x in %s\n", err, where);
}

#define GL_CHECK(call)                                                         \
  do {                                                                         \
    call;                                                                      \
    check_gl_errors(#call);                                                    \
  } while (0)
#else
#define GL_CHECK(call) call
#endif
