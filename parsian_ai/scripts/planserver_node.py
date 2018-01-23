#!/usr/bin/env python
# license removed for brevity

import rospy
from parsian_msgs.srv import *
import docWatch


class Get_Plan:
    def __init__(self):

        rospy.init_node('plan_server')
        self.s = rospy.Service('update_plans', parsian_update_plans, self.handle_gui_plan_request)
        self.__w = docWatch.Watcher()
        self.__w.run()
        rospy.spin()


    def handle_gui_plan_request(self, req):
        #type: (parsian_update_plansRequest)
        print req
        received = req.newPlans
        if len(received) > 0:
            self.__w.update_master_active(received, req.isMaster, req.isActive)
            print ("response to gui......")
            return self.__w.get_all_plans()


    def handle_plan_request(self, req):
        #type: (plan_service)
        return self.__w.choose_plan(req.request.playersNum, req.request.gameMode)

if __name__ == '__main__':
    Get_Plan()
