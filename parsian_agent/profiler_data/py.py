#!/usr/bin/env python3.5
import matplotlib.pyplot as plt
from matplotlib.pyplot import setp
import pprint
import numpy as np
import os

command = []
world_model = []
remain_dist = []
time =[]
with open("motion_profiler.profile") as file :
    f = eval(file.read())
    for (key,value) in f:
        for point in f[(key,value)]['data']:
            command.append(point['robot_command'])
            world_model.append(point['world_model'])
            remain_dist.append(point['remain_dist'])
            time.append(point['time'])
        plt.savefig(str((key,value))+'.pdf')
        fig = plt.figure(1)
        ax = plt.axes([0.1, 0.1, .8, .8])
        setp(ax, 'frame_on', False)

        #plt.plot(time, world_model, 'ro')
        #plt.plot(time, world_model, 'k--')

#        plt.plot(time, command, 'ro')
 #       plt.plot(time, command, 'k--')

        plt.plot(time, command, 'ro')
        plt.plot(time, command, 'k--')

        plt.grid(True)

        ax.set_xlabel(r'time')
        ax.set_ylabel(r'world_model ')
        plt.savefig(os.path.join(os.getcwd(), str((key,value))+'.pdf'))

