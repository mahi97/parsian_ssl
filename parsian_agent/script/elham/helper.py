import rospy
from std_msgs.msg import String

from parsian_msgs.msg import parsian_world_model, parsian_skill_gotoPointAvoid
from parsian_msgs.msg import parsian_robot_task
from parsian_msgs.msg import parsian_skill_gotoPoint
from parsian_msgs.msg import vector2D
import random

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
    All = open("All.txt", "w")
    with open('PID.txt') as f:
        array = []
        for line in f:
            array = [int(x) for x in line.split()]
    with open('result.txt') as r:
        r = []
        for line in r:
           r = [int(x) for x in line.split()]
    PID = open("PID.txt", "w")
    neighbor = int(random.randint(1 , 6))
    array[int(neighbor / 2)] = array[int(neighbor / 2)] + (-1) ** ((neighbor % 2) + 1)
    PID.write(array)
    for x in array:
        PID.write(str(x))
        PID.write(str(' '))
    #PID.write(str(array))
    PID.close()
    All.close()

if __name__ == "__main__":
    if 1:
        print("Right!")
    else:
        print("Helper not working")