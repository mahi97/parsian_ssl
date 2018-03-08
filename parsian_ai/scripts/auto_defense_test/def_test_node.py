from enum import Enum
import rospy
from parsian_msgs.msg import parsian_world_model
from parsian_msgs.msg import parsian_robot_task
from plans.pass_shoot_plan import *
from point import Point

## kian bia ye GUI barash benevisim


##------------------------------States------------------------------------##
class Attack_Plan(Enum):
    No_Plan = 0,
    Pass_Shoot_Plan = 1  


class Pass_Shoot_State(Enum):
	A = 1





##------------------------------Main Node------------------------------------##
class AutoDefenseTest():
    
    def __init__(self):

        self.wm = parsian_world_model
        rospy.init_node('def_test', anonymous=True)
	self.wm_sub = rospy.Subscriber('/world_model', parsian_world_model, self.wm_cb, queue_size=1, buff_size=2 ** 24)
        self.attack_state = Attack_Plan.No_Attack
        self.srv = Server(def_testConfig, self.cfg_callback)

    def wm_cb(self, wm):

	self.wm = wm

        if (self.attack_state == Attack_Plan.No_Plan):
            #halt
            pass


        if (self.attack_state == Attack_Plan.Pass_Shoot_plan):
            if (pass_shoot_plan(self.wm)):
                self.STATE = STATE.Get_User_Plan


    #-------------------------Plan Functions------------------------------------##
        
    def Pass_Shoot_Plan():
        pass


