//
// Created by ali on 1/10/18.
//

#include <latency/latency_nodelet.h>

PLUGINLIB_EXPORT_CLASS(parsian_tools::Latency, nodelet::Nodelet);

using namespace parsian_tools;

void Latency::onInit(){
    ROS_INFO("%s oninit", getName().c_str());

    nh = getNodeHandle();

    world_model = new LatencyInfo<parsian_world_model>    (nh,"world model"  ,"/world_model") ;
    robot_command= new LatencyInfo<parsian_robot_command> (nh,"robot command","/agent_0/command") ;
    vision = new LatencyInfo<ssl_vision_detection>        (nh,"robot task"   ,"/agent_0/task")  ;
    plan = new LatencyInfo<parsian_get_plan    >          (nh,"vision"       ,"/vision_detection");
    //robot_task = new LatencyInfo<parsian_robot_task>    (nh,"plan"         ,"/agent_0/plan")  ;
    path = new LatencyInfo<parsian_path         >         (nh,"path"         ,"/planner_0/path");
    packet =new LatencyInfo<parsian_packets>              (nh,"packet"       ,"/packets");


}
