#!/usr/bin/env python
# license removed for brevity

import rospy
from parsian_msgs.msg import parsian_robot_task
from parsian_msgs.msg import parsian_world_model
from parsian_msgs.msg import parsian_robot_command
from parsian_msgs.msg import parsian_skill_gotoPointAvoid
from parsian_msgs.msg import parsian_skill_kick


def wmCallback(data):{}
    #rospy.loginfo("wmcallback is runn")

def rcCallback(data):{}
    #rospy.loginfo("rccallback is runn")

def talker():
    rospy.init_node('kick_profiler', anonymous=True)
    rospy.loginfo("kick_profiler is running")

    robot_id = 0
    wm_sub = rospy.Subscriber('world_model', parsian_world_model, wmCallback, queue_size=1,
                                     buff_size=2 ** 24)
    rbt_cmd_sub = rospy.Subscriber('robot_command' + str(robot_id), parsian_robot_command, rcCallback,
                                          queue_size=1, buff_size=2 ** 24)
    task_pub = rospy.Publisher('robot_task_' + str(robot_id), parsian_robot_task, queue_size=1,
                                      latch=True)

    current_task = parsian_robot_task()
    current_task.select = parsian_robot_task.GOTOPOINTAVOID
    task = parsian_skill_gotoPointAvoid()
    task.noAvoid = False
    # task.base.robot_id = robot_id
    task.base.lookAt.x = 5000
    task.base.lookAt.y = 5000
    task.base.maxVelocity = 1.5
    task.base.targetPos.x = 4
    task.base.targetPos.y = 1
    task.base.targetDir.x = 2
    task.base.targetDir.y = 1
    current_task.gotoPointAvoidTask = task
    while True:
        task_pub.publish(current_task)


    rospy.spin()

if __name__ == '__main__':
    try:
        talker()
    except rospy.ROSInterruptException:
        pass
