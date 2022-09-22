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

  // printf("A_num_pages: %d, B_num_pages: %d\n", A_num_pages, B_num_pages);
  // printf("C_num_pages: %d\n", C_num_pages);

  std::vector<uint32_t> A_index_lut(I*K);

  // std::string in = "abcdefgh", out;
  //   std::sample(A_index_lut.begin(), A_index_lut.end(), std::back_inserter(out),
  //               5, std::mt19937{std::random_device{}()});
  //   std::cout << "five random letters out of " << in << " : " << out << '\n';

  //   while (1) {};

  for (unsigned int i = 0; i < I*K; i++) {
    A_index_lut[i] = i;
    // printf("%d, ", A_index_lut[i]);
  }

  unsigned int num_randomized;

  if (frac_randomized == 0.0) {
    num_randomized = 0;
  } else {
    num_randomized = (int) (((float) I*K) * frac_randomized);
  }

  // printf("frac_randomized: %f, num_randomized: %d\n", frac_randomized, num_randomized);

  // num_randomized = 50000;

  std::vector<uint32_t> idx_to_randomize;

  std::sample(A_index_lut.begin(), A_index_lut.end(), std::back_inserter(idx_to_randomize), num_randomized, std::mt19937{std::random_device{}()});

  for (uint32_t idx : idx_to_randomize) {
    uint32_t page_num, page_offset;
    page_num = idx / PAGE_SIZE;
    page_offset = idx % PAGE_SIZE;
    

    uint32_t rand_target_page_num = rand() % A_num_pages;

    uint32_t target_idx = rand_target_page_num * (PAGE_SIZE / sizeof(float)) + page_offset;

    // printf("idx: %d, page_num: %d, page_offset: %d target_page_num: %d target_idx: %d\n", idx, page_num, page_offset, rand_target_page_num, target_idx);

    uint32_t tmp = 0;

    if (target_idx > I*K) {
      // printf("bad idx!\n");
      // exit(1);
    } else {

      tmp = A_index_lut[target_idx];
      A_index_lut[target_idx] = A_index_lut[idx];
      A_index_lut[idx] = tmp;
    }
  }

  std::vector<uint32_t> B_index_lut(J*K);

  for (unsigned int i = 0; i < J*K; i++) {
    B_index_lut[i] = i;
  }

  idx_to_randomize.clear();

  std::sample(B_index_lut.begin(), B_index_lut.end(), std::back_inserter(idx_to_randomize), num_randomized, std::mt19937{std::random_device{}()});

  for (uint32_t idx : idx_to_randomize) {
    uint32_t page_num, page_offset;
    page_num = idx / PAGE_SIZE;
    page_offset = idx % PAGE_SIZE;
    

    uint32_t rand_target_page_num = rand() % A_num_pages;

    uint32_t target_idx = rand_target_page_num * (PAGE_SIZE / sizeof(float)) + page_offset;

    // printf("idx: %d, page_num: %d, page_offset: %d target_page_num: %d target_idx: %d\n", idx, page_num, page_offset, rand_target_page_num, target_idx);

    uint32_t tmp = 0;

    if (target_idx > J*K) {
      // printf("bad idx!\n");
      // exit(1);
    } else {

      tmp = B_index_lut[target_idx];
      B_index_lut[target_idx] = B_index_lut[idx];
      B_index_lut[idx] = tmp;
    }
  }

  // for (uint32_t idx : A_index_lut) {
  //   printf("%d, ", idx);
  // }

  

  // std::vector<uint32_t> A_p_index_order(C_num_pages);
  // std::random_shuffle(A_p_index_order.begin(), A_p_index_order.end());

  // unsigned int counter = 0;
  // for (unsigned int p_offset = 0; p_offset < PAGE_SIZE; p_offset++) {
  //   for (unsigned int p_idx = 0; p_idx < C_num_pages; p_idx++) {
  //     unsigned int idx = A_p_index_order[p_idx] * PAGE_SIZE + p_offset;

  //     if (counter < I*K) {
  //       A_index_lut[counter] = idx;
  //     }

  //     counter++;
  //   }
  // }

  // for (unsigned int i = 0; i < I*K; i++) {
  //   printf("%x\n", A_index_lut[i]);
  // }

  // while (1) {};


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