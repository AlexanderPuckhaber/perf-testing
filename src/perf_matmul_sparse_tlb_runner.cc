#include "matmul.h"
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include "get_perf_configs.h"
#include "mappings.h"



int main(int argc, char *argv[]) {
  int c;
  // initialize input matrices A (I x K), B (K x J)
  // and output matrix C (I x J)

  int I, J, K;
  int bs;
  int split_factor;
  std::string perf_command;

  while ((c = getopt(argc, argv, "i:j:k:b:p:s:")) != -1) {
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
      case 'b':
        bs = atoi(optarg);
        break;
      case 'p':
        perf_command = std::string(optarg);
        break;
      case 's':
        split_factor = atoi(optarg);
        break;
      case 'h':
        // fall through
      default:
        printf("Help: multiply matrices: A (I x K), B (K x J) to output C (I x J).\n"
          "usage: -i I -j J -k K -b BLOCK_SIZE -p {CFG_L1_LL, CFG_BRANCHES, CFG_CYCLES_TLB} -s Number of page splits (1-64)\n");
        return -1;
    }
  }

  std::vector<uint32_t> A_index_lut(I*K);

  std::vector<uint32_t> B_index_lut(J*K);

  PerfProfiler myperf = get_perf_config(perf_command);

  // A: (I x K)
  std::vector<float> A, B, C;
  init_2d_vector(A, I, K, 0);
  init_2d_vector(B, K, J, 0);
  init_2d_vector(C, I, J, 0);

  split_across_n_pages(A_index_lut, A, split_factor);
  // scatter_across_n_pages(B_index_lut, B, split_factor);

  myperf.Initialize();
  myperf.Reset();
  myperf.Enable();

  // your test code here
  matmul_tiled_tlb(A, B, C, I, J, K, bs, A_index_lut, B_index_lut);


  myperf.Disable();

  std::map<std::string, uint64_t> result_map = myperf.Read();
  result_map = myperf.Read();

  for (const auto& kv : result_map) {
    printf("%s: %s\n", kv.first.c_str(), std::to_string(kv.second).c_str());
  }

  return 0;
}