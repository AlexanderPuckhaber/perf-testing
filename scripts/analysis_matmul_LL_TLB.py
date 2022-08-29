import pandas as pd
import os
import seaborn as sns; sns.set_theme()
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm, Normalize
import numpy as np
from sklearn.metrics import mean_absolute_percentage_error
import sklearn.metrics

csv_filepath = os.path.join('..', 'data', 'matmul_runner_test_fixed.csv')

df = pd.read_csv(csv_filepath)



df_pivot = df.pivot_table(index=['i', 'j', 'k', 'bs', 'p'], columns='event', values='count', aggfunc=[np.mean])
print(df_pivot)

df_sel = df_pivot[('mean', )]

print(df_sel)
df_sel.reset_index(inplace=True)

df_sel['CACHE_MISS_PER_L1D'] = (df_sel['cache-misses'] / df_sel['L1-dcache-loads'])
df_sel['CACHE_ACCESS_PER_L1D'] = (df_sel['cache-references'] / df_sel['L1-dcache-loads'])
df_sel['DTLB_MISS_PER_L1D'] = (df_sel['dTLB-load-misses'] / df_sel['L1-dcache-loads'])

losses = []

hh = df_sel[['CACHE_ACCESS_PER_L1D', 'CACHE_MISS_PER_L1D', 'DTLB_MISS_PER_L1D']].to_numpy()

target_resnet50_alex_laptop = [0.0620, 0.0202, 0.000206]

for i in range(0, hh.shape[0]):
  l = sklearn.metrics.mean_absolute_percentage_error(target_resnet50_alex_laptop, hh[i])
  losses.append(l)

df_sel['loss'] = losses

print(min(df_sel['loss']))

print(df_sel.sort_values(by='loss'))

# df['Matrix Loading Size'] = (df['i'] * df['j'] + df['i'] * df['k'] + df['j'] * df['k']) * 4 # 4 bytes in a float

K = 16

matmul_description = "MatMul Operation: IxI <= IxK * KxI, K={0}".format(K)


df_filtered = df_sel[(df_sel['k'] == K) & (df_sel['bs'] == 64)]

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


matmul_description = ""


df_filtered = df_sel[(df_sel['i'] == 64) & (df_sel['bs'] == 64)]

df_filtered = df_filtered.groupby(['j', 'k'], as_index=False).mean()

df_select = df_filtered.pivot('j', 'k', 'CACHE_MISS_PER_L1D')
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('J')
ax.set_ylabel('K')
ax.set_title('CACHE_MISS_PER_L1D' + '\n' + matmul_description)
plt.show()

df_select = df_filtered.pivot('j', 'k', 'CACHE_ACCESS_PER_L1D')
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('J')
ax.set_ylabel('K')
ax.set_title('CACHE_ACCESS_PER_L1D' + '\n' + matmul_description)
plt.show()

df_select = df_filtered.pivot('j', 'k', 'DTLB_MISS_PER_L1D')
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('J')
ax.set_ylabel('K')
ax.set_title('DTLB_MISS_PER_L1D' + '\n' + matmul_description)
plt.show()




matmul_description = ""


df_filtered = df_sel[(df_sel['i'] == 64) & (df_sel['k'] == 64)]

df_filtered = df_filtered.groupby(['j', 'bs'], as_index=False).mean()

df_select = df_filtered.pivot('j', 'bs', 'CACHE_MISS_PER_L1D')
print(df_select)
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('J')
ax.set_ylabel('bs')
ax.set_title('CACHE_MISS_PER_L1D' + '\n' + matmul_description)
plt.show()

df_select = df_filtered.pivot('j', 'bs', 'CACHE_ACCESS_PER_L1D')
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('J')
ax.set_ylabel('bs')
ax.set_title('CACHE_ACCESS_PER_L1D' + '\n' + matmul_description)
plt.show()

df_select = df_filtered.pivot('j', 'bs', 'DTLB_MISS_PER_L1D')
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('J')
ax.set_ylabel('bs')
ax.set_title('DTLB_MISS_PER_L1D' + '\n' + matmul_description)
plt.show()