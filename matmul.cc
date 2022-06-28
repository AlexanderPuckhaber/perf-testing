#include "matmul.h"

void init_2d_vector(std::vector<std::vector<float>>& V, int I, int J, float default_value) {
  V = std::vector<std::vector<float>>(I);
  for (int i = 0; i < V.size(); i++) {
    V[i] = std::vector<float>(J, default_value);
  }
}

// https://iitd-plos.github.io/col729/lec/matrix_multiplication.html
void matmul_naive(std::vector<std::vector<float>>& A, std::vector<std::vector<float>>& B, std::vector<std::vector<float>>& C) {
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

// https://stackoverflow.com/questions/26892504/c-performacne-analysis-of-tiled-matrix-multiplication-with-valgrind
void matmul_tiled(std::vector<std::vector<float>>& A, std::vector<std::vector<float>>& B, std::vector<std::vector<float>>& C, int block_size) {
  int incr = block_size;
  int row = C.size();
  int col = C[0].size();
  int K = B.size();
  for (int i = 0; i < row; i += incr) {
    int x_lim = std::min( i + incr, row );
      for (int j = 0; j < col; j += incr) {
        int y_lim = std::min( j + incr, col );
        C[i][j] = (float) 0.0;
        for (int k = 0; k < K; k += incr) {
          int z_lim = std::min( k + incr, K );
          for (int x = i; x < x_lim; x++) {
            for (int y = j; y < y_lim; y++) {
              for (int z = k; z < z_lim; z++) {

                  C[ x ][y] +=  A[ x ][ z ] * B[ z ][ y  ];

              }
            } 
          }
        }
      }
    }
}