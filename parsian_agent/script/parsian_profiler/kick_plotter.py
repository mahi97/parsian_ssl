#!/usr/bin/env python2.7

from matplotlib import pyplot as plt
import numpy as np
import os
import csv

# set directory for profile files
# datas : a list of dictionaries, each for a robot profile
datas = []
input_speeds = []
all_real_raw_output_speeds = []
all_coeffs = []

# files_list : a list of file paths
files_list = [file_token for file_token in os.listdir('.') if file_token.endswith('kick(nospin).profile')]
file_count = len(files_list)

for i in range (12):
    datas.append(-1)
    input_speeds.append(-1)
    all_real_raw_output_speeds.append([])
    all_coeffs.append(-1)


# filling datas
for i in range(12):
	for f_t in files_list:
		if f_t.startswith(str(i)):
			temp_file = open(f_t)
			temp_prof = eval(temp_file.read())
			##datas.append(temp_prof)
			datas[i] = temp_prof
			temp_file.close()

# input speeds
for i in range(12):
    if datas[i] != -1:
        temp = list(datas[i].keys())
        temp.sort()
        input_speeds[i] = temp

# getting the raw outputs

for i in range(12):
    if datas[i] != -1:
        real_raw_out = [(sum(datas[i][x])/len(datas[i][x])) for x in input_speeds[i]]
        all_real_raw_output_speeds[i] = real_raw_out

# debuging input parsing
print(input_speeds)
print(all_real_raw_output_speeds)

for i in range(12):
	if datas[i] != -1:
		if all_real_raw_output_speeds[i] != []:
			p = np.polyfit(all_real_raw_output_speeds[i], input_speeds[i], 2)
	    	all_coeffs[i] = p


#for out_speed, in_speed in all_real_raw_output_speeds, input_:
#    p = np.polyfit(out_speed, input_speeds, 2)
#    all_coeffs.append(p)

# debug polyfit coefficients
print(all_coeffs)

x = np.linspace(0,10,100)
y = all_coeffs[7][0]*(x**2) + all_coeffs[7][1]*x + all_coeffs[7][2]

# plt.plot(output_speeds, input_speeds, 'o', x, y, 'r--')
plt.plot(x, y, 'r--')
plt.plot(all_real_raw_output_speeds[7], input_speeds[7], 'o')

plt.xlabel('Real Speed M/S')
plt.ylabel('Cap Charge')
plt.grid(True)

# saving coeffs to csv file
with open('coeffs_kick.csv', 'wb') as csv_file:
    my_writer = csv.writer(csv_file)
    my_writer.writerow(['robotID', 'a', 'b', 'c'])
    for i in range(12):
        if datas[i] != -1:
            my_writer.writerow([str(i)] + list(all_coeffs[i]))

plt.show()
