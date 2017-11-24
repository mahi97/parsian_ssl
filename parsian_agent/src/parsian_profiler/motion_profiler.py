import json
import time
import math
import point
import rospy
from os import path
from parsian_msgs.msg import parsian_skill_gotoPointAvoid
from parsian_msgs.msg import parsian_robot_task
from parsian_msgs.msg import parsian_world_model
from parsian_msgs.msg import parsian_robot
from parsian_msgs.msg import parsian_robot_command

log_file = open(path.realpath("parsian_agent/profiler_data/motion_profiler.txt"), "w+")

move_type = {"going": 0, "coming_back": 1}

class MotionProfiler:
    def __init__(self, robot_id, start_pos, end_pos, init_phase=0, dist_step=3, ang_step=8, max_vel=4.5):
        # type: (int, point.Point, point.Point,int, int, int, int) -> object
        self.__init_phase = init_phase
        self.__last_move_type = move_type["coming_back"]
        self.__profilingIsFinished = True
        self.__ang_step = ang_step
        self.__dist_step = dist_step
        self.__start_pos = start_pos
        self.__end_pos = end_pos
        self.__robot_id = robot_id
        self.__max_vel = max_vel
        self.__result = dict()
        self.__result["max_vel"] = max_vel
        self.__result["robot_id"] = robot_id
        self.__result["data"] = list()
        self.__profiling = False
        self.__current_ang_step = 0
        self.__current_dist_step = -1
        self.__start_time = time.time()
        self.__path_angle = self.__start_pos.angle(self.__end_pos)
        self.__robot_command = parsian_robot_command()
        self.__wm_sub = rospy.Subscriber('world_model', parsian_world_model, self.wmCallback)
        self.__rbt_cmd_sub = rospy.Subscriber('robot_command' + str(robot_id), parsian_world_model, self.rcCallback)
        self.__task_pub = rospy.Publisher('robot_task_'+str(self.__robot_id), parsian_robot_task, queue_size=100)
        self.__current_task = parsian_robot_task()
        self.__current_task.select = parsian_robot_task.GOTOPOINTAVOID

    def rcCallback(self, data):
        rospy.loginfo("robot command rc")
        self.__robot_command = data

    def wmCallback(self, data):
        # type:(parsian_world_model)
        rospy.loginfo(str(self.__profilingIsFinished))
        if self.__profilingIsFinished:
            self.__getTask()
            self.__task_pub.publish(self.__current_task)
        else:
            self.doProfiling(data)

        my_robot = parsian_robot()
        my_robot.id = -1
        for robot in data.our:
            if robot.id == self.__robot_id:
                my_robot = robot

        if my_robot.id == -1:
            rospy.loginfo("robot "+str(self.__robot_id)+" losted!")
            return

        dis=point.Point(self.__current_task.gotoPointAvoidTask.base.targetPos.x,
                        self.__current_task.gotoPointAvoidTask.base.targetPos.y).distance(
            point.Point(my_robot.pos.x, my_robot.pos.y)
        )
        if dis < .001 :
            self.__profilingIsFinished = True

    def doProfiling(self, world_model):
        # type: (parsian_world_model) -> null
        dataList = list(self.__result["data"])

        my_robot = parsian_robot()
        my_robot.id = -1
        for robot in world_model.our:
            if robot.id == self.__robot_id:
                my_robot = robot

        if my_robot.id == -1:
            rospy.loginfo("robot not found")
            return

        vel_F = my_robot.vel.x * math.cos(self.__path_angle) + my_robot.vel.y * math.sin(self.__path_angle)
        vel_N = -1 * my_robot.vel.x * math.sin(self.__path_angle) + my_robot.vel.y * math.cos(self.__path_angle)

        if self.__last_move_type == move_type["coming_back"]:
            vel_F *= -1
            vel_N *= -1

        data = {
            "world_model": {"vel_F": vel_F, "vel_N": vel_N},
            "robot_command": {"vel_F": self.__robot_command.vel_F, "vel_N": self.__robot_command.vel_N}
        }

        dataList.append(data)
        self.__result["data"] = dataList

    def __saveResult(self):
        log_file.write(json.dumps(self.__result, sort_keys=True, indent=4, ensure_ascii=False))


    def __nextStep(self):
        self.__current_dist_step += 1
        if self.__current_dist_step == self.__dist_step:
            self.__current_dist_step = 0
            self.__current_ang_step += 1
            if self.__current_ang_step == self.__ang_step:
                self.__profilingIsFinished = True

    def __getTask(self):
        task = parsian_skill_gotoPointAvoid()
        task.base.robot_id = self.__robot_id
        task.base.lookAt.x = 5000
        task.base.lookAt.y = 5000
        task.base.maxVelocity = self.__max_vel
        if self.__current_dist_step == -1:
            task.base.targetPos.x = self.__start_pos.x
            task.base.targetPos.y = self.__start_pos.y
        else:
            if self.__last_move_type == move_type["coming_back"]:
                task.base.targetPos.x = self.__start_pos.x + self.__start_pos.difX(
                    self.__end_pos) * self.__current_dist_step / self.__dist_step
                task.base.targetPos.y = self.__start_pos.y + self.__start_pos.difY(
                    self.__end_pos) * self.__current_dist_step / self.__dist_step
                task.base.targetDir.x = math.cos(self.__getPhase())
                task.base.targetDir.y = math.sin(self.__getPhase())
                self.__last_move_type = move_type["going"]
            else:  # "COMING BACK"
                task.base.targetPos.x = self.__start_pos.x
                task.base.targetPos.y = self.__start_pos.y
                task.base.targetDir.x = -1 * math.cos(self.__getPhase())
                task.base.targetDir.y = -1 * math.sin(self.__getPhase())
                self.__last_move_type = move_type["coming_back"]
                self.__nextStep()
        self.__profilingIsFinished = False
        self.__current_task.gotoPointAvoidTask = task

    def __getPhase(self):
        return math.pi * (self.__current_ang_step * 2 / self.__ang_step + self.__init_angle / 180) + self.__path_angle

