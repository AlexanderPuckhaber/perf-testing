#include "matmul.h"
#include <stdio.h>

void init_2d_vector(std::vector<float>& V, int I, int J, float default_value) {
  V = std::vector<float>(I*J, default_value);
}

// https://iitd-plos.github.io/col729/lec/matrix_multiplication.html
// void matmul_naive(std::vector<std::vector<float>>& A, std::vector<std::vector<float>>& B, std::vector<std::vector<float>>& C) {
//   for (int i = 0; i < C.size(); i++) {
//     for (int j = 0; j < C[0].size(); j++) {
//       //reads C(i,j) (can be register-allocated in the inner-most loop if we know that C, A, and B cannot alias)
//       for (int k = 0; k < A[0].size(); k++) {
//         //reads row i, column k of A into cache. cache hit (i,k-1) was read in previous iteration which was adjacent in memory.
//         //reads row k, column j of B into cache. CACHE MISS (k-1,j) is far away from (k,j) giving little spatial locality
//         C[i][j] = C[i][j] + A[i][k]*B[k][j];
//       }
//     }
//   }
// }

// https://stackoverflow.com/questions/26892504/c-performacne-analysis-of-tiled-matrix-multiplication-with-valgrind
void matmul_tiled(std::vector<float>& A, std::vector<float>& B, std::vector<float>& C, int I, int J, int K, int block_size) {
  int incr = block_size;
  int row = I;
  int col = J;
  for (int row_start = 0; row_start < row; row_start += incr) {
    int row_lim = std::min( row_start + incr, row );    
      for (int col_start = 0; col_start < col; col_start += incr) {
        int col_lim = std::min( col_start + incr, col );
        for (int k_start = 0; k_start < K; k_start += incr) {
          int k_lim = std::min( k_start + incr, K );
          for (int block_row = row_start; block_row < row_lim; block_row ++) {
            for (int block_col = col_start; block_col < col_lim; block_col ++) {

              for (int block_k = k_start; block_k < k_lim; block_k ++) {

                  C[block_col*row + block_row] +=  A[ block_k*row + block_row ] * B[ block_col*K + block_k ];

              }
            } 
          }
        }
      }
    }
}

void matmul_tiled_tlb(std::vector<float>& A, std::vector<float>& B, std::vector<float>& C, int I, int J, int K, int block_size, std::vector<uint32_t>& A_index_lut, std::vector<uint32_t>& B_index_lut) {
  int incr = block_size;
  int row = I;
  int col = J;
  for (int row_start = 0; row_start < row; row_start += incr) {
    int row_lim = std::min( row_start + incr, row );    
      for (int col_start = 0; col_start < col; col_start += incr) {
        int col_lim = std::min( col_start + incr, col );
        for (int k_start = 0; k_start < K; k_start += incr) {
          int k_lim = std::min( k_start + incr, K );
          for (int block_col = col_start; block_col < col_lim; block_col ++) {
            for (int block_row = row_start; block_row < row_lim; block_row ++) {

              for (int block_k = k_start; block_k < k_lim; block_k ++) {

                

                // int A_index = A_index_lut[block_k*row + block_row];
                int A_index = A_index_lut[block_row*K + block_k];
                int B_index = B_index_lut[block_col*K + block_k];
                // printf("%x\n", A_index);

                C[block_col*row + block_row] +=  A[ A_index] * B[ B_index ];
                // C[block_col*row + block_row] +=  A[ block_k*row + block_row ] * B[ A_index ];

              }
            } 
          }
        }
      }
    }
}