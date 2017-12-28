#!/usr/bin/env python3.5
import matplotlib.pyplot as plt
from math import pi
from sklearn import linear_model
import os
X_values = []
Y_values = []
r = linear_model.LassoCV()
with open("../../profiler_data/motion_profiler.profile") as file:
    f = eval(file.read())
    for (dist, phi, khab) in f:
        for point in f[(dist, phi, khab)]['data']:
            X_values.append([point['remain_dist'], point['world_model'],phi])
            Y_values.append(f[(dist, phi, khab)]['total_time'] - point['time'])

r.fit(X_values, Y_values)
for i in range(0, 4):
    for j in range(0, 4):
        for k in range(0, 8):
            print(r.predict([[i, j, k*45]]))



