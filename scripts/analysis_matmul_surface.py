import pandas as pd
import os
import seaborn as sns; sns.set_theme()
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm, Normalize
import numpy as np
from sklearn.metrics import mean_absolute_percentage_error
import sklearn.metrics

csv_filepath = os.path.join('..', 'data', 'matmul_row_col_runner_test_fixed_surface9.csv')
csv_filepath = os.path.join('..', 'data', 'matmul_runner_test_fixed_surface8.csv')

df = pd.read_csv(csv_filepath)


df_pivot = df.pivot_table(index=['i', 'j', 'k', 'bs'], columns='event', values='count', aggfunc=[np.mean])
print(df_pivot)



df_sel = df_pivot[('mean', )]

print(df_sel)

# print(df[(df['i'] == 2) & (df['j'] == 37419)])

# df_filtered = df_sel.groupby(['i', 'j', 'k', 'bs'], as_index=True).mean()
# print(df_filtered)
# print(a)

print(df_sel)
df_sel.reset_index(inplace=True)

print(df_sel['i'])

df_sel['est_mem_loads'] = (df_sel['i'] * df_sel['j']) * (df_sel['k'] * 2)
df_sel['L1-dcache-loads-per-est_mem_loads'] = df_sel['L1-dcache-loads'] / df_sel['est_mem_loads']

df_sel['CACHE_MISS_PER_L1D'] = (df_sel['cache-misses'] / df_sel['L1-dcache-loads'])
df_sel['CACHE_ACCESS_PER_L1D'] = (df_sel['cache-references'] / df_sel['L1-dcache-loads'])
df_sel['DTLB_MISS_PER_L1D'] = (df_sel['dTLB-load-misses'] / df_sel['L1-dcache-loads'])

# Instruction count data was not collected in this file...

mean_abs_per_err = []
msle_err = []

hh = df_sel[['CACHE_ACCESS_PER_L1D', 'CACHE_MISS_PER_L1D', 'DTLB_MISS_PER_L1D']].to_numpy()
print(hh)

target_resnet50_alex_laptop = [0.0620, 0.0202, 0.000206]

for i in range(0, hh.shape[0]):
  mape = sklearn.metrics.mean_absolute_percentage_error(target_resnet50_alex_laptop, hh[i])
  mean_abs_per_err.append(mape)
  msle = sklearn.metrics.mean_squared_log_error(target_resnet50_alex_laptop, hh[i])
  msle_err.append(msle)

df_sel['mean_absolute_percentage_error'] = mean_abs_per_err
df_sel['mean_squared_log_error'] = msle_err

# print(min(df_sel['loss']))

df_sel.sort_values(by='mean_absolute_percentage_error')
df_sel.to_csv(os.path.join('..', 'data', 'matmul_runner_test_fixed_surface-targets.csv'))

print(df)

# # df['Matrix Loading Size'] = (df['i'] * df['j'] + df['i'] * df['k'] + df['j'] * df['k']) * 4 # 4 bytes in a float

K = 16

matmul_description = "MatMul Operation: IxJ <= IxK * KxJ, bs=?"


# df_filtered = df_sel[(df_sel['k'] == K) & (df_sel['bs'] == 64)]
df_filtered = df_sel

df_filtered = df_filtered.groupby(['i', 'j'], as_index=False).mean()

# df_select = df_filtered.pivot('i', 'bs', 'Matrix Loading Size')
# ax = sns.heatmap(df_select, cmap='YlGnBu', norm=LogNorm())
# ax.set_xlabel('Block Size')
# ax.set_ylabel('I')
# ax.set_title('Matrix Loading Size' + '\n' + matmul_description)
# plt.show()

# df_select = df_filtered.pivot('i', 'bs', 'PERF_COUNT_HW_CACHE_REFERENCES')
# ax = sns.heatmap(df_select, cmap='YlGnBu', norm=LogNorm())
# ax.set_xlabel('Block Size')
# ax.set_ylabel('I')
# ax.set_title('LLC Cache References' + '\n' + matmul_description)
# plt.show()

# df_select = df_filtered.pivot('i', 'bs', 'PERF_COUNT_HW_CACHE_MISSES')
# ax = sns.heatmap(df_select, cmap='YlGnBu', norm=LogNorm())
# ax.set_xlabel('Block Size')
# ax.set_ylabel('I')
# ax.set_title('LLC Cache Misses' + '\n' + matmul_description)
# plt.show()

# df_select = df_filtered.pivot('i', 'bs', 'CACHE_MISS_RATIO')
# ax = sns.heatmap(df_select, cmap='YlGnBu')
# ax.set_xlabel('Block Size')
# ax.set_ylabel('I')
# ax.set_title('LLC Cache Miss Ratio' + '\n' + matmul_description)
# plt.show()

df_select = df_filtered.pivot('i', 'j', 'k')
ax = sns.heatmap(df_select, cmap='YlGnBu', norm=LogNorm())
ax.set_xlabel('J')
ax.set_ylabel('I')
ax.set_title('K' + '\n' + matmul_description)
plt.show()

