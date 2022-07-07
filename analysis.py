import pandas as pd
import os
import seaborn as sns; sns.set_theme()
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm, Normalize

df = pd.read_csv('matmul_test13.csv')
K = 512
matmul_description = "MatMul Operation: IxI <= IxK * KxI, K={0}".format(K)

df['CACHE_MISS_RATIO'] = (df['PERF_COUNT_HW_CACHE_MISSES'] / df['PERF_COUNT_HW_CACHE_REFERENCES'])

df['Matrix Loading Size'] = (df['i'] * df['j'] + df['i'] * df['k'] + df['j'] * df['k']) * 4 # 4 bytes in a float

df_filtered = df[df['k'] == K]

df_filtered = df_filtered.groupby(['i', 'bs'], as_index=False).mean()

df_select = df_filtered.pivot('i', 'bs', 'Matrix Loading Size')
ax = sns.heatmap(df_select, cmap='YlGnBu', norm=LogNorm())
ax.set_xlabel('Block Size')
ax.set_ylabel('I')
ax.set_title('Matrix Loading Size' + '\n' + matmul_description)
plt.show()

df_select = df_filtered.pivot('i', 'bs', 'PERF_COUNT_HW_CACHE_REFERENCES')
ax = sns.heatmap(df_select, cmap='YlGnBu', norm=LogNorm())
ax.set_xlabel('Block Size')
ax.set_ylabel('I')
ax.set_title('LLC Cache References' + '\n' + matmul_description)
plt.show()

df_select = df_filtered.pivot('i', 'bs', 'PERF_COUNT_HW_CACHE_MISSES')
ax = sns.heatmap(df_select, cmap='YlGnBu', norm=LogNorm())
ax.set_xlabel('Block Size')
ax.set_ylabel('I')
ax.set_title('LLC Cache Misses' + '\n' + matmul_description)
plt.show()

df_select = df_filtered.pivot('i', 'bs', 'CACHE_MISS_RATIO')
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('Block Size')
ax.set_ylabel('I')
ax.set_title('LLC Cache Miss Ratio' + '\n' + matmul_description)
plt.show()

df_select = df_filtered.pivot('i', 'bs', 'PERF_COUNT_HW_CPU_CYCLES')
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('Block Size')
ax.set_ylabel('I')
ax.set_title('CPU CYCLES' + '\n' + matmul_description)
plt.show()


df_select = df_filtered.pivot('i', 'bs', 'PERF_COUNT_HW_CACHE_L1D')
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('Block Size')
ax.set_ylabel('I')
ax.set_title('PERF_COUNT_HW_CACHE_L1D' + '\n' + matmul_description)
plt.show()

df_select = df_filtered.pivot('i', 'bs', 'PERF_COUNT_SW_PAGE_FAULTS')
ax = sns.heatmap(df_select, cmap='YlGnBu')
ax.set_xlabel('Block Size')
ax.set_ylabel('I')
ax.set_title("PERF_COUNT_SW_PAGE_FAULTS" + '\n' + matmul_description)
plt.show()
