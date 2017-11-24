#!/usr/bin/env python
# license removed for brevity
import motion_profiler
import point
import rospy



def talker():
    rospy.init_node('motion_profiler', anonymous=True)
    profiler = motion_profiler.MotionProfiler(1, point.Point(0, 0), point.Point(4, 3))
    rospy.spin()

if __name__ == '__main__':
    try:
        talker()


    except rospy.ROSInterruptException:
        pass
