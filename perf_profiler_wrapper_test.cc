#include "perf_profiler.h"
#include "matmul.h"

int main() {
  PerfProfiler myperf;

  // perf_read_format prf;

  myperf.Initialize();

  // initialize input matrices A (I x K), B (K x J)
  // and output matrix C (I x J)

  int I = 500;
  int J = 500;
  int K = 1000;

  // A: (I x K)
  std::vector<std::vector<int>> A, B, C;
  init_2d_vector(A, I, K, 0);
  init_2d_vector(B, K, J, 0);
  init_2d_vector(C, I, J);

  myperf.Reset();
  myperf.Enable();

  // your test code here
  matmul_naive(A, B, C);

  myperf.Disable();
  std::map<std::string, uint64_t> result_map = myperf.Read();

  for (const auto& kv : result_map) {
    printf("%s: %s\n", kv.first.c_str(), std::to_string(kv.second).c_str());
  }

  return 0;
}