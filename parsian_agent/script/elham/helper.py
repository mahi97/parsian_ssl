import rospy
from std_msgs.msg import String

from parsian_msgs.msg import parsian_world_model, parsian_skill_gotoPointAvoid
from parsian_msgs.msg import parsian_robot_task
from parsian_msgs.msg import parsian_skill_gotoPoint
from parsian_msgs.msg import vector2D
import random


wm = parsian_world_model()

def GTPA(pub , d):
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
    setTask.base.targetPos = r.pos
    setTask.base.lookAt = d
    task.gotoPointAvoidTask = setTask
    pub.publish(task)

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
    #print(r.vel)
    if abs(((r.dir.x - d.x) * (r.pos.y - d.y) - (r.dir.y - d.y) * (r.pos.x - d.x)) - (r.pos.x - d.x) * (r.pos.y - d.y)) < 1 and abs(r.vel.x) + abs(r.vel.y) < 0.00001:
        return 1
    print(d)
    print((r.dir.x - d.x) * (r.pos.y - d.y) - (r.dir.y - d.y) * (r.pos.x - d.x))
    print((r.pos.x - d.x) * (r.pos.y - d.y))
    print("_______________")
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
    neighbor = random.randint(1 , 6)
    array[int((neighbor - 1) / 2)] = array[int((neighbor - 1) / 2)] + ((-1) ** ((neighbor % 2) + 1)) * 0.1
    #PID.write(array)
    for x in array:
        PID.write(str(x))
        PID.write(str(' '))
    #GTPA(pub)
    PID.close()
    All.close()
    x = random.randint(-2 , 2)
    y = random.randint(-2 , 2)
    d = vector2D(x , y)
    print(d)
    return d

if __name__ == "__main__":
    if 1:
        print("Right!")
    else:
        print("Helper not working")