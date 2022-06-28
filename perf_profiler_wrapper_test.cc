#include "perf_profiler.h"

int main() {
  PerfProfiler myperf;

  // perf_read_format prf;

  myperf.Initialize();

  myperf.Reset();
  myperf.Enable();
  for (uint i = 0; i < 1000; i++) {

  }
  myperf.Disable();
  std::map<std::string, uint64_t> result_map = myperf.Read();

  for (const auto& kv : result_map) {
    printf("%s: %d\n", kv.first.c_str(), kv.second);
  }

  return 0;
}