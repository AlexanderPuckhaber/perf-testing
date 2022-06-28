#include "matmul.h"

void init_2d_vector(std::vector<std::vector<int>>& V, int I, int J, int default_value) {
  V = std::vector<std::vector<int>>(I);
  for (int i = 0; i < V.size(); i++) {
    V[i] = std::vector<int>(J, default_value);
  }
}

void matmul_naive(std::vector<std::vector<int>>& A, std::vector<std::vector<int>>& B, std::vector<std::vector<int>>& C) {
  for (int i = 0; i < C.size(); i++) {
    for (int j = 0; j < C[0].size(); j++) {
      //reads C(i,j) (can be register-allocated in the inner-most loop if we know that C, A, and B cannot alias)
      for (int k = 0; k < A[0].size(); k++) {
        //reads row i, column k of A into cache. cache hit (i,k-1) was read in previous iteration which was adjacent in memory.
        //reads row k, column j of B into cache. CACHE MISS (k-1,j) is far away from (k,j) giving little spatial locality
        C[i][j] = C[i][j] + A[i][k]*B[k][j];
      }
    }
  }
}