import pandas as pd
import os
import seaborn as sns
from torch import rand; sns.set_theme()
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm, Normalize
import numpy as np
from matplotlib.ticker import FormatStrFormatter

csv_filepath = os.path.join('..', 'data', 'mlcommons_targets.csv')

df = pd.read_csv(csv_filepath)

# df['CACHE_MISS_RATIO'] = (df['PERF_COUNT_HW_CACHE_MISSES'] / df['PERF_COUNT_HW_CACHE_REFERENCES'])
df['DTLB_MISS_RATIO'] = (df['DTLB-LOAD-MISSES'] / df['DTLB-LOADS'])
# df['L1_DCACHE_MISS_RATIO'] = (df['L1-DCACHE-LOAD-MISSES'] / df['L1-DCACHE-LOADS'])
# df['L2_RQSTS:DEMAND_DATA_RD_MISS_RATIO'] = (df['L2_RQSTS:DEMAND_DATA_RD_MISS'] / df['L2_RQSTS:ALL_DEMAND_DATA_RD'])
df['PERF_COUNT_HW_CACHE_LL:READ:MISS_RATIO'] = (df['PERF_COUNT_HW_CACHE_LL:READ:MISS'] / df['PERF_COUNT_HW_CACHE_LL:READ:ACCESS'])

df['DTLB-LL_MISS_RATIO_RATIO'] = (df['DTLB_MISS_RATIO'] / df['PERF_COUNT_HW_CACHE_LL:READ:MISS_RATIO'])
df['DTLB-MISSES/LL-MISSES'] = (df['DTLB-LOAD-MISSES'] / df['PERF_COUNT_HW_CACHE_LL:READ:MISS'])

subplot_fields = []
# subplot_fields.append(['DTLB-LOADS', 'L1-DCACHE-LOADS', 'L2_RQSTS:ALL_DEMAND_DATA_RD', 'PERF_COUNT_HW_CACHE_LL:READ:ACCESS'])
# subplot_fields.append(['DTLB-LOAD-MISSES', 'L1-DCACHE-LOAD-MISSES', 'L2_RQSTS:DEMAND_DATA_RD_MISS', 'PERF_COUNT_HW_CACHE_LL:READ:MISS'])
# subplot_fields.append(['DTLB_MISS_RATIO', 'L1_DCACHE_MISS_RATIO', 'L2_RQSTS:DEMAND_DATA_RD_MISS_RATIO', 'PERF_COUNT_HW_CACHE_LL:READ:MISS_RATIO'])
# subplot_fields.append(['DTLB-LL_MISS_RATIO_RATIO', 'DTLB-MISSES/LL-MISSES'])

for k in [1]:


  # df_filtered = df[(df['k'] == k)]
  df_filtered = df

  # df_filtered = df[(df['c'] == 'LL') & (df['r'] == 0)]

  df_averaged = df_filtered.groupby(['i', 'j', 'k', 'm', 'bs'], as_index=False).mean()

  # df_filtered = df_averaged


  labels = ['resnet50', 'ssd-resnet34', 'ssd-mobilenet']
  target_dtlb_ll = pd.DataFrame({'DTLB_MISS_RATIO': [0.01903, 0.0093899, 0.04511], 'PERF_COUNT_HW_CACHE_LL:READ:MISS_RATIO': [30.926, 11.717, 23.7965]})
  target_dtlb_ll *= 0.01
  target_dtlb_ll['DTLB-LOADS'] = [3440537388, 241844148098, 1178625931]
  target_dtlb_ll['PERF_COUNT_HW_CACHE_LL:READ:ACCESS'] = [208571274.5, 15999679307, 108443104.3]
  target_dtlb_ll['label'] = labels
  print(target_dtlb_ll)

  data_len = len(df_filtered['DTLB_MISS_RATIO'])
  target_data_len = len(target_dtlb_ll['DTLB_MISS_RATIO'])

  # x_l = 'DTLB-LOADS'
  # y_l = 'PERF_COUNT_HW_CACHE_LL:READ:ACCESS'
  x_l = 'DTLB_MISS_RATIO'
  y_l = 'PERF_COUNT_HW_CACHE_LL:READ:MISS_RATIO'

  x = list(df_filtered[x_l]) + list(target_dtlb_ll[x_l])
  y = list(df_filtered[y_l]) + list(target_dtlb_ll[y_l])

  print(x)

  colormap = np.empty((data_len + target_data_len,), dtype=object)
  alphamap = np.ones_like(colormap) * 0.5
  edgecolors = np.empty_like(colormap, dtype=object)
  # color_on = 'bs'
  # colormap[:data_len] = np.log2(df_filtered[color_on])
  colormap[:data_len] = 'black'
  resnet50_ind = df_filtered.index[(df_filtered['i'] == 32)].to_list()
  resnet34_ind = df_filtered.index[(df_filtered['i'] == 23)].to_list()
  mobilenet_ind = df_filtered.index[(df_filtered['i'] == 28)].to_list()
  colormap[resnet50_ind] = 'red'
  colormap[resnet34_ind] = 'green'
  colormap[mobilenet_ind] = 'blue'
  # colormap[df_filtered['i'] == 32]
  colormap[data_len:] = ['red', 'green', 'blue']
  # print('min:', np.min(colormap))
  # colormap -= np.min(colormap)
  alphamap[data_len:] = 1
  edgecolors[:data_len] = 'red'
  edgecolors[data_len:] = 'black'
  print(colormap)

  fig = plt.figure()
  ax = plt.gca()
  ax.scatter(x, y, c=colormap, alpha=alphamap, edgecolor='black', cmap='viridis')
  ax.set_yscale('log')
  ax.set_xscale('log')
  ax.set_xlabel(x_l)
  ax.set_ylabel('LL_MISS_RATIO')
  for index, row in target_dtlb_ll.iterrows():
    x, y = (row[x_l], row[y_l])
    label_point = (x, y)
    ax.annotate(row['label'], label_point)
    

    # find closest point
    dist_name = 'dist_{}'.format(row['label'])
    df_averaged[dist_name] = np.sqrt((x - df_averaged[x_l])**2 + (y - df_averaged[y_l])**2)
    print(df_averaged.sort_values(by=dist_name)[['i', 'j', 'k', 'bs', x_l, y_l, dist_name]])


  plt.show()

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
    # fig.suptitle("Randomize: {0}".format(randomize))

    plt.show()


