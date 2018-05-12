import rospy
from std_msgs.msg import String

from parsian_msgs.msg import parsian_world_model, parsian_skill_gotoPointAvoid
from parsian_msgs.msg import parsian_robot_task
from parsian_msgs.msg import parsian_skill_gotoPoint
from parsian_msgs.msg import vector2D
import random

def f(x, y):
    return x * y

def GTPA(pub):
    task = parsian_robot_task()
    task.select = parsian_robot_task.GOTOPOINTAVOID
    setTask = parsian_skill_gotoPointAvoid()  # type: parsian_skill_gotoPointAvoid
    setTask.diveMode = False
    setTask.base.lookAt = vector2D(0, 0)
    setTask.base.targetPos = vector2D(1, 1)
    task.gotoPointAvoidTask = setTask
    pub.publish(task)

def SA(t):
    PID = open("PID1.txt", "w")
    with open('PID.txt') as f:
        array = []
        for line in f:
            array.append([int(x) for x in line.split()])
    with open('result.txt') as r:
        result = []
        for line in r:
           result.append([int(x) for x in line.split()])
    neighbor = int(random.randint(1 , 8))
    array[0] = array[0] + pow(-1 , (neighbor % 2) + 1)
    array[1] = array[1] + pow(-1 , (int(neighbor / 2) % 2) + 1)
    array[2] = array[2] + pow(-1 , (int(neighbor / 4) % 2) + 1)
    PID.write(str(array))
    PID.write(str(neighbor))
    PID.close()

if __name__ == "__main__":
    if 1:
        print("Right!")
    else:
        print("Helper not working")