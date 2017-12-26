#!/usr/bin/env python3.5
import matplotlib.pyplot as plt
from math import pi
import os

command = []
world_model = []
remain_dist = []
time =[]
with open("profiler_data/motion_profiler.profile") as file:
    f = eval(file.read())
    for (dist, phi, khab) in f:
        for point in f[(dist, phi, khab)]['data']:
            command.append(point['robot_command'])
            world_model.append(point['world_model'])
            remain_dist.append(point['remain_dist'])
            time.append(point['time'])
        plt.figure()
        plt.subplot(1, 1, 1)
        plt.xlabel(r'time')
        plt.title(str(round(phi/pi*180))+"  "+khab)
        wm = plt.plot(time, world_model, 'ro',color='blue', label="world_model vel")
        plt.plot(time, world_model, 'k--')
        com = plt.plot(time, command, 'ro',color='red', label="command vel")
        plt.plot(time, command, 'k--')
        rem = plt.plot(time, remain_dist, 'ro',color='green', label="remain dist")
        plt.plot(time, remain_dist, 'k--')

        plt.legend(bbox_to_anchor=(1, 1),
                   bbox_transform=plt.gcf().transFigure)
        plt.grid(True)
        plt.savefig(os.path.join(os.getcwd(), str((dist,phi,khab))+'.pdf'))
        plt.clf()
        time.clear()
        remain_dist.clear()
        command.clear()
        world_model.clear()