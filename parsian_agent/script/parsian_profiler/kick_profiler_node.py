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
#2)the receivepass skill as publishing in a while loop its not good body
#3)better to modify the code after ballreplacement skill fixed(if the ball doesnt received in the starting points)
#4)ballwtf() function clears that if the ball had a suuden change in its speed must be declared

class State(Enum):
    GOTOPBOTH = 1
    GOFORKICK = 2
    KICK = 3
    RECEIVE = 4
    RETREAT = 5

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
        self.wm_sub = rospy.Subscriber('world_model', parsian_world_model, self.wmCallback, queue_size=1,
                                  buff_size=2 ** 24)
        # self.rbt_cmd_sub = rospy.Subscriber('robot_command' + str(self.robot_id), parsian_robot_command, self.rcCallback,
        #                                queue_size=1, buff_size=2 ** 24)






    def wmCallback(self, data):
        # type:(parsian_world_model) ->object
        self.m_wm = data
        self.getrobots(0, 1)
        self.updatenearballid()
        rospy.loginfo(self.state)
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
            self.receive()



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




    #clears if the robot has the ball or not
    def ballisnear(self, robot, dist = 0.18):
        # type:(parsian_robot, float) ->object
        ballpos = point.Point(self.m_wm.ball.pos.x, self.m_wm.ball.pos.y)
        robotpos = point.Point(robot.pos.x, robot.pos.y)
        if ballpos.distance(robotpos) < dist and math.atan2(math.fabs(ballpos.x - robotpos.x), math.fabs(ballpos.y - robotpos.y)) < math.pi / 3:
            return 0
        elif ballpos.distance(robotpos) < dist and not math.atan2(math.fabs(ballpos.x - robotpos.x), math.fabs(ballpos.y - robotpos.y)) < math.pi / 3:
            return 1
        elif not ballpos.distance(robotpos) < dist and math.atan2(math.fabs(ballpos.x - robotpos.x), math.fabs(ballpos.y - robotpos.y)) < math.pi / 3:
            return 2
        else:
            return 3


    #if ball had a sudden change in its speed robot will go after the ball till reach it
    def foundtheball(self, robot):
        # type:(parsian_robot) ->object
        if self.ballisnear(robot) == 0:
            return True
        elif self.ballisnear(robot) == 1:
            self.gotopoint(robot.id, point.Point(robot.pos.x, robot.pos.y), self.setdir(robot.pos.x, robot.pos.y, self.m_wm.ball.pos.x, self.m_wm.ball.pos.y))
            return False
        elif self.ballisnear(robot) == 2 or self.ballisnear(robot) == 3:
            self.gotopoint(robot.id, point.Point(self.m_wm.ball.pos.x, self.m_wm.ball.pos.y), point.Point(robot.dir.x, robot.dir.y))
            return False



    def kick(self):
        # robot1 should do the kicking and robot2 will receive
        if self.kickstat == KickStat.ROBOT1KICKING:
            current_task1 = parsian_robot_task()
            current_task1.select = parsian_robot_task.KICK
            task1 = parsian_skill_kick()
            task1.chip = False
            task1.kickSpeed = 5
            task1.target.x = self.my_robot2.pos.x
            task1.target.y = self.my_robot2.pos.y
            current_task1.kickTask = task1
            self.task_pub1.publish(current_task1)
            if math.hypot(self.m_wm.ball.vel.x, self.m_wm.ball.vel.y) > 1 and math.hypot(self.m_wm.ball.pos.x - self.my_robot1.pos.x, self.m_wm.ball.pos.y - self.my_robot1.pos.y) > 0.2:
                self.kickstat = KickStat.ROBOT1RETREATING
                return True
            else:
                return False

            # while not self.foundtheball(self.my_robot2):                    #TODO it shouldnt be in a while loop
            #     current_task2 = parsian_robot_task()
            #     current_task2.select = parsian_robot_task.RECIVEPASS
            #     task2 = parsian_skill_receivePass()
            #     task2.receiveRadius = 0.2
            #     current_task2.receivePassTask = task2
            #     self.task_pub2.publish(current_task2)

        # robot2 should do the kicking and robot1 will receive
        if self.kickstat == KickStat.ROBOT2KICKING:
            current_task2 = parsian_robot_task()
            current_task2.select = parsian_robot_task.KICK
            task2 = parsian_skill_kick()
            task2.chip = False
            task2.kickSpeed = 5
            task2.target.x = self.my_robot1.pos.x
            task2.target.y = self.my_robot1.pos.y
            current_task2.kickTask = task2
            self.task_pub2.publish(current_task2)
            if math.hypot(self.m_wm.ball.vel.x, self.m_wm.ball.vel.y) > 1 and math.hypot(self.m_wm.ball.pos.x - self.my_robot2.pos.x, self.m_wm.ball.pos.y - self.my_robot2.pos.y) > 0.2:
                self.kickstat = KickStat.ROBOT2RETREATING
                return True
            else:
                return False

            # while not self.foundtheball(self.my_robot1):                    #TODO it shouldnt be in a while loop
            #     current_task1 = parsian_robot_task()
            #     current_task1.select = parsian_robot_task.RECIVEPASS
            #     task1 = parsian_skill_receivePass()
            #     task1.receiveRadius = 0.2
            #     current_task1.receivePassTask = task1
            #     self.task_pub1.publish(current_task1)



    def retreat(self):
        if self.kickstat == KickStat.ROBOT1RETREATING:
            rospy.loginfo('1ret')
            self.gotopoint(1, self.startingpoint1, self.setdirtorobot(2))
            self.receivestat = ReceiveStat.ROBOT2RECING
        if self.kickstat == KickStat.ROBOT2RETREATING:
            rospy.loginfo('2ret')
            self.gotopoint(2, self.startingpoint2, self.setdirtorobot(1))
            self.receivestat = ReceiveStat.ROBOT1RECING


    def ontheway(self, robot, target):
        # type:(parsian_robot, point.Point) ->object
        if robot.vel.x * (target.x - robot.pos.x) > 0 and robot.vel.y * (target.y - robot.pos.y) > 0:
            return True
        else:
            return False

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

        if self.receivestat == ReceiveStat.ROBOT2RECING:
            current_task2 = parsian_robot_task()
            current_task2.select = parsian_robot_task.RECIVEPASS
            task2 = parsian_skill_receivePass()
            task2.receiveRadius = 0.5
            task2.target.x = self.startingpoint2.x
            task2.target.y = self.startingpoint2.y
            current_task2.receivePassTask = task2
            self.task_pub2.publish(current_task2)



    #clears that if the ball had a suuden change in its speed
    def ballwtf(self):
        if math.hypot(self.m_wm.ball.vel.x, self.m_wm.ball.vel.y) == 0:
            return True

    def updatenearballid(self):
        robot1_ball = math.hypot(self.my_robot1.pos.x - self.m_wm.ball.pos.x, self.my_robot1.pos.y - self.m_wm.ball.pos.y)
        robot2_ball = math.hypot(self.my_robot2.pos.x - self.m_wm.ball.pos.x, self.my_robot2.pos.y - self.m_wm.ball.pos.y)
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
            if self.robotarrived(self.my_robot1, point.Point(self.m_wm.ball.pos.x, self.m_wm.ball.pos.y), 0.2):
                self.kickstat = KickStat.ROBOT1KICKING
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
            if self.robotarrived(self.my_robot2, point.Point(self.m_wm.ball.pos.x, self.m_wm.ball.pos.y), 0.2):
                self.kickstat = KickStat.ROBOT2KICKING
                return True
            else:
                return  False






if __name__ == '__main__':
    try:
        rospy.init_node('kick_profiler', anonymous=True)
        rospy.loginfo("kick_profiler is running")
        kick = KickProfiler()
        rospy.spin()
    except rospy.ROSInterruptException:
        pass
