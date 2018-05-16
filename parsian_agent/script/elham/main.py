#!/usr/bin/env python
from __future__ import print_function
import rospy
from std_msgs.msg import String
import math
from helper import *
from parsian_msgs.msg import parsian_world_model

t = 0

if __name__ == "__main__":
    try:
        rospy.init_node('GTPA', anonymous=True)
        rate = rospy.Rate(10)  # 10hz
        pub = rospy.Publisher('agent_' + str(0) + '/task', parsian_robot_task, queue_size=10)  # !!!!!!!!!!!!!!!
        wm_sub = rospy.Subscriber('/world_model', parsian_world_model, wmCallback, queue_size=1, buff_size=2)

        while not rospy.is_shutdown():
            t = GTPA(pub)
            if done(t):
                NA(pub)
                SA(6)

        #!*!zaviye dorost bod
        #t = t + 1
        rate.sleep()
        #rospy.spin()
    except rospy.ROSInterruptException:
        pass
