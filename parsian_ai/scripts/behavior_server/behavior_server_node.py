#!/usr/bin/env python
# license removed for brevity
from parsian_msgs.msg import parsian_behavior
from parsian_msgs.msg import parsian_ai_status
import best_selector

import rospy
from parsian_ai.cfg import
class BehaviorServer:
    def __init__(self):
        rospy.init_node('behavior_server')
        self.selector = best_selector.BestSelector()
        self.srv = Server(, self.cfg_callback)
        self.eval_sub = rospy.Subscriber('/eval', parsian_behavior, self.getEvals, queue_size=1, buff_size=2 ** 24)
        self.ai_status_sub = rospy.Subscriber('/ai_status', parsian_ai_status, self.correctProbillty, queue_size=1,
                                              buff_size=2 ** 24)
        self.selected_pub = rospy.Publisher('/behavior', parsian_behavior, queue_size=1, latch=True)
        self.timer = rospy.Timer(rospy.Duration(1), self.publisherCallBack, oneshot=False)
        rospy.loginfo('behavior server inited')
        rospy.spin()

    def getEvals(self, msg):
        self.selector.update_data(msg)
        rospy.loginfo("running")

    def publisherCallBack(self, event):
        best = self.selector.get_best()
        if best is not -1:
            self.selected_pub.publish(best)

    def correctProbillty(self, msg):
        self.selector.update_success_rate(msg)

if __name__ == '__main__':
    bs = BehaviorServer()
    rospy.spin()
