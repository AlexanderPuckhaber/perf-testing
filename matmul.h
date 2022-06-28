#include <vector>
#include <inttypes.h>

void matmul_naive(std::vector<std::vector<int>>& A, std::vector<std::vector<int>>& B, std::vector<std::vector<int>>& C);

void init_2d_vector(std::vector<std::vector<int>>& V, int I, int J, int default_value = 0);