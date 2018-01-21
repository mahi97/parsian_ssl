#!/usr/bin/ python

import rospy
from parsian_msgs.srv import *
from  plan_stuff import docWatch


class Get_Plan:
    def __init__(self):
        self.__w = docWatch.Watcher()
        self.__w.run()
        rospy.init_node('plan_server')
        rospy.Service('update_plans', parsian_update_plans, self.handle_gui_plan_request)


    def handle_gui_plan_request(self, req):
        #type: (parsian_update_plans)
        # print(req)
        return self.__w.get_all_plans()


    def handle_plan_request(selfself, req):
        #type: (plan_service)
        gameMode = req

if __name__ == '__main__':
    Get_Plan()