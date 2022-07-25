import pandas as pd
import os
import seaborn as sns
from torch import rand; sns.set_theme()
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm, Normalize

csv_filepath = os.path.join('..', 'data', 'tlb_test4.csv')

df = pd.read_csv(csv_filepath)

# df['CACHE_MISS_RATIO'] = (df['PERF_COUNT_HW_CACHE_MISSES'] / df['PERF_COUNT_HW_CACHE_REFERENCES'])
df['DTLB_MISS_RATIO'] = (df['DTLB-LOAD-MISSES'] / df['DTLB-LOADS'])
df['L1_DCACHE_MISS_RATIO'] = (df['L1-DCACHE-LOAD-MISSES'] / df['L1-DCACHE-LOADS'])
df['L2_RQSTS:DEMAND_DATA_RD_MISS_RATIO'] = (df['L2_RQSTS:DEMAND_DATA_RD_MISS'] / df['L2_RQSTS:ALL_DEMAND_DATA_RD'])
df['PERF_COUNT_HW_CACHE_LL:READ:MISS_RATIO'] = (df['PERF_COUNT_HW_CACHE_LL:READ:MISS'] / df['PERF_COUNT_HW_CACHE_LL:READ:ACCESS'])

df['DTLB-LL_MISS_RATIO_RATIO'] = (df['DTLB_MISS_RATIO'] / df['PERF_COUNT_HW_CACHE_LL:READ:MISS_RATIO'])
df['DTLB-MISSES/LL-MISSES'] = (df['DTLB-LOAD-MISSES'] / df['PERF_COUNT_HW_CACHE_LL:READ:MISS'])

subplot_fields = []
subplot_fields.append(['DTLB-LOADS', 'L1-DCACHE-LOADS', 'L2_RQSTS:ALL_DEMAND_DATA_RD', 'PERF_COUNT_HW_CACHE_LL:READ:ACCESS'])
subplot_fields.append(['DTLB-LOAD-MISSES', 'L1-DCACHE-LOAD-MISSES', 'L2_RQSTS:DEMAND_DATA_RD_MISS', 'PERF_COUNT_HW_CACHE_LL:READ:MISS'])
subplot_fields.append(['DTLB_MISS_RATIO', 'L1_DCACHE_MISS_RATIO', 'L2_RQSTS:DEMAND_DATA_RD_MISS_RATIO', 'PERF_COUNT_HW_CACHE_LL:READ:MISS_RATIO'])
subplot_fields.append(['DTLB-LL_MISS_RATIO_RATIO', 'DTLB-MISSES/LL-MISSES'])

for randomize in [0, 1]:


  df_filtered = df[(df['r'] == randomize)]

  # df_filtered = df[(df['c'] == 'LL') & (df['r'] == 0)]

  df_filtered = df_filtered.groupby(['m', 's'], as_index=False).mean()

  for subplot_field_set in subplot_fields:
    fig, axs = plt.subplots(2, 2)
    i = 0
    for field in subplot_field_set:
      df_select = df_filtered.pivot('m', 's', field)
      ax = axs[i//2][i%2]
      ax = sns.heatmap(df_select, cmap='YlGnBu', ax=ax, norm=LogNorm())
      ax.set_xlabel('stride')
      ax.set_ylabel('map size')
      ax.set_title(field + '\n')
      i += 1

    fig.subplots_adjust(hspace=0.41)
    fig.suptitle("Randomize: {0}".format(randomize))

    plt.show()


