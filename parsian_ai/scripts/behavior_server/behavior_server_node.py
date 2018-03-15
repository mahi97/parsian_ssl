#!/usr/bin/env python
# license removed for brevity
from parsian_msgs.msg import parsian_behavior
from parsian_msgs.msg import parsian_ai_status
import best_selector

import rospy


class TempMsg:
    def __init__(self):
        self.name = ""
        self.point = 0


class BehaviorServer:
    def __init__(self):
        rospy.init_node('behavior_server')
        self.selector = best_selector.BestSelector()
        self.eval_sub = rospy.Subscriber('/eval', parsian_behavior, self.getEvals, queue_size=1, buff_size=2 ** 24)
        self.ai_status_sub = rospy.Subscriber('/ai_status', parsian_ai_status, self.correctProbillty, queue_size=1,
                                              buff_size=2 ** 24)
        self.selected_pub = rospy.Publisher('/behavior', parsian_behavior, queue_size=1, latch=True)
<<<<<<< 49390fd93b02ba59fed888ef9127f64178ae96d9
<<<<<<< f4daa6829b44da76de41398c944284af19a34ee9
        self.timer = rospy.Timer(rospy.Duration(1), self.publisherCallBack, oneshot=False)
=======
        self.timer = rospy.Timer(rospy.Duration(10), self.publisherCallBack, oneshot=False)
>>>>>>> add timer
=======
        self.timer = rospy.Timer(rospy.Duration(1), self.publisherCallBack, oneshot=False)
>>>>>>> fix runtime errors
        rospy.loginfo('behavior server inited')
        rospy.spin()

    def getEvals(self, msg):
        self.selector.update_data(msg)
        rospy.loginfo("running")

    def publisherCallBack(self, event):
<<<<<<< 49390fd93b02ba59fed888ef9127f64178ae96d9
<<<<<<< f4daa6829b44da76de41398c944284af19a34ee9
        best = self.selector.get_best()
        if best is not -1:
            self.selected_pub.publish(best)
=======
        self.selected_pub.publish(self.selector.get_best())
>>>>>>> add timer
=======
        best = self.selector.get_best()
        if best is not -1:
            self.selected_pub.publish(best)
>>>>>>> fix runtime errors

    def correctProbillty(self, msg):
        self.selector.update_success_rate(msg)

if __name__ == '__main__':
    bs = BehaviorServer()
    rospy.spin()
