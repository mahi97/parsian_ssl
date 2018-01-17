#!/usr/bin/env python
# license removed for brevity

import rospy
import point
import math
from enum import Enum
from parsian_msgs.msg import parsian_robot_task
from parsian_msgs.msg import vector2D
from parsian_msgs.msg import parsian_robot_command
from parsian_msgs.msg import parsian_skill_gotoPointAvoid
from parsian_msgs.msg import parsian_world_model
from parsian_msgs.msg import parsian_robot
from parsian_msgs.msg import parsian_skill_receivePass
from parsian_msgs.msg import parsian_skill_kick

#####PROBLEMS#####
#1)fix the that annoying number in kick skill
#2)robots dont avoid the ball in gotopointavoids :/
#3)better condition for receiving, setkicker
#4)create a standby state

class State(Enum):
    GOTOPBOTH = 1
    GOFORKICK = 2
    KICK = 3
    RECEIVE = 4
    RETREAT = 5
    STANDBY = 6
    CALCULATING = 7

class KickStat(Enum):
    ROBOT1KICKING = 1
    ROBOT2KICKING = 2
    NONE = 3
    ROBOT1RETREATING = 4
    ROBOT2RETREATING = 5


class ReceiveStat(Enum):
    ROBOT1RECING = 1
    ROBOT2RECING = 2
    NONE = 3

