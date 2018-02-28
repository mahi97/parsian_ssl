#!/usr/bin/env python
# license removed for brevity

import rospy

class BehaviorServer:
    def __init__(self):
        rospy.init_node('behavior_server')
        rospy.loginfo('behavior server inited')
        rospy.spin()

if __name__ == '__main__':
    bs = BehaviorServer()
    rospy.spin()
