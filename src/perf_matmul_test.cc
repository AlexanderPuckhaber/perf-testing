#include "get_perf_configs.h"
#include "matmul.h"
#include <stdarg.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int c;
  // initialize input matrices A (I x K), B (K x J)
  // and output matrix C (I x J)

  int I, J, K;
  int bs;
  std::string method_name;

  while ((c = getopt(argc, argv, "i:j:k:m:b:")) != -1) {
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
      case 'h':
        // fall through
      default:
        printf("Help: multiply matrices: A (I x K), B (K x J) to output C (I x J).\n"
          "usage: -i I -j J -k K -m METHOD (naive, tiled) -b BLOCK_SIZE\n");
        return -1;
    }
  }

  std::map<perf_type_config_t, std::string> counter_name_map;

  // counter_name_map[{PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES}] = "PERF_COUNT_HW_CPU_CYCLES";
  // counter_name_map[{PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES}] = "PERF_COUNT_HW_CACHE_REFERENCES";
  // counter_name_map[{PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES}] = "PERF_COUNT_HW_CACHE_MISSES";
  // // counter_name_map[{PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS}] = "PERF_COUNT_HW_INSTRUCTIONS";
  // // counter_name_map[{PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS}] = "PERF_COUNT_HW_BRANCH_INSTRUCTIONS";
  // // counter_name_map[{PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES}] = "PERF_COUNT_HW_BRANCH_MISSES";
  // // counter_name_map[{PERF_TYPE_HW_CACHE, PERF_COUNT_HW_CACHE_L1D}] = "PERF_COUNT_HW_CACHE_L1D";
  // counter_name_map[{PERF_TYPE_SOFTWARE, PERF_COUNT_SW_PAGE_FAULTS}] = "PERF_COUNT_SW_PAGE_FAULTS";

  std::vector<std::string> event_list;
  // event_list = {"PERF_COUNT_HW_CACHE_LL:READ:ACCESS", "PERF_COUNT_HW_CACHE_LL:READ:MISS", "DTLB-LOADS", "DTLB-LOAD-MISSES"};




  PerfProfiler myperf = get_perf_config("CFG_LL_TLB");

  myperf.Initialize();

  // A: (I x K)
  std::vector<float> A, B, C;
  init_2d_vector(A, I, K, 0);
  init_2d_vector(B, K, J, 0);
  init_2d_vector(C, I, J);

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