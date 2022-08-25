import numpy as np
import subprocess
import pandas as pd
import random
import os

csv_filename = os.path.join('..', 'data', 'tlb_test_rnet50.csv')

map_sizes = np.logspace(12, 26, num=15, base=2)
map_sizes = np.logspace(20, 26, num=15, base=2)
# map_sizes = [65536]
# stride_sizes = np.logspace(6, 13, num=8, base=2)
# stride_sizes = [128, 256, 512, 1024, 2048]
stride_sizes = [64, 256]
access_counts = [1000 * 1000]
randomize_settings = [0]
# randomize_settings = [1]
cache_levels = ['L1', 'L2', 'LL']
cache_levels = ['LL']

num_samples = 16 * 2

result_dicts = []

params_dicts = []

for sample in range(num_samples):
  for map_size in map_sizes:
    for stride in stride_sizes:
      for access_count in access_counts:
        for randomize in randomize_settings:
          for cache_level in cache_levels:
            if (map_size > stride):
              new_param = {}
              new_param['sample'] = int(sample)
              new_param['m'] = int(map_size)
              new_param['s'] = int(stride)
              new_param['n'] = int(access_count)
              new_param['r'] = int(randomize)
              new_param['c'] = cache_level
              params_dicts.append(new_param)

random.shuffle(params_dicts)

for param_dict in params_dicts:
  result_dict = {}

  arg_str = '../bin/perf_tlb_test '
  for key in param_dict.keys():
    result_dict[key] = param_dict[key]

    if key != 'sample':
      arg_str += '-{0} {1} '.format(key, param_dict[key])


  output = subprocess.check_output([arg_str], shell=True, text=True)

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
