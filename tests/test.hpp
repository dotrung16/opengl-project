#pragma once

#include <cmath>
#include <cstdio>
#include <vector>

namespace test {

struct Case {
  const char* name;
  void (*fn)();
};

inline std::vector<Case>& registry() {
  static std::vector<Case> cases;
  return cases;
}

inline int& failures() {
  static int count = 0;
  return count;
}

struct Registrar {
  Registrar(const char* name, void (*fn)()) { registry().push_back({name, fn}); }
};

inline void fail(const char* file, int line, const char* expr) {
  std::fprintf(stderr, "%s:%d: CHECK failed: %s\n", file, line, expr);
  ++failures();
}

inline constexpr int kSkipExitCode = 77;

inline int run_all() {
  int failed_cases = 0;
  for (const Case& c : registry()) {
    const int before = failures();
    c.fn();
    const bool passed = failures() == before;
    if (!passed) ++failed_cases;
    std::printf("[%s] %s\n", passed ? " OK " : "FAIL", c.name);
  }
  std::printf("%zu cases, %d failed\n", registry().size(), failed_cases);
  std::fflush(stdout);
  return failed_cases == 0 ? 0 : 1;
}

}

#define TEST(name)                                         \
  static void name();                                      \
  static const test::Registrar name##_registrar(#name, name); \
  static void name()

#define CHECK(expr) \
  do { if (!(expr)) test::fail(__FILE__, __LINE__, #expr); } while (0)

#define CHECK_NEAR(a, b, eps) \
  do { if (std::fabs((a) - (b)) > (eps)) test::fail(__FILE__, __LINE__, #a " ~= " #b); } while (0)
