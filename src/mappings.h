#include <vector>
#include <inttypes.h>
#include <algorithm>
#include <random>

#define PAGE_SIZE 4096
#define CACHE_LINE 64
#define PAGE_INDICES (PAGE_SIZE / sizeof(float))
#define CACHE_LINE_INDICES (CACHE_LINE / sizeof(float))

void scatter_across_n_pages(std::vector<uint32_t>& index_lut, std::vector<float>& data_matrix, unsigned int n);

void split_across_n_pages(std::vector<uint32_t>& index_lut, std::vector<float>& data_matrix, unsigned int n);

void randomly_swap_cache_blocks_across_pages(std::vector<uint32_t>& index_lut, float frac_to_randomly_swap);