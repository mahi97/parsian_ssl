#!/usr/bin/env python
# license removed for brevity


import rospy
import random
from enum import Enum
from parsian_msgs.msg import parsian_robot_task
from parsian_msgs.msg import vector2D
from parsian_msgs.msg import parsian_robot_command
from parsian_msgs.msg import parsian_skill_gotoPointAvoid
from parsian_msgs.msg import parsian_skill_no
from parsian_msgs.msg import parsian_world_model
from parsian_msgs.msg import parsian_robot
from parsian_msgs.msg import parsian_skill_receivePass
from parsian_msgs.msg import parsian_skill_kick
from point import Point
import math
import point
from parsian_msgs.msg import parsian_robot



class States(Enum):
    SET_ROLES = 0
    PASS_TO_KICK = 1
    KICK = 2

class Pass_Shoot_Plan():

    def __init__(self):
        self.wm = parsian_world_model
        self.state = States.SET_ROLES

        self.kicker = parsian_robot
        self.receiver = parsian_robot
        self.random_ids = []



    def execute(self, _wm):
        self.wm = _wm

        if self.state == States.SET_ROLES:
            if self.set_roles():
                self.state = States.PASS_TO_KICK


        if self.state == States.PASS_TO_KICK:
            if self.pass_to_kicker():
                self.state = States.KICK


        if self.state == States.KICK:
            if self.kick():
                return True

        if self.ball_out_of_field():
            return True

        if self.out_of_time():
            return True

        return False


    def set_roles(self):

        ## get nearest to ball
        nearerid = self.findnearesttoballID()
        for robot in self.wm.our:
            if robot.id == nearerid:
                self.kicker = robot
        kickertarget = self.generateSMWbehindP1inrelP2(point.Point(self.wm.ball.pos.x, self.wm.ball.pos.y), point.Point(6, 0))

        ## choose receiver
        for robot in self.wm.our:
            if robot.id != self.kicker.id:
                self.receiver = robot
                break

        ## generenate random positions
        for robot in self.wm.our:
            if robot.id != self.kicker.id and robot.id != self.receiver.id:
                tar = point.Point(random.uniform(0, 6), random.uniform(-4, 4))
                self.gotopoint(robot.id, tar)

        ## if kicker and receiver arrive -> send force_start
        if self.gotopoint(self.kicker.id, kickertarget):
            rospy.loginfo("finish")
            return True
        return False

    def pass_to_kicker(self):
        pass

        ## assign pass and receive skills
        task_pub1 = rospy.Publisher('agent_' + str(self.receiver.id) + str('/task'), parsian_robot_task,
                                         queue_size=1,
                                         latch=True)
        current_task1 = parsian_robot_task()
        current_task1.select = parsian_robot_task.RECIVEPASS
        task1 = parsian_skill_receivePass()
        task1.receiveRadius = 1
        task1.target.x = self.receiver.pos.x
        task1.target.y = self.receiver.pos.y
        current_task1.receivePassTask = task1
        task_pub1.publish(current_task1)

        task_pub2 = rospy.Publisher('agent_' + str(self.kicker.id) + str('/task'), parsian_robot_task,
                                         queue_size=1,
                                         latch=True)
        current_task2 = parsian_robot_task()
        current_task2.select = parsian_robot_task.KICK
        task2 = parsian_skill_kick()
        task2.chip = random.randint(0,2)
        task2.spin = random.randint(0,6)
        # task2.iskickchargetime = True
        # task2.kickchargetime = self.current_speed
        task2.kickSpeed = 400 ##
        task2.target.x = self.receiver.pos.x
        task2.target.y = self.receiver.pos.y
        current_task2.kickTask = task2
        task_pub2.publish(current_task2)
        ## set gotopoint skills in the for random points in their regions

        ## finished if ball enters the receivers region
        if math.hypot(self.wm.ball.vel.x, self.wm.ball.vel.y) > 0.2:
            current_task3 = parsian_robot_task()
            current_task3.select = parsian_robot_task.NOTASK
            task3 = parsian_skill_no()
            current_task3.noTask = task3
            task_pub2.publish(current_task3)
            return True
        return False

    def kick(self):
        pass

        ## assign kikck skill

        ## maybe give random positions and gotopoint skills

        ## finished if 1)ball speed increases very much and ball far from kicker


    def ball_out_of_field(self):
        pass

    def out_of_time(self):
        pass

    def dist(self, firstV2, secondV2):
        # type:(vector2D, vector2D) ->float
        return math.hypot(firstV2.x - secondV2.x, firstV2.y - secondV2.y)

    def findnearesttoballID(self):
        nearid = -1
        dist = 10000
        for robot in self.wm.our:
            if self.dist(robot.pos, self.wm.ball.pos) < dist:
                nearid = robot.id
                dist = self.dist(robot.pos, self.wm.ball.pos)
        return nearid



    def gotopoint(self, id, point, dirpoint = point.Point(0, 1)):
        # type:(int, point.Point, point.Point) ->object
        task_pub = rospy.Publisher('agent_' + str(id) + str('/task'), parsian_robot_task,
                                         queue_size=1,
                                         latch=True)
        current_task1 = parsian_robot_task()
        current_task1.select = parsian_robot_task.GOTOPOINTAVOID
        task1 = parsian_skill_gotoPointAvoid()
        task1.noAvoid = False
        task1.ballObstacleRadius = 0.3
        task1.base.lookAt.x = 5000
        task1.base.lookAt.y = 5000
        task1.base.maxVelocity = 0.1
        task1.base.targetPos.x = point.x
        task1.base.targetPos.y = point.y
        task1.base.targetDir.x = dirpoint.x #self.my_robot2.pos.x - self.my_robot1.pos.x
        task1.base.targetDir.y = dirpoint.y #self.my_robot2.pos.y - self.my_robot1.pos.y
        current_task1.gotoPointAvoidTask = task1
        task_pub.publish(current_task1)
        if self.robotarrived(id, point):
            return  True
        else:
            return False

    # if the robots arrived too the target or not
    def robotarrived(self, id, target, dist=0.1):
        # type:(int, point.Point) ->object
        robot = parsian_robot
        for mrobot in self.wm.our:
            if mrobot.id == id:
                robot = mrobot
        if math.hypot(robot.pos.x - target.x, robot.pos.y - target.y) < dist:
            return True
        else:
            return False

    def generateSMWbehindP1inrelP2(self, p1, p2):
        # type:(point.Point, point.Point) ->point.Point
        alpha = 0.3  # WTF: from 0.4
        target = point.Point(0, 0)
        r = point.Point(p1.x - p2.x,
                        p1.y - p2.y)
        r = r.unitPoint()
        r.x = r.x * alpha
        r.y = r.y * alpha
        target.x = p1.x + r.x
        target.y = p1.y + r.y
        return target