class KickProfiler():
    def __init__(self):
        self.m_wm = parsian_world_model()
        self.startingpoint1 = point.Point(-1.5, 0)
        self.startingpoint2 = point.Point(1.5, 0)
        self.my_robot1 = parsian_robot()
        self.my_robot2 = parsian_robot()
        self.state = State.GOTOPBOTH
        self.kickstat = KickStat.NONE
        self.receivestat = ReceiveStat.NONE
        self.neaarertoballid = -1
        self.velrecorder = []
        self.velrecflag = False
        self.wm_sub = rospy.Subscriber('world_model', parsian_world_model, self.wmCallback, queue_size=1,
                                  buff_size=2 ** 24)
        # self.rbt_cmd_sub = rospy.Subscriber('robot_command' + str(self.robot_id), parsian_robot_command, self.rcCallback,
        #                                queue_size=1, buff_size=2 ** 24)






    def wmCallback(self, data):
        # type:(parsian_world_model) ->object
        self.m_wm = data
        self.getrobots(0, 1)
        self.updatenearballid()
        #rospy.loginfo(self.state)
        if self.velrecflag:
            self.velrecorder.append(math.hypot(data.ball.vel.x, data.ball.vel.y))
        if self.state == State.GOTOPBOTH:
            if self.gotopoint(1, self.startingpoint1, self.setdirtorobot(2)) and self.gotopoint(2, self.startingpoint2, self.setdirtorobot(1)):
                self.state = State.GOFORKICK
        if self.state == State.GOFORKICK:
            if self.setkickerpos():
                self.state = State.KICK
        if self.state == State.KICK:
            if self.kick():
                self.state = State.RETREAT
        if self.state == State.RETREAT:
            self.retreat()
            self.state = State.RECEIVE
        if self.state == State.RECEIVE:
            if self.receive():
                self.state = State.CALCULATING
        if self.state == State.CALCULATING:
            if self.calculate():
                self.state = State.GOFORKICK





    # def rcCallback(self, data):
    #     # type: (parsian_robot_command) -> object
    #     pass

    #get the robots from world model
    def getrobots(self, id1, id2):
        # type: (int, int) -> object
        for robot in self.m_wm.our:
            if robot.id == id1:
                self.my_robot1 = robot
            if robot.id == id2:
                self.my_robot2 = robot

        self.task_pub1 = rospy.Publisher('agent_' + str(self.my_robot1.id) + str('/task'), parsian_robot_task, queue_size=1,
                                                 latch=True)
        self.task_pub2 = rospy.Publisher('agent_' + str(self.my_robot2.id) + str('/task'), parsian_robot_task, queue_size=1,
                                                 latch=True)




    def robotarrived(self, robot, target, dist = 0.1):
        # type:(parsian_robot, point.Point) ->object
        if math.hypot(robot.pos.x - target.x, robot.pos.y - target.y) < dist:
            return True
        else:
            return False




    #send robot(id) to the point
    def gotopoint(self, id, point, dirpoint = point.Point(0, 1)):
        # type:(int, point.Point, point.Point) ->object

        if id == 1:
            current_task1 = parsian_robot_task()
            current_task1.select = parsian_robot_task.GOTOPOINTAVOID
            task1 = parsian_skill_gotoPointAvoid()
            task1.noAvoid = False
            task1.base.lookAt.x = 5000
            task1.base.lookAt.y = 5000
            task1.base.maxVelocity = 1.5
            task1.base.targetPos.x = point.x
            task1.base.targetPos.y = point.y
            task1.base.targetDir.x = dirpoint.x #self.my_robot2.pos.x - self.my_robot1.pos.x
            task1.base.targetDir.y = dirpoint.y #self.my_robot2.pos.y - self.my_robot1.pos.y
            current_task1.gotoPointAvoidTask = task1
            self.task_pub1.publish(current_task1)
            if self.robotarrived(self.my_robot1, point):
                return  True
            else:
                return False


        if id == 2:
            current_task2 = parsian_robot_task()
            current_task2.select = parsian_robot_task.GOTOPOINTAVOID
            task2 = parsian_skill_gotoPointAvoid()
            task2.noAvoid = False
            task2.base.lookAt.x = 5000
            task2.base.lookAt.y = 5000
            task2.base.maxVelocity = 1.5
            task2.base.targetPos.x = point.x
            task2.base.targetPos.y = point.y
            task2.base.targetDir.x = dirpoint.x #self.my_robot1.pos.x - self.my_robot2.pos.x
            task2.base.targetDir.y = dirpoint.y #self.my_robot1.pos.y - self.my_robot2.pos.y
            current_task2.gotoPointAvoidTask = task2
            self.task_pub2.publish(current_task2)
            if self.robotarrived(self.my_robot2, point):
                return  True
            else:
                return False


    def setdir(self, myxpos, myypos, tarxpos, tarypos):
        # type:(float, float, float, float) ->point.Point
         return point.Point(tarxpos - myxpos, tarypos - myypos)


    def setdirtorobot(self, targetid):
        # type:(int) ->point.Point
        if targetid == 1:
            return point.Point(self.my_robot1.pos.x - self.my_robot2.pos.x, self.my_robot1.pos.y - self.my_robot2.pos.y)
        if targetid == 2:
            return point.Point(self.my_robot2.pos.x - self.my_robot1.pos.x, self.my_robot2.pos.y - self.my_robot1.pos.y)




    def kick(self):
        if self.kickstat == KickStat.ROBOT1KICKING:
            self.velrecflag = True
            current_task1 = parsian_robot_task()
            current_task1.select = parsian_robot_task.KICK
            task1 = parsian_skill_kick()
            task1.chip = False
            task1.kickSpeed = 5
            task1.target.x = self.my_robot2.pos.x
            task1.target.y = self.my_robot2.pos.y
            current_task1.kickTask = task1
            self.task_pub1.publish(current_task1)
            if math.hypot(self.m_wm.ball.vel.x, self.m_wm.ball.vel.y) > 0.02 and math.hypot(self.m_wm.ball.pos.x - self.my_robot1.pos.x, self.m_wm.ball.pos.y - self.my_robot1.pos.y) > 0.2:
                self.kickstat = KickStat.ROBOT1RETREATING
                return True
            else:
                return False


        if self.kickstat == KickStat.ROBOT2KICKING:
            self.velrecflag = True
            current_task2 = parsian_robot_task()
            current_task2.select = parsian_robot_task.KICK
            task2 = parsian_skill_kick()
            task2.chip = False
            task2.kickSpeed = 5
            task2.target.x = self.my_robot1.pos.x
            task2.target.y = self.my_robot1.pos.y
            current_task2.kickTask = task2
            self.task_pub2.publish(current_task2)
            if math.hypot(self.m_wm.ball.vel.x, self.m_wm.ball.vel.y) > 0.02 and math.hypot(self.m_wm.ball.pos.x - self.my_robot2.pos.x, self.m_wm.ball.pos.y - self.my_robot2.pos.y) > 0.2:
                self.kickstat = KickStat.ROBOT2RETREATING
                return True
            else:
                return False




    def retreat(self):
        if self.kickstat == KickStat.ROBOT1RETREATING:
            self.gotopoint(1, self.startingpoint1, self.setdirtorobot(2))
            self.receivestat = ReceiveStat.ROBOT2RECING
        if self.kickstat == KickStat.ROBOT2RETREATING:
            self.gotopoint(2, self.startingpoint2, self.setdirtorobot(1))
            self.receivestat = ReceiveStat.ROBOT1RECING



    def receive(self):
        if self.receivestat == ReceiveStat.ROBOT1RECING:
            current_task1 = parsian_robot_task()
            current_task1.select = parsian_robot_task.RECIVEPASS
            task1 = parsian_skill_receivePass()
            task1.receiveRadius = 0.5
            task1.target.x = self.startingpoint1.x
            task1.target.y = self.startingpoint1.y
            current_task1.receivePassTask = task1
            self.task_pub1.publish(current_task1)
            if math.hypot(self.m_wm.ball.vel.x, self.m_wm.ball.vel.y) < 0.02 and math.hypot(self.m_wm.ball.pos.x - self.my_robot1.pos.x, self.m_wm.ball.pos.y  - self.my_robot1.pos.y) < 0.5:
                return True
            if math.hypot(self.m_wm.ball.vel.x, self.m_wm.ball.vel.y) < 0.02 and math.hypot(self.m_wm.ball.pos.x - self.my_robot1.pos.x, self.m_wm.ball.pos.y  - self.my_robot1.pos.y) >= 0.5:
                return True
            else:
                return False

        if self.receivestat == ReceiveStat.ROBOT2RECING:
            current_task2 = parsian_robot_task()
            current_task2.select = parsian_robot_task.RECIVEPASS
            task2 = parsian_skill_receivePass()
            task2.receiveRadius = 0.5
            task2.target.x = self.startingpoint2.x
            task2.target.y = self.startingpoint2.y
            current_task2.receivePassTask = task2
            self.task_pub2.publish(current_task2)
            if math.hypot(self.m_wm.ball.vel.x, self.m_wm.ball.vel.y) < 0.02 and math.hypot(self.m_wm.ball.pos.x - self.my_robot2.pos.x, self.m_wm.ball.pos.y  - self.my_robot2.pos.y) < 0.5:
                return True
            if math.hypot(self.m_wm.ball.vel.x, self.m_wm.ball.vel.y) < 0.02 and math.hypot(self.m_wm.ball.pos.x - self.my_robot2.pos.x, self.m_wm.ball.pos.y  - self.my_robot2.pos.y) >= 0.5:
                return True
            else:
                return False



    def updatenearballid(self):
        robot1_ball = math.hypot(self.my_robot1.pos.x - self.m_wm.ball.pos.x, self.my_robot1.pos.y - self.m_wm.ball.pos.y)
        robot2_ball = math.hypot(self.my_robot2.pos.x - self.m_wm.ball.pos.x, self.my_robot2.pos.y - self.m_wm.ball.pos.y)
        if abs(robot1_ball - robot2_ball) < 0.05:
            self.neaarertoballid = 1
        if robot1_ball > robot2_ball:
            self.neaarertoballid = 2
        else:
            self.neaarertoballid = 1

    def setkickerpos(self):
        if self.neaarertoballid == 1:
            current_task1 = parsian_robot_task()
            current_task1.select = parsian_robot_task.KICK
            task1 = parsian_skill_kick()
            task1.dontKick = True
            task1.chip = False
            task1.target.x = self.my_robot2.pos.x
            task1.target.y = self.my_robot2.pos.y
            current_task1.kickTask = task1
            self.task_pub1.publish(current_task1)
            if self.robotarrived(self.my_robot1, point.Point(self.m_wm.ball.pos.x, self.m_wm.ball.pos.y), 0.2) and math.hypot(self.my_robot1.pos.x - self.my_robot2.pos.x ,self.my_robot1.pos.y - self.my_robot2.pos.y) >  math.hypot(self.m_wm.ball.pos.x - self.my_robot2.pos.x ,self.m_wm.ball.pos.y - self.my_robot2.pos.y):
                self.kickstat = KickStat.ROBOT1KICKING
                # prepare the other robot for reciveing
                current_task2 = parsian_robot_task()
                current_task2.select = parsian_robot_task.RECIVEPASS
                task2 = parsian_skill_receivePass()
                task2.receiveRadius = 0.5
                task2.target.x = self.startingpoint2.x
                task2.target.y = self.startingpoint2.y
                current_task2.receivePassTask = task2
                self.task_pub2.publish(current_task2)
                return True
            else:
                return  False

        if self.neaarertoballid == 2:
            current_task2 = parsian_robot_task()
            current_task2.select = parsian_robot_task.KICK
            task2 = parsian_skill_kick()
            task2.dontKick = True
            task2.chip = False
            task2.target.x = self.my_robot1.pos.x
            task2.target.y = self.my_robot1.pos.y
            current_task2.kickTask = task2
            self.task_pub2.publish(current_task2)
            if self.robotarrived(self.my_robot2, point.Point(self.m_wm.ball.pos.x, self.m_wm.ball.pos.y), 0.2) and math.hypot(self.my_robot1.pos.x - self.my_robot2.pos.x ,self.my_robot1.pos.y - self.my_robot2.pos.y) >  math.hypot(self.m_wm.ball.pos.x - self.my_robot1.pos.x ,self.m_wm.ball.pos.y - self.my_robot1.pos.y):
                self.kickstat = KickStat.ROBOT2KICKING
                # prepare the other robot for reciveing
                current_task1 = parsian_robot_task()
                current_task1.select = parsian_robot_task.RECIVEPASS
                task1 = parsian_skill_receivePass()
                task1.receiveRadius = 0.5
                task1.target.x = self.startingpoint1.x
                task1.target.y = self.startingpoint1.y
                current_task1.receivePassTask = task1
                self.task_pub1.publish(current_task1)
                return True
            else:
                return  False

    def calculate(self):
        self.velrecflag = False
        self.velrecorder.sort()
        self.velrecorder.reverse()
        rospy.loginfo(self.velrecorder[0])
        self.velrecorder[:] = []
        return True





if __name__ == '__main__':
    try:
        rospy.init_node('kick_profiler', anonymous=True)
        rospy.loginfo("kick_profiler is running")
        kick = KickProfiler()
        rospy.spin()
    except rospy.ROSInterruptException:
        pass
