#include "matmul.h"
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include "get_perf_configs.h"

int main(int argc, char *argv[]) {
  int c;
  // initialize input matrices A (I x K), B (K x J)
  // and output matrix C (I x J)

  int I, J, K;
  int bs;
  std::string method_name, perf_command;

  while ((c = getopt(argc, argv, "i:j:k:m:b:p:")) != -1) {
    switch (c) {
      case 'i':
        I = atoi(optarg);
        break;
      case 'j':
        J = atoi(optarg);
        break;
      case 'k':
        K = atoi(optarg);
        break;
      case 'm':
        method_name = std::string(optarg);
        break;
      case 'b':
        bs = atoi(optarg);
        break;
      case 'p':
        perf_command = std::string(optarg);
        break;
      case 'h':
        // fall through
      default:
        printf("Help: multiply matrices: A (I x K), B (K x J) to output C (I x J).\n"
          "usage: -i I -j J -k K -m METHOD {naive, tiled} -b BLOCK_SIZE -p {CFG_L1_LL, CFG_BRANCHES, CFG_CYCLES_TLB}\n");
        return -1;
    }
  }

  PerfProfiler myperf = get_perf_config(perf_command);

  // A: (I x K)
  std::vector<float> A, B, C;
  init_2d_vector(A, I, K, 0);
  init_2d_vector(B, K, J, 0);
  init_2d_vector(C, I, J);

  myperf.Initialize();
  myperf.Reset();
  myperf.Enable();

  // your test code here
  if (method_name == "naive") {
    // matmul_naive(A, B, C);
  } else if (method_name == "tiled") {
    matmul_tiled(A, B, C, I, J, K, bs);
  } else {
    printf("Invalid method name: %s\n", method_name.c_str());
    return -1;
  }

  myperf.Disable();

  std::map<std::string, uint64_t> result_map = myperf.Read();
  result_map = myperf.Read();

  for (const auto& kv : result_map) {
    printf("%s: %s\n", kv.first.c_str(), std::to_string(kv.second).c_str());
  }

  return 0;
}