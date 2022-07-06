import numpy as np
import subprocess
import pandas as pd

min_I = 8
max_I = 9

min_K = 8
max_K = 11

methods = ['tiled']

# block_sizes = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
block_sizes = np.logspace(7, 9, num=9, base=2)

num_samples = 1

# I_sizes = np.arange(min_I, max_I)

I_sizes = np.logspace(min_I, max_I, num=9, base=2)

# K_sizes = np.arange(min_K, max_K)
K_sizes = [1024]

result_dicts = []

for sample in range(num_samples):
  for i in I_sizes:
    for k in K_sizes:
      for method in methods:
        for bs in block_sizes:
          # i = 2 ** i_exp
          # k = 2 ** k_exp
          i = int(i)
          k = int(k)
          bs = int(bs)
          print(i, k)

          result_dict = {}
          result_dict['sample'] = sample
          result_dict['i'] = i
          result_dict['j'] = i
          result_dict['k'] = k
          result_dict['m'] = method
          result_dict['bs'] = bs

          output = subprocess.check_output(['./a.out -i {0} -j {1} -k {2} -m {3} -b {4}'.format(i, i, k, method, bs)], shell=True, text=True)

          for line in output.splitlines():
            output_sliced = line.split(':')
            result_dict[output_sliced[0].strip()] = int(output_sliced[1].strip())
          
          print(result_dict)
          result_dicts.append(result_dict)

df = pd.DataFrame.from_records(result_dicts)
print(df)

df.to_csv('matmul_test7.csv')
