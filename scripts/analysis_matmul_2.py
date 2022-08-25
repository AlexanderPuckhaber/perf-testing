import pandas as pd
import os
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np

csv_filepath = os.path.join('..', 'data', 'matmul_runner_test.csv')

df = pd.read_csv(csv_filepath)


df['hue'] = df['arg.profile'] + ' ' + df['hardware']

hue_order = []

for model in ['resnet50-onnxruntime', 'ssd-mobilenet-onnxruntime', 'ssd-resnet34-onnxruntime']:
  for hardware in ['alex laptop', 'hummer']:
    hue_order.append(model + ' ' + hardware)

# print(df)
# print(hue_order)

ax = sns.catplot(x="event", y="count", hue='hue', hue_order=hue_order, data=df, kind="bar", palette='muted', log=True, legend=False)

plt.xticks(rotation=60)

# plt.tight_layout()
plt.legend(loc='upper center', borderaxespad=0)
plt.show()

df_pivot = df.pivot_table(index=['arg.profile', 'hardware'], columns='event', values='count', aggfunc=[np.mean, np.std])

ratio_dict = {}
ratio_dict['L1-dcache-load-miss-ratio'] = {'denom': 'L1-dcache-loads', 'num': 'L1-dcache-load-misses'}
ratio_dict['LL-cache-miss-ratio'] = {'denom': 'cache-references', 'num': 'cache-misses'}
ratio_dict['dTLB-load-miss-ratio'] = {'denom': 'L1-dcache-loads', 'num': 'dTLB-load-misses'}
ratio_dict['branch-miss-ratio'] = {'denom': 'branches', 'num': 'branch-misses'}

for key in ratio_dict.keys():
  df_pivot[('mean', key)] = df_pivot[('mean', ratio_dict[key]['num'])] / df_pivot[('mean', ratio_dict[key]['denom'])]


df_sel = df_pivot[('mean', )][ratio_dict.keys()]

print(df_sel)
df_sel.reset_index(inplace=True)

df_sel['hue'] = df_sel['arg.profile'] + ' ' + df_sel['hardware']
print(df_sel)
df_sel_melted = df_sel.melt(id_vars=['arg.profile', 'hardware', 'hue'])
print(df_sel_melted)

ax = sns.catplot(x='event', y='value', data=df_sel_melted, kind='bar', hue='hue')
plt.xticks(rotation=10)
plt.show()

for key in ratio_dict.keys():
  ax = sns.catplot(x='arg.profile', y=key, data=df_sel, kind='bar', hue='hardware')
  plt.xticks(rotation=10)
  plt.show()

ax = sns.catplot(x="event", y="computed_value", hue='hue', hue_order=hue_order, data=df.loc[df['event'].isin(['L1-dcache-load-misses', 'cache-misses'])], kind="bar", palette='muted')
plt.show()

# df_mean = df_pivot['mean']

# df['L1-dcache-loads-miss-ratio'] = df['L1-dcache-load-misses'] / df['L1-dcache-loads']