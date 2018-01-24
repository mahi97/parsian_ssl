#!/usr/bin/env python2.7.12

from matplotlib import pyplot as plt
import numpy as np
import os
import csv

# set directory for profile files
# datas : a list of dictionaries, each for a robot profile
datas = []

# files_list : a list of file paths
files_list = [file_token for file_token in os.listdir('.') if file_token.endswith('kick.profile')]
file_count = len(files_list)

# filling datas
for i in range(12):
    for f_t in files_list:
        if f_t.startswith(str(i)):
            temp_file = open(f_t)
            temp_prof = eval(temp_file.read())
            datas.append(temp_prof)
            temp_file.close()

# input speeds
input_speeds = np.arange(200, 601, 100)
##input_speeds.append(1023)

# getting the raw outputs
all_real_raw_output_speeds = []
for i in range(file_count):
    for speed_key, speed_list in datas[i].iteritems():
        for speed in speed_list:
            if (speed - np.mean(speed_list)) > np.std(speed_list):
                datas[i][speed_key].remove(speed)

    real_raw_out = [(sum(datas[i][x])/len(datas[i][x])) for x in input_speeds]
    all_real_raw_output_speeds.append(real_raw_out)

# debuging input parsing
print(input_speeds)
print(all_real_raw_output_speeds)

all_coeffs = []
for out_speed in all_real_raw_output_speeds:
    p = np.polyfit(out_speed, input_speeds, 2)
    all_coeffs.append(p)

# debug polyfit coefficients
print(all_coeffs)

x = np.linspace(0,10,100)
y = all_coeffs[0][0]*(x**2) + all_coeffs[0][1]*x + all_coeffs[0][2]

# plt.plot(output_speeds, input_speeds, 'o', x, y, 'r--')
plt.plot(x, y, 'r--',)

plt.xlabel('Real Speed M/S')
plt.ylabel('Cap Charge')
plt.grid(True)

# saving coeffs to csv file
with open('coeffs_kick.csv', 'wb') as csv_file:
    my_writer = csv.writer(csv_file)
    my_writer.writerow(['robotID', 'a', 'b', 'c'])
    for i in range(12):

        my_writer.writerow([str(i)] + list(all_coeffs[i]))

plt.show()
