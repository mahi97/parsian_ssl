#!/usr/bin/env python
# license removed for brevity
import motion_profiler
import point
import rospy



def talker():
    rospy.init_node('motion_profiler', anonymous=True)
    profiler = motion_profiler.MotionProfiler(0, point.Point(-1, -2), point.Point(-4, -2),0,8,1)
    rospy.spin()

if __name__ == '__main__':
    try:
        talker()
    except rospy.ROSInterruptException:
        pass
