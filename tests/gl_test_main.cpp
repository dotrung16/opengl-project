#include "gl_fixture.hpp"
#include "test.hpp"

int main() {
  glfwInitHint(GLFW_WAYLAND_LIBDECOR, GLFW_WAYLAND_DISABLE_LIBDECOR);
  {
    GlFixture probe;
    if (!probe.ok()) {
      std::printf("No usable display or GL context, skipping\n");
      return test::kSkipExitCode;
    }
  }
  return test::run_all();
}
