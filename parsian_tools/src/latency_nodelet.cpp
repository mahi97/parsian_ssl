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


    world_model_sub   = nh.subscribe("/world_model", 10000, &Latency::wmCb, this);
    robot_task_sub    = nh.subscribe("/agent_0/task", 10, &Latency::rtCb, this);
    robot_command_sub = nh.subscribe("/agent_0/command", 10, &Latency::rcCb, this);
    planner_sub       = nh.subscribe("/planner_0/path", 5, &Latency::plannerCb, this);

    wm_sum = 0;
}

void Latency::wmCb(const parsian_msgs::parsian_world_modelConstPtr& _wm) {

    int latency = (ros::Time::now() - _wm->Header.stamp).nsec;
    wm_sum += latency;
    wm_latency_queue.push_back(latency);

    if(wm_latency_queue.length() > buffSize) {
        wm_sum -= wm_latency_queue.dequeue();
        ROS_INFO_STREAM("world model delay: " <<wm_sum / buffSize);
    }
}

void Latency::rtCb(const parsian_msgs::parsian_robot_taskConstPtr& _robot_task){
    int latency = (ros::Time::now() - _robot_task->Header.stamp).nsec;
    task_sum += latency;
    task_latency_queue.push_back(latency);

    if(wm_latency_queue.length() > buffSize) {
        task_sum -= wm_latency_queue.dequeue();
        ROS_INFO_STREAM("robot task delay: " << task_sum / buffSize);
    }
}

void Latency::plannerCb(const parsian_msgs::parsian_pathConstPtr & _path) {
}

void Latency::rcCb(const parsian_msgs::parsian_robot_commandConstPtr & _robot_command) {
    int latency = (ros::Time::now() - _robot_command->Header.stamp).nsec;
    rc_sum += latency;
    rc_latency_queue.push_back(latency);

    if(rc_latency_queue.length() > buffSize) {
        rc_sum -= rc_latency_queue.dequeue();
        ROS_INFO_STREAM("robot command delay: " << rc_sum / buffSize);
    }
}