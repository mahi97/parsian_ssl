#!/usr/bin/env python
# license removed for brevity

import rospy
import point
import math
import rospkg
from enum import Enum
from parsian_msgs.msg import parsian_robot_task
from parsian_msgs.msg import vector2D
from parsian_msgs.msg import parsian_robot_command
from parsian_msgs.msg import parsian_skill_gotoPointAvoid
from parsian_msgs.msg import parsian_skill_no
from parsian_msgs.msg import parsian_world_model
from parsian_msgs.msg import parsian_robot
from parsian_msgs.msg import parsian_skill_receivePass
from parsian_msgs.msg import parsian_skill_kick
from Plans.pass_shoot_plan import Pass_Shoot_Plan



##------------------------------States------------------------------------##
class Attack_Plan(Enum):
    Pass_Shoot_Plan = 0






##------------------------------Main Node------------------------------------##
class AutoDefenseTest():
    
    def __init__(self):

        self.wm = parsian_world_model
        self.wm_sub = rospy.Subscriber('/world_model', parsian_world_model, self.wm_cb, queue_size=1, buff_size=2 ** 24)
        self.ready_to_run = True ##should br false and assign true in cfg
        self.plan = Attack_Plan.Pass_Shoot_Plan

        self.pass_shoot_plan = Pass_Shoot_Plan()


    def wm_cb(self, wm):
        self.wm = wm

        if self.ready_to_run:


            if (self.plan == Attack_Plan.Pass_Shoot_Plan):
                if (self.pass_shoot_plan.execute(self.wm)):
                    self.ready_to_run = False




if __name__ == '__main__':
    try:
        rospy.init_node('def_test', anonymous=True)
        rospy.loginfo("def_test_node is running")
        def_test = AutoDefenseTest()
        rospy.spin()
    except rospy.ROSInterruptException:
        pass

