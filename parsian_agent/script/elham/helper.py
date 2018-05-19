import rospy
from std_msgs.msg import String

from parsian_msgs.msg import parsian_world_model, parsian_skill_gotoPointAvoid, parsian_robot
from parsian_msgs.msg import parsian_robot_task
from parsian_msgs.msg import parsian_skill_gotoPoint
from parsian_msgs.msg import vector2D
import random
import math

wm = parsian_world_model()
d = vector2D(1 , 1)

def GTPA(pub):
    global wm , d
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
    setTask.base.lookAt = vector2D(5000 , 5000)
    setTask.base.targetDir = vector2D(1 , 1)
    task.gotoPointAvoidTask = setTask
    pub.publish(task)

def NA(pub):
    task = parsian_robot_task()
    task.select = parsian_robot_task.NOTASK
    pub.publish(task)

def wmCallback(data):
    global wm
    wm = data

def done():
    global wm , d
    if not wm.our:
        return 0
    for robot in wm.our:
        if robot.id == 0:
            r = robot

    #target
    #print(r.vel)
    print(d)
    print(r.angularVel)
    print(r.dir.x)
    print(r.dir.y)
    print(r.dir.x/d.x)
    print(r.dir.y/d.y)
    print("_______________")
    if abs(r.dir.x / d.x) < 0.001 and abs(r.dir.y / d.y) < 0.001 and abs(r.vel.x) + abs(r.vel.y) + abs(r.angularVel) < 0.0001:
        return 1
    return 0

newResult = 0

def SA(pub , t , lastResult):
    global newResult
    if not done():
        GTPA(pub)
        newResult += 1
    else:
        with open('PID.txt') as f:
            array = []
            for line in f:
                array = [float(x) for x in line.split()]
        PID = open("PID.txt", "w")
        neighbor = random.randint(1, 6)
        array[int((neighbor - 1) / 2)] = array[int((neighbor - 1) / 2)] + ((-1) ** ((neighbor % 2) + 1)) * 0.1
        # PID.write(array)
        for x in array:
            PID.write(str(x))
            PID.write(str(' '))
        # GTPA(pub)
        PID.close()
        global d
        if d == vector2D(1, 1):
            d = vector2D(1, -1)
        elif d == vector2D(1, -1):
            d = vector2D(-1, -1)
        elif d == vector2D(-1, -1):
            d = vector2D(-1, 1)
        elif d == vector2D(-1, 1):
            d = vector2D(1, 1)

        NA(pub)
        if newResult < lastResult:
            return newResult
        rand = random.randint(0 , 1)
        #if(newResult - lastResult) < rand:
        #    return newResult
        array[int((neighbor - 1) / 2)] = array[int((neighbor - 1) / 2)] - ((-1) ** ((neighbor % 2) + 1)) * 0.1
        # PID.write(array)
        PID = open("PID.txt", "w")
        for x in array:
            PID.write(str(x))
            PID.write(str(' '))
        PID.close()
        return lastResult

if __name__ == "__main__":
    if 1:
        print("Right!")
    else:
        print("Helper not working")