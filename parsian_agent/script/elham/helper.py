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
newResult = 0
t = 0
sumResult = 0

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
    setTask.base.targetDir = d
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
    #print(d)
    #print(r.angularVel)
    #print(r.dir.x)
    #print(d.y)
    x = r.dir.x * (2 / (r.dir.x ** 2 + r.dir.y ** 2)) ** (1/2.0)
    y = r.dir.y * (2 / (r.dir.x ** 2 + r.dir.y ** 2)) ** (1/2.0)
    #print(x)
    #print(y)
    #print("_______________")
    if abs(x - d.x) < 0.2 and abs(y - d.y) < 0.2 and abs(r.vel.x) + abs(r.vel.y) + abs(r.angularVel) < 0.01:
        return 1
    return 0

def SA(pub , lastResult):
    global newResult , t , sumResult
    if not done():
        GTPA(pub)
        if newResult < 100000:
            newResult += 1
            return lastResult
    t += 1
    NA(pub)
    with open('PID.txt') as f:
        array = []
        for line in f:
            array = [float(x) for x in line.split()]
    PID = open("PID.txt", "w")
    neighbor = random.randint(1, 6)
    array[int((neighbor - 1) / 2)] = array[int((neighbor - 1) / 2)] + ((-1) ** ((neighbor % 2) + 1)) * 0.001
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
    sumResult += newResult
    if d == vector2D(1 , 1):
        print("_______________")
        print(array)
        print(sumResult)
        print(lastResult)
        if sumResult < lastResult:
            lastResult = sumResult
            sumResult = 0
            return lastResult
        print("bad bod")
        rand = random.uniform(0 , 1)
        print(rand)
        print((sumResult - lastResult) / (math.e ** t))
        if (sumResult - lastResult) / (math.e ** t) > rand:
            lastResult = sumResult
            newResult = 0
            sumResult = 0
            print("ehtemal")
            return lastResult
        array[int((neighbor - 1) / 2)] = array[int((neighbor - 1) / 2)] - ((-1) ** ((neighbor % 2) + 1)) * 0.001
        # PID.write(array)
        PID = open("PID.txt", "w")
        for x in array:
            PID.write(str(x))
            PID.write(str(' '))
        PID.close()
        newResult = 0
        sumResult = 0
    return lastResult

if __name__ == "__main__":
    if 1:
        print("Right!")
    else:
        print("Helper not working")