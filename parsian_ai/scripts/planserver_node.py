#!/usr/bin/env python
# license removed for brevity

import rospy
from parsian_msgs.srv import *
import docWatch


class Get_Plan:
    def __init__(self):
        self.__w = docWatch.Watcher()
        self.__w.run()
        rospy.init_node('plan_server')
        rospy.Service('update_plans', parsian_update_plans, self.handle_gui_plan_request)


    def handle_gui_plan_request(self, req):
        #type: (parsian_update_plans)

        received = req.request.newPlan
        if len(received) > 0:
            self.__w.update_master_active(received, req.request.isMaster, req.request.isActive)
        return self.__w.get_all_plans_msgs()


    def handle_plan_request(self, req):
        #type: (plan_service)
        return self.__w.choose_plan(req.request.playersNum, req.request.gameMode)

if __name__ == '__main__':
    Get_Plan()