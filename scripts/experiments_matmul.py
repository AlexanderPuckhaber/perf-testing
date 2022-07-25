import numpy as np
import subprocess
import pandas as pd
import random
import os

csv_filename = os.path.join('..', 'data', 'matmul_test_14.csv')

min_I = 8
max_I = 9

min_K = 8
max_K = 11

methods = ['tiled']

# block_sizes = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
block_sizes = np.logspace(4, 9, num=24, base=2)

num_samples = 4

# I_sizes = np.arange(min_I, max_I)

I_sizes = np.logspace(min_I, max_I, num=2, base=2)

# K_sizes = np.arange(min_K, max_K)
K_sizes = [512]

result_dicts = []

params_dicts = []

for sample in range(num_samples):
  for i in I_sizes:
    for k in K_sizes:
      for method in methods:
        for bs in block_sizes:
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
    output_sliced = line.split(':')
    result_dict[output_sliced[-2].strip()] = int(output_sliced[-1].strip())
  
  # print(result_dict)
  result_dicts.append(result_dict)

  print(len(result_dicts), '/', len(params_dicts))          

df = pd.DataFrame.from_records(result_dicts)
print(df)

df.to_csv(csv_filename)