df_select = df_filtered.pivot('i', 'j', 'est_mem_loads')
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('J')
ax.set_ylabel('I')
ax.set_title('est_mem_loads' + '\n' + matmul_description)
plt.show()

df_select = df_filtered.pivot('i', 'j', 'L1-dcache-loads')
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('J')
ax.set_ylabel('I')
ax.set_title('L1-dcache-loads' + '\n' + matmul_description)
plt.show()

df_select = df_filtered.pivot('i', 'j', 'L1-dcache-loads-per-est_mem_loads')
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('J')
ax.set_ylabel('I')
ax.set_title('L1-dcache-loads-per-est_mem_loads' + '\n' + matmul_description)
plt.show()

df_select = df_filtered.pivot('i', 'j', 'CACHE_MISS_PER_L1D')
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('J')
ax.set_ylabel('I')
ax.set_title('CACHE_MISS_PER_L1D' + '\n' + matmul_description)
plt.show()

df_select = df_filtered.pivot('i', 'j', 'CACHE_ACCESS_PER_L1D')
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('J')
ax.set_ylabel('I')
ax.set_title('CACHE_ACCESS_PER_L1D' + '\n' + matmul_description)
plt.show()

df_select = df_filtered.pivot('i', 'j', 'DTLB_MISS_PER_L1D')
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('J')
ax.set_ylabel('I')
ax.set_title('DTLB_MISS_PER_L1D' + '\n' + matmul_description)
plt.show()

# df_select = df_filtered.pivot('i', 'bs', 'PERF_COUNT_HW_CPU_CYCLES')
# ax = sns.heatmap(df_select, cmap='YlGnBu')
# ax.set_xlabel('Block Size')
# ax.set_ylabel('I')
# ax.set_title('CPU CYCLES' + '\n' + matmul_description)
# plt.show()


# df_select = df_filtered.pivot('i', 'bs', 'PERF_COUNT_HW_CACHE_L1D')
# ax = sns.heatmap(df_select, cmap='YlGnBu')
# ax.set_xlabel('Block Size')
# ax.set_ylabel('I')
# ax.set_title('PERF_COUNT_HW_CACHE_L1D' + '\n' + matmul_description)
# plt.show()

# df_select = df_filtered.pivot('i', 'bs', 'PERF_COUNT_SW_PAGE_FAULTS')
# ax = sns.heatmap(df_select, cmap='YlGnBu')
# ax.set_xlabel('Block Size')
# ax.set_ylabel('I')
# ax.set_title("PERF_COUNT_SW_PAGE_FAULTS" + '\n' + matmul_description)
# plt.show()


# matmul_description = ""


# # df_filtered = df_sel[(df_sel['i'] == 64) & (df_sel['bs'] == 64)]

# df_filtered = df_filtered.groupby(['j', 'k'], as_index=False).mean()

# df_select = df_filtered.pivot('j', 'k', 'CACHE_MISS_PER_L1D')
# ax = sns.heatmap(df_select, cmap='YlGnBu')
# ax.set_xlabel('J')
# ax.set_ylabel('K')
# ax.set_title('CACHE_MISS_PER_L1D' + '\n' + matmul_description)
# plt.show()

# df_select = df_filtered.pivot('j', 'k', 'CACHE_ACCESS_PER_L1D')
# ax = sns.heatmap(df_select, cmap='YlGnBu')
# ax.set_xlabel('J')
# ax.set_ylabel('K')
# ax.set_title('CACHE_ACCESS_PER_L1D' + '\n' + matmul_description)
# plt.show()

# df_select = df_filtered.pivot('j', 'k', 'DTLB_MISS_PER_L1D')
# ax = sns.heatmap(df_select, cmap='YlGnBu')
# ax.set_xlabel('J')
# ax.set_ylabel('K')
# ax.set_title('DTLB_MISS_PER_L1D' + '\n' + matmul_description)
# plt.show()




# matmul_description = ""


# df_filtered = df_sel[(df_sel['i'] == 64) & (df_sel['k'] == 64)]

# df_filtered = df_filtered.groupby(['j', 'bs'], as_index=False).mean()

# df_select = df_filtered.pivot('j', 'bs', 'CACHE_MISS_PER_L1D')
# print(df_select)
# ax = sns.heatmap(df_select, cmap='YlGnBu')
# ax.set_xlabel('bs')
# ax.set_ylabel('J')
# ax.set_title('CACHE_MISS_PER_L1D' + '\n' + matmul_description)
# plt.show()

# df_select = df_filtered.pivot('j', 'bs', 'CACHE_ACCESS_PER_L1D')
# ax = sns.heatmap(df_select, cmap='YlGnBu')
# ax.set_xlabel('bs')
# ax.set_ylabel('J')
# ax.set_title('CACHE_ACCESS_PER_L1D' + '\n' + matmul_description)
# plt.show()

# df_select = df_filtered.pivot('j', 'bs', 'DTLB_MISS_PER_L1D')
# ax = sns.heatmap(df_select, cmap='YlGnBu')
# ax.set_xlabel('bs')
# ax.set_ylabel('J')
# ax.set_title('DTLB_MISS_PER_L1D' + '\n' + matmul_description)
# plt.show()