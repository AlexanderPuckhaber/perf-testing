#include <vector>
#include <inttypes.h>

void init_2d_vector(std::vector<float>& V, int I, int J, float default_value = 0);

void matmul_naive(std::vector<std::vector<float>>& A, std::vector<std::vector<float>>& B, std::vector<std::vector<float>>& C);

void matmul_tiled(std::vector<float>& A, std::vector<float>& B, std::vector<float>& C, int I, int J, int K, int block_size);

void matmul_tiled_tlb(std::vector<float>& A, std::vector<float>& B, std::vector<float>& C, int I, int J, int K, int block_size, std::vector<uint32_t>& A_index_lut, std::vector<uint32_t>& B_index_lut);