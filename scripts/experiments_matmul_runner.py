import numpy as np
import subprocess
import pandas as pd
import random
import os
import copy

csv_filename = os.path.join('..', 'data', 'matmul_runner_test_large_bs.csv')

I_sizes = [64]

K_sizes = [1024*64]

block_sizes = [1024*8]

methods = ['tiled']

perf_configs = ['CFG_L1_LL', 'CFG_BRANCHES', 'CFG_CYCLES_TLB']

num_samples = 16 * 2


result_dicts = []

params_dicts = []

for sample in range(num_samples):
  for perf_config in perf_configs:
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
              new_param['p'] = perf_config
              params_dicts.append(new_param)

random.shuffle(params_dicts)

results_processed_dicts = []

for param_dict in params_dicts:
  sample = param_dict['sample']
  i = param_dict['i']
  j = param_dict['j']
  k = param_dict['k']
  method = param_dict['method']
  bs = param_dict['bs']
  perf_config = param_dict['p']

  # print(i, k)

  result_dict = {}
  result_dict['sample'] = sample
  result_dict['i'] = i
  result_dict['j'] = i
  result_dict['k'] = k
  result_dict['m'] = method
  result_dict['bs'] = bs
  result_dict['p'] = perf_config

  param_dict_lines = []

  output = subprocess.check_output(['../bin/perf_matmul_runner -i {0} -j {1} -k {2} -m {3} -b {4} -p {5}'.format(i, i, k, method, bs, perf_config)], shell=True, text=True)

  for line in output.splitlines():
    # print(line)
    last_colon_idx = line.rindex(':')
    # print(last_colon_idx)
    output_sliced = line.split(':')
    key = line[:last_colon_idx]
    value = int(output_sliced[-1].strip())
    result_dict[key] = value

    param_dict_line = copy.copy(param_dict)
    param_dict_line['event'] = key
    param_dict_line['count'] = value
    param_dict_lines.append(param_dict_line)
    results_processed_dicts.append(param_dict_line)
  
  # print(result_dict)
  result_dicts.append(result_dict)

  # print(param_dict_lines)
  # results_processed_dicts.append(param_dict_lines)

  # df = pd.DataFrame.from_records(param_dict_lines)
  # print(df)

  print(len(result_dicts), '/', len(params_dicts))          

df = pd.DataFrame.from_records(results_processed_dicts)
print(df)

df.to_csv(csv_filename)
