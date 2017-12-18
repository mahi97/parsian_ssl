#!/usr/bin/env python
# license removed for brevity
from parsian_msgs.msg import parsian_robot_task
from parsian_msgs.msg import parsian_world_model
from parsian_msgs.msg import parsian_robot_command
from dynamic_reconfigure.server import Server
from motion_profiler.cfg import motion_pro_cfg
import motion_profiler
from point import Point
import rospy


class MotionProfilerNode:
    def __init__(self):
        self.robot_id = 0

        rospy.init_node('motion_profiler', anonymous=True)
        self.wm_sub = rospy.Subscriber('world_model', parsian_world_model, self.wmCallback,
                                       queue_size=1, buff_size=2 ** 24)

        self.rbt_cmd_sub = rospy.Subscriber('robot_command' + str(self.robot_id), parsian_robot_command,
                                            self.rcCallback,
                                            queue_size=1, buff_size=2 ** 24)

        self.task_pub = rospy.Publisher('robot_task_' + str(self.robot_id), parsian_robot_task, queue_size=1,
                                        latch=True)

        self.srv = Server(motion_pro_cfg, self.cfg_callback)

        self.profiler = motion_profiler.MotionProfiler(0, Point(-1, -2), Point(-4, -2), 0, 8, 1)

        rospy.spin()

    def rcCallback(self, data):
        self.profiler.set_robot_command(data)

    def wmCallback(self, data):
        self.profiler.wmCallback(data)
        self.task_pub.publish(self.profiler.get_task())

    def cfg_callback(self, config, level):
        self.setConfig(config)

    def setConfig(self,config):
        pass


if __name__ == '__main__':
    try:
        MotionProfilerNode()
    except rospy.ROSInterruptException:
        pass
