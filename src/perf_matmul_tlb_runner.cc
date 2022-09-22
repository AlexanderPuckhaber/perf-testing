#include "matmul.h"
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include "get_perf_configs.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <iterator>

#define PAGE_SIZE 4096
#define CACHE_LINE 64

void randomly_swap_cache_blocks_across_pages(std::vector<uint32_t>& index_lut, float frac_to_randomly_swap) {
  uint32_t lut_num_pages = index_lut.size() / PAGE_SIZE;

  uint32_t idx_per_cache_line = CACHE_LINE / sizeof(float);

  std::vector<uint32_t> lut_cache_lines(index_lut.size() / idx_per_cache_line);

  for (unsigned int i = 0; i < lut_cache_lines.size(); i++) {
    lut_cache_lines[i] = i * idx_per_cache_line;
  }

  unsigned int num_randomized;

  if (frac_to_randomly_swap == 0.0) {
    num_randomized = 0;
  } else {
    num_randomized = (int) (((float) lut_cache_lines.size()) * frac_to_randomly_swap);
  }

  // printf("frac_randomized: %f, num_randomized: %d\n", frac_randomized, num_randomized);

  // num_randomized = 50000;

  std::vector<uint32_t> idx_to_randomize;

  std::sample(lut_cache_lines.begin(), lut_cache_lines.end(), std::back_inserter(idx_to_randomize), num_randomized, std::mt19937{std::random_device{}()});

  for (uint32_t idx : idx_to_randomize) {
    uint32_t page_num, page_offset;
    page_num = idx / PAGE_SIZE;
    page_offset = idx % PAGE_SIZE;
    

    uint32_t rand_target_page_num = rand() % lut_num_pages;

    for (uint32_t j = 0; j < idx_per_cache_line; j++) {
      uint32_t target_idx = rand_target_page_num * (PAGE_SIZE / sizeof(float)) + page_offset + j;

      // printf("idx: %d, page_num: %d, page_offset: %d target_page_num: %d target_idx: %d\n", idx, page_num, page_offset, rand_target_page_num, target_idx);

      uint32_t tmp = 0;

      if (target_idx > index_lut.size()) {
        // printf("bad idx!\n");
        // exit(1);
      } else {

        tmp = index_lut[target_idx];
        index_lut[target_idx] = index_lut[idx];
        index_lut[idx] = tmp;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  int c;
  // initialize input matrices A (I x K), B (K x J)
  // and output matrix C (I x J)

  int I, J, K;
  int bs;
  float frac_randomized = 0.0;
  std::string method_name, perf_command;

  while ((c = getopt(argc, argv, "i:j:k:m:b:p:r:")) != -1) {
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
      case 'r':
        frac_randomized = atof(optarg);
        break;
      case 'h':
        // fall through
      default:
        printf("Help: multiply matrices: A (I x K), B (K x J) to output C (I x J).\n"
          "usage: -i I -j J -k K -m METHOD {naive, tiled} -b BLOCK_SIZE -p {CFG_L1_LL, CFG_BRANCHES, CFG_CYCLES_TLB} -r FRAC_RANDOMIZED\n");
        return -1;
    }
  }

  // calculate number of pages in A and B vectors

  uint32_t A_num_pages = (I * K * sizeof(float)) / PAGE_SIZE;
  uint32_t B_num_pages = (J * K * sizeof(float)) / PAGE_SIZE;
  uint32_t C_num_pages = (I * J * sizeof(float)) / PAGE_SIZE;


  std::vector<uint32_t> A_index_lut(I*K);

  for (unsigned int i = 0; i < I*K; i++) {
    A_index_lut[i] = i;
  }

  std::vector<uint32_t> B_index_lut(J*K);

  for (unsigned int i = 0; i < J*K; i++) {
    B_index_lut[i] = i;
  }

  randomly_swap_cache_blocks_across_pages(A_index_lut, frac_randomized);
  randomly_swap_cache_blocks_across_pages(B_index_lut, frac_randomized);


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
    matmul_tiled_tlb(A, B, C, I, J, K, bs, A_index_lut, B_index_lut);
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