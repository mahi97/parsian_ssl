//
// Created by ali on 1/10/18.
//

#include <latency_nodelet.h>

PLUGINLIB_EXPORT_CLASS(parsian_tools::Latency, nodelet::Nodelet);

using namespace parsian_tools;
void Latency::onInit(){
    ROS_INFO("%s oninit", getName().c_str());

    nh = getNodeHandle();
    private_nh = getPrivateNodeHandle();


    world_model_sub   = nh.subscribe("/world_model", 20, &Latency::wmCb, this);
    robot_task_sub    = nh.subscribe("/agent_0/task", 20, &Latency::rtCb, this);
    robot_command_sub = nh.subscribe("/agent_0/command", 20, &Latency::rcCb, this);
    planner_sub       = nh.subscribe("/planner_0/path",20, &Latency::plannerCb, this);
    vision_sub        = nh.subscribe("/vision_detection",20,&Latency::visionCb,this);

    wm_sum = vision_sum = task_sum = 0;
    wm_max = vision_max = task_max = 0;
    wm_min = vision_min = task_min = 0xffffffff;

}

void Latency::wmCb(const parsian_msgs::parsian_world_modelConstPtr& _wm) {

    int latency = (ros::Time::now() - _wm->Header.stamp).nsec;
    wm_sum += latency;
    wm_latency_queue.push_back(latency);

    if (latency > wm_max)
        wm_max = latency;
    if (latency < wm_min)
        wm_min = latency;

    if(wm_latency_queue.length() > buffSize) {
        wm_sum -= wm_latency_queue.dequeue();
        ROS_INFO_STREAM("world model delay: min: " <<vision_min <<
                                                  "    mid: "<< wm_sum / buffSize<< "    max: "<<vision_max);
    }
}

void Latency::rtCb(const parsian_msgs::parsian_robot_taskConstPtr& _robot_task){
    int latency = (ros::Time::now() - _robot_task->Header.stamp).nsec;
    task_sum += latency;
    task_latency_queue.push_back(latency);

    if (latency > task_max)
        task_max = latency;
    if (latency < task_min)
        task_min = latency;

    if(wm_latency_queue.length() > buffSize) {
        task_sum -= task_latency_queue.dequeue();
        ROS_INFO_STREAM("robot task delay: min: " <<vision_min <<
                                                     "    mid: "<< task_sum / buffSize<< "    max: "<<vision_max);
    }
}

void Latency::plannerCb(const parsian_msgs::parsian_pathConstPtr & _path) {
}

void Latency::rcCb(const parsian_msgs::parsian_robot_commandConstPtr & _robot_command) {
    int latency = (ros::Time::now() - _robot_command->Header.stamp).nsec;
    rc_sum += latency;
    rc_latency_queue.push_back(latency);

    if (latency > rc_max)
        rc_max = latency;
    if (latency < rc_min)
        rc_min = latency;

    if(rc_latency_queue.length() > buffSize) {
        rc_sum -= rc_latency_queue.dequeue();
        ROS_INFO_STREAM("robot command delay: min: " <<vision_min <<
                                              "    mid: "<< rc_sum / buffSize<< "    max: "<<vision_max);
    }
}

void Latency::visionCb(const parsian_msgs::ssl_vision_detectionConstPtr & _vision_detection) {
    int latency = (ros::Time::now() - _vision_detection->Header.stamp).nsec;
    vision_sum += latency;
    vision_latency_queue.push_back(latency);

    if (latency > vision_max)
        vision_max = latency;
    if (latency < vision_min)
        vision_min = latency;

    if(vision_latency_queue.length() > buffSize) {
        vision_sum -= vision_latency_queue.dequeue();
        ROS_INFO_STREAM("vision delay: min: " <<vision_min <<
                                   "    mid: "<< vision_sum / buffSize<< "    max: "<<vision_max);
    }
}