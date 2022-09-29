#include "mappings.h"

// 1 >= n >= (PAGE_SIZE / CACHE_LINE)
// so 1 >= n >= 64
// data matrix will be expanded by a factor of n
// Problem: when n doesn't divide 64 evenly, splits are not on cache lines + not all of data matrix is remapped
void scatter_across_n_pages(std::vector<uint32_t>& index_lut, std::vector<float>& data_matrix, unsigned int n) {
  std::vector<float> new_data_matrix(data_matrix.size() * n);

  // go through each page
  unsigned int fragment_size = (PAGE_INDICES) / n;
  unsigned int data_matrix_num_pages = (data_matrix.size() / PAGE_INDICES) + 1;

  for (unsigned int page_num = 0; page_num < data_matrix_num_pages; page_num++) {
    for (unsigned int fragment_num = 0; fragment_num < n; fragment_num++) {
      unsigned int fragment_idx_limit = std::max(fragment_size, (unsigned int) PAGE_INDICES - fragment_num*fragment_size);
      for (unsigned int fragment_idx = 0; fragment_idx < fragment_idx_limit; fragment_idx++) {
        unsigned int data_matrix_idx = (page_num * PAGE_INDICES) + (fragment_num * fragment_size) + fragment_idx;
        unsigned int new_data_matrix_idx = (((page_num * n) + fragment_num) * PAGE_INDICES) + fragment_idx;
        if (data_matrix_idx < data_matrix.size()) {
          new_data_matrix[new_data_matrix_idx] = data_matrix[data_matrix_idx];
          index_lut[data_matrix_idx] = new_data_matrix_idx;

          // printf("data_matrix_idx: %x new_idx: %x\n", data_matrix_idx, new_data_matrix_idx);
        }
      }
    }
  }
  
  data_matrix = new_data_matrix;
}

// 1 >= n >= a large number
// keeps all 64 cache lines intact, allocates them to n pages
// because only a discrete number can be assigned, can be slightly uneven
// e.g. 64 into 63 pages will have 62 pages with 1 line and 1 page with 2 lines
// But, this allows for spacing the pages further apart:
//   64 lines into 128 pages will map a line to every-other page, and so on
// this can be used to emulate sparse matrix lookups
void split_across_n_pages(std::vector<uint32_t>& index_lut, std::vector<float>& data_matrix, unsigned int n) {
  std::vector<float> new_data_matrix(data_matrix.size() * n, 0);

  // go through each page
  unsigned int fragment_size = (PAGE_INDICES) / n;
  unsigned int data_matrix_num_pages = (data_matrix.size() / PAGE_INDICES) + 1;

  unsigned int fragments_to_allocate = n;

  unsigned int lines_per_page = PAGE_SIZE / CACHE_LINE;

  // this is the mapping of cache lines to pages, re-used for every page of the input data_matrix
  std::vector<unsigned int> line_map(lines_per_page, 0);

  float remainder_bucket = 0.0;
  unsigned int cursor = 0;

  for (unsigned int fragment_idx = 0; fragment_idx < fragments_to_allocate; fragment_idx++) {
    float lines_per_fragment = (float) lines_per_page / (float) fragments_to_allocate;

    unsigned int whole_lines = (unsigned int) lines_per_fragment;
    remainder_bucket += lines_per_fragment - whole_lines;

    if (remainder_bucket >= 1.0) {
      whole_lines += 1;
      remainder_bucket -= 1.0;
    }

    for (unsigned int i = cursor; i < cursor + whole_lines; i++) {
      line_map[i] = fragment_idx;
      // printf("line_map[%d]: %d %.2f\n", i, fragment_idx, remainder_bucket);
    }

    cursor += whole_lines;
  }

  // exit(0);

  for (unsigned int i = cursor; i < lines_per_page; i++) {
    line_map[i] = fragments_to_allocate-1;
    // printf("line_map[%d]: %d %.2f\n [added after]", i, fragments_to_allocate-1, remainder_bucket);
  }

  for (unsigned int page_num = 0; page_num < data_matrix_num_pages; page_num++) {
    for (unsigned int line_idx = 0; line_idx < lines_per_page; line_idx++) {
      unsigned int fragment_idx = line_map[line_idx];

      // printf("page_num: %d, line_idx: %d, fragment_idx: %d\n", page_num, line_idx, fragment_idx);

      for (unsigned int idx = 0; idx < CACHE_LINE_INDICES; idx++) {
        unsigned int data_matrix_idx = (page_num * PAGE_INDICES) + (line_idx * CACHE_LINE_INDICES) + idx;
        unsigned int new_data_matrix_idx = (((page_num * n) + fragment_idx) * PAGE_INDICES) + (line_idx * CACHE_LINE_INDICES) + idx;

        if (data_matrix_idx < data_matrix.size()) {
          // printf("data_matrix_idx: %x new_idx: %x data_matrix_size: %x new_data_matrix_size: %x index_lut_size %x\n", data_matrix_idx, new_data_matrix_idx, data_matrix.size(), new_data_matrix.size(), index_lut.size());

          new_data_matrix[new_data_matrix_idx] = data_matrix[data_matrix_idx];
          index_lut[data_matrix_idx] = new_data_matrix_idx;

          
        }
      }
    }
  }

  // exit(0);
  
  data_matrix = new_data_matrix;
}

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