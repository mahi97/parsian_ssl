#include "ros/ros.h"

#include "parsian_msgs/parsian_world_model.h"
#include "parsian_ai/ai.h"


AI ai;

void wmCallback(const parsian_msgs::parsian_world_model& _wm) {
    ai.updateWM(_wm);
}

void robotStatusCallback(const parsian_msgs::parsian_robot& _robotStatus) {
    ai.updateRobotStatus(_robotStatus);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "ai_node");

    ros::NodeHandle n;

    ros::Subscriber worldModelSub   = n.subscribe("/world_model", 1000, wmCallback);
    ros::Subscriber robotStatusSub  = n.subscribe("/robot_status", 1000, robotStatusCallback);

    ros::Rate loop_rate(62);

    while (ros::ok()) {
        ai.execute();
        ros::spinOnce();
        loop_rate.sleep();
    }

    ros::shutdown();

    return 0;
}
