#include <vector>
#include <inttypes.h>

void matmul_naive(std::vector<std::vector<float>>& A, std::vector<std::vector<float>>& B, std::vector<std::vector<float>>& C);

void init_2d_vector(std::vector<std::vector<float>>& V, int I, int J, float default_value = 0);