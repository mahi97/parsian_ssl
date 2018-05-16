import rospy
from std_msgs.msg import String

from parsian_msgs.msg import parsian_world_model, parsian_skill_gotoPointAvoid
from parsian_msgs.msg import parsian_robot_task
from parsian_msgs.msg import parsian_skill_gotoPoint
from parsian_msgs.msg import vector2D
import random


wm = parsian_world_model()

def GTPA(pub):
    global wm
    if not wm.our:
        return 0
    for robot in wm.our:
        if robot.id == 0:
            r = robot
    task = parsian_robot_task()
    task.select = parsian_robot_task.GOTOPOINTAVOID
    setTask = parsian_skill_gotoPointAvoid()  # type: parsian_skill_gotoPointAvoid
    setTask.diveMode = False
    setTask.base.targetDir = vector2D(1 , 1)
    d = vector2D(1 , 1)
    setTask.base.targetPos = vector2D(0, 0)
    task.gotoPointAvoidTask = setTask
    pub.publish(task)
    return d

def NA(pub):
    task = parsian_robot_task()
    task.select = parsian_robot_task.NOTASK
    pub.publish(task)

def wmCallback(data):
    global wm
    wm = data

def done(d):
    global wm
    if not wm.our:
        return 0
    for robot in wm.our:
        if robot.id == 0:
            r = robot
    #target
    print(r.vel)
    if r.dir.x / d.x - r.dir.y / d.y < 0.1  and r.dir.x / d.x - r.dir.y / d.y > -0.1 and abs(r.vel.x) + abs(r.vel.y) < 0.00001:
        return 1
    else:
        return 0

def SA(t):
    All = open("All.txt", "w")
    with open('PID.txt') as f:
        array = []
        lastArray = []
        for line in f:
            array = lastArray = [float(x) for x in line.split()]
    with open('result.txt') as r:
        r = []
        for line in r:
           r = [int(x) for x in line.split()]
    PID = open("PID.txt", "w")
    neighbor = int(random.randint(1 , 6))
    array[int((neighbor - 1) / 2)] = array[int((neighbor - 1) / 2)] + ((-1) ** ((neighbor % 2) + 1)) * 0.1
    #PID.write(array)
    for x in array:
        PID.write(str(x))
        PID.write(str(' '))
    #GTPA(pub)
    PID.close()
    All.close()

if __name__ == "__main__":
    if 1:
        print("Right!")
    else:
        print("Helper not working")