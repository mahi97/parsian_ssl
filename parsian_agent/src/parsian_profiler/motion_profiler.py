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
    def __init__(self, robot_id, start_pos, end_pos, init_phase=0, dist_step=3.0, ang_step=7.0, max_vel=4.5):
        # type: (int, point.Point, point.Point,float, float, float, float) -> object
        self.__init_phase = init_phase
        self.__last_move_type = move_type["coming_back"]
        self.__profilingIsFinished = False
        self.__doProfiling = False
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
        self.__rbt_cmd_sub = rospy.Subscriber('robot_command' + str(robot_id), parsian_robot_command, self.rcCallback)
        self.__task_pub = rospy.Publisher('robot_task_'+str(self.__robot_id), parsian_robot_task, queue_size=100, latch=True)
        self.__current_task = parsian_robot_task()
        self.__current_task.select = parsian_robot_task.GOTOPOINTAVOID
        self.__wait_time = 0.0
        self.__wating_mode = False

    def rcCallback(self, data):
        self.__robot_command = data

    def wmCallback(self, data):
        # type:(parsian_world_model)
        if self.__profilingIsFinished:
            return

        if self.__doProfiling:
            self.doProfiling(data)
        else:
            self.__getTask()

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

        if dis < .03 :
            if(self.__wating_mode):
                if time.time() - self.__wait_time > .5:
                    self.__doProfiling = False
            else:
                self.__wait_time = time.time()
                self.__wating_mode = True

        else :
            self.__wating_mode = False


        self.__task_pub.publish(self.__current_task)

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

        dis=point.Point(self.__current_task.gotoPointAvoidTask.base.targetPos.x,
                        self.__current_task.gotoPointAvoidTask.base.targetPos.y).distance(
            point.Point(my_robot.pos.x, my_robot.pos.y)
        )
        data = {
            "task" : {"dist" : dis},
            "world_model": {"vel_F": vel_F, "vel_N": vel_N},
            "robot_command": {"vel_F": self.__robot_command.vel_F, "vel_N": self.__robot_command.vel_N}
        }

        dataList.append(data)
        self.__result["data"] = dataList

    def __saveResult(self):
        log_file.write(json.dumps(self.__result, sort_keys=True, indent=4, ensure_ascii=False))

    def __nextStep(self):

        if self.__current_dist_step == self.__dist_step:
            if self.__current_ang_step == self.__ang_step:
                rospy.loginfo("profiling finished!!")
                self.__saveResult()
                self.__profilingIsFinished = True
            else :
                self.__current_dist_step = 0
                self.__current_ang_step += 1
        else :
            self.__current_dist_step += 1

    def __getTask(self):
        task = parsian_skill_gotoPointAvoid()
        task.noAvoid = True
        task.base.robot_id = self.__robot_id
        task.base.lookAt.x = 5000
        task.base.lookAt.y = 5000
        task.base.maxVelocity = self.__max_vel
        if self.__current_dist_step == -1:
            task.base.targetPos.x = self.__start_pos.x
            task.base.targetPos.y = self.__start_pos.y
            self.__nextStep()
        else:
            if self.__last_move_type == move_type["coming_back"]:
                task.base.targetPos.x = self.__start_pos.x + self.__start_pos.difX(
                    self.__end_pos) * self.__current_dist_step / float(self.__dist_step)
                task.base.targetPos.y = self.__start_pos.y + self.__start_pos.difY(
                    self.__end_pos) * self.__current_dist_step / float(self.__dist_step)
                task.base.targetDir.x = math.cos(self.__getPhase())
                task.base.targetDir.y = math.sin(self.__getPhase())
                self.__last_move_type = move_type["going"]
            else:  # "COMING BACK"
                task.base.targetPos.x = self.__start_pos.x
                task.base.targetPos.y = self.__start_pos.y
                task.base.targetDir.x = math.cos(self.__getPhase())
                task.base.targetDir.y = math.sin(self.__getPhase())
                self.__last_move_type = move_type["coming_back"]
                self.__nextStep()
            self.__doProfiling = True
        self.__current_task.gotoPointAvoidTask = task
        rospy.loginfo(str(self.__current_task.gotoPointAvoidTask.base.targetPos.x)+"  "+str(self.__current_task.gotoPointAvoidTask.base.targetPos.x))

    def __getPhase(self):
        return math.pi * (self.__current_ang_step * 2 / float(self.__ang_step) + self.__init_phase / 180.0) + self.__path_angle

