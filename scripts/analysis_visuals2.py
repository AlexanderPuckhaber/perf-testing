import pandas as pd
import os
import seaborn as sns
from torch import rand; sns.set_theme()
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm, Normalize
import numpy as n
import sklearn.metrics


csv_filepath = os.path.join('..', 'data', 'matmul_test10.csv')

df = pd.read_csv(csv_filepath)

df = df.groupby(['i', 'bs'], as_index=False).mean()

# df_filtered = df[df['k'] == 1024]

# print(df_filtered)

# # need to de-duplicate
# df_filtered = df_filtered.groupby(['i', 'bs'], as_index=False).mean()

# coords = df_filtered[['i', 'bs']].to_numpy()

# data = df_filtered['PERF_COUNT_HW_CACHE_MISSES'].to_numpy()

# print(coords.shape, data.shape)

# # print(data)



# xgrid = np.mgrid[256:512:150j, 128:512:150j]
# xflat = xgrid.reshape(2, -1).T

# a = RBFInterpolator(coords, data)(xflat)

# print(a)

# ygrid = a.reshape(150, 150)
# print(ygrid)

# fig, ax = plt.subplots()
# ax.pcolormesh(*xgrid, ygrid)
# p = ax.scatter(*coords.T, c=data, ec='k')

# plt.show()

# coords_train, coords_test, data_train, data_test = train_test_split(coords, data, test_size=0.95)

# reg = make_pipeline(StandardScaler(), GaussianProcessRegressor())

# reg.fit(coords_train, data_train)

# ee = reg.predict(xflat)

# print(ee)

# fig, ax = plt.subplots()
# ygrid = ee.reshape(150, 150)
# ax.pcolormesh(*xgrid, ygrid, vmin=0, vmax=5668997)
# p = ax.scatter(*coords_train.T, c=data_train, ec='k', s=200, vmin=0, vmax=5668997)
# p = ax.scatter(*coords_test.T, c=data_test, vmin=0, vmax=5668997)
# plt.show()

mean_abs_per_err = []
msle_err = []

hh = df_sel[['CACHE_ACCESS_PER_L1D', 'CACHE_MISS_PER_L1D', 'DTLB_MISS_PER_L1D']].to_numpy()

target_resnet50_alex_laptop = [0.0620, 0.0202, 0.000206]

for i in range(0, hh.shape[0]):
  mape = sklearn.metrics.mean_absolute_percentage_error(target_resnet50_alex_laptop, hh[i])
  mean_abs_per_err.append(mape)
  msle = sklearn.metrics.mean_squared_log_error(target_resnet50_alex_laptop, hh[i])
  msle_err.append(msle)

df_sel['mean_absolute_percentage_error'] = mean_abs_per_err
df_sel['mean_squared_log_error'] = msle_err
print(df)
print(target)



# fig, ax = plt.subplots()
# plt.tricontourf(df['i'], df['bs'], df['loss'], levels=50)
# p = ax.scatter(x=df['i'], y=df['bs'], c=df['loss'], ec='w')
# # ax.colorbar()
# plt.show()

# fig, ax = plt.subplots()

# plt.show()

# print(hh)
# print(target)
# print(np.tile(target, (hh.shape[0],1)))
# print(mean_absolute_percentage_error(np.tile(target, (hh.shape[0],1)), hh))

# df['loss'] = mean_absolute_percentage_error(target, df[['PERF_COUNT_HW_CACHE_L1D', 'PERF_COUNT_HW_CACHE_MISSES', 'PERF_COUNT_HW_CACHE_REFERENCES', 'PERF_COUNT_HW_CPU_CYCLES']])

# def f(x):
#   i, k, bs = x
#   df_select = df[(df['i'] == i) & (df['k'] == k) & (df['bs'] == bs)]

#   if df_select.empty:
#     return 10

#   df_sample = df_select.sample()
#   # print(df_sample)
  
#   y = df_sample[['PERF_COUNT_HW_CACHE_L1D', 'PERF_COUNT_HW_CACHE_MISSES', 'PERF_COUNT_HW_CACHE_REFERENCES']].to_numpy()[0]
#   # print(y)
#   # print(target)
#   # print(mean_absolute_percentage_error(target, y))

#   return mean_absolute_percentage_error(target, y)

# # print(f([256, 1024, 128]))

# start_state = [0, 0, 0]

# i_values = sorted(df['i'].unique())
# k_values = sorted(df['k'].unique())
# bs_values = sorted(df['bs'].unique())

# # print(i_values, k_values, bs_values)


# class MyProblem(Annealer):
  
#   Tmax = 0.5
#   Tmin= 0.01

#   curr_step = 0
#   steps = 200

#   replay_states = []

  


#   def move(self):
#     # https://stackoverflow.com/questions/37411633/how-to-generate-a-random-normal-distribution-of-integers
#     move_amount = 10 * ((self.steps - self.curr_step) / self.steps) + 2
#     x = np.arange(-int(move_amount), int(move_amount) + 1)
#     xU, xL = x + 0.5, x - 0.5 
#     prob = ss.norm.cdf(xU, scale = move_amount) - ss.norm.cdf(xL, scale = move_amount)
#     prob = prob / prob.sum() # normalize the probabilities so their sum is 1

#     # moves = np.random.randint(-2, 3, size=(3,))
#     moves = np.random.choice(x, size = 3, p = prob)

#     poss_next_state = self.state + moves

#     while min(poss_next_state) < 0 or poss_next_state[0] >= len(i_values) or poss_next_state[1] >= len(k_values) or poss_next_state[2] >= len(bs_values):
#       moves = np.random.choice(x, size = 3, p = prob)

#       poss_next_state = self.state + moves

#     self.state = poss_next_state
#     # print(moves)

#     # self.state[0] = np.clip(self.state[0] + moves[0], a_min=0, a_max = len(i_values)-1)
#     # self.state[1] = np.clip(self.state[1] + moves[1], a_min=0, a_max = len(k_values)-1)
#     # self.state[2] = np.clip(self.state[2] + moves[2], a_min=0, a_max = len(bs_values)-1)

#     # print(self.state)
#     self.replay_states.append(self.copy_state(self.state))
#     self.curr_step += 1
#     # time.sleep(0.05)
  
#   def energy(self):
#     # print(self.state)
#     params = [i_values[self.state[0]], k_values[self.state[1]], bs_values[self.state[2]]]
#     return f(params)
  
#   def get_replay_states(self):
#     return self.replay_states

# myp = MyProblem(start_state)

# aa, bb = myp.anneal()

# print('aa:', aa)
# print('bb:', bb)

# df_best = df[(df['i'] == i_values[aa[0]]) & (df['k'] == k_values[aa[1]]) & (df['bs'] == bs_values[aa[2]])]

# print(df_best)

# replay_states = myp.get_replay_states()
# # print(replay_states)

# replay_points = []
# for replay_state in replay_states:
#   replay_point = i_values[replay_state[0]], k_values[replay_state[1]], bs_values[replay_state[2]]
#   replay_points.append(replay_point)

# replay_points = np.array(replay_points)
# # print(replay_points)

# fig, ax = plt.subplots()
# plt.tricontourf(df['i'], df['bs'], df['loss'], levels=200, vmax=1.5)
# plt.colorbar(label="Loss function")
# plt.plot(replay_points[:, 0], replay_points[:, 2], color='green')

# p = ax.scatter(replay_points[:, 0], replay_points[:, 2], c=np.arange(0, len(replay_points)), ec='w')

# ax.set_xlabel("I")
# ax.set_ylabel("Block Size")
# plt.show()

# print(min(df['loss']))

df.to_csv(os.path.join('..', 'data', 'matmul_test10-targets.csv'))