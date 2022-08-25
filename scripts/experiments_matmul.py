import numpy as np
import subprocess
import pandas as pd
import random
import os

csv_filename = os.path.join('..', 'data', 'matmul_test_resnet34_2.csv')

min_I = 4
max_I = 5

min_K = 8
max_K = 14

methods = ['tiled']

# block_sizes = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
# block_sizes = [256, 512, 1024, 2048, 4096, 4096*4, 4096*16, 4096*256]
block_sizes = np.logspace(12, 16, num=8, base=2)
# block_sizes = [2**18]

num_samples = 16 * 1

# I_sizes = np.arange(min_I, max_I)

I_sizes = np.logspace(min_I, max_I, num=8, base=2)
# I_sizes = [6, 8, 10, 20]
# I_sizes = [8]

# K_sizes = np.arange(min_K, max_K)
# K_sizes = [4096*1024]
K_sizes = np.logspace(15, 16, num=8, base=2)

block_sizes = np.array([44102])
I_sizes = [23]
K_sizes = np.array([48693])

result_dicts = []

params_dicts = []

for sample in range(num_samples):
  for i in I_sizes:
    for k in K_sizes:
      for method in methods:
        for bs in block_sizes:
          if bs > k:
            pass
          else:
            new_param = {}
            new_param['sample'] = int(sample)
            new_param['i'] = int(i)
            new_param['j'] = int(i)
            new_param['k'] = int(k)
            new_param['method'] = method
            new_param['bs'] = int(bs)
            params_dicts.append(new_param)

random.shuffle(params_dicts)

for param_dict in params_dicts:
  sample = param_dict['sample']
  i = param_dict['i']
  j = param_dict['j']
  k = param_dict['k']
  method = param_dict['method']
  bs = param_dict['bs']

  # print(i, k)

  result_dict = {}
  result_dict['sample'] = sample
  result_dict['i'] = i
  result_dict['j'] = i
  result_dict['k'] = k
  result_dict['m'] = method
  result_dict['bs'] = bs

  output = subprocess.check_output(['../bin/perf_matmul_test -i {0} -j {1} -k {2} -m {3} -b {4}'.format(i, i, k, method, bs)], shell=True, text=True)

  for line in output.splitlines():
    print(line)
    last_colon_idx = line.rindex(':')
    print(last_colon_idx)
    output_sliced = line.split(':')
    result_dict[line[:last_colon_idx]] = int(output_sliced[-1].strip())
  
  # print(result_dict)
  result_dicts.append(result_dict)

  print(len(result_dicts), '/', len(params_dicts))          

df = pd.DataFrame.from_records(result_dicts)
print(df)

df.to_csv(csv_filename)
