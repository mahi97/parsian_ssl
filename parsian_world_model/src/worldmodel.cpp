#include "ros/ros.h"
#include "QDebug"


int main(int argc, char **argv)
{
    ros::init(argc, argv, "world_node");

    ros::NodeHandle n;
    ros::Rate loop_rate(62);
    while (ros::ok()) {

        ros::spinOnce();
        qDebug() << "MAHI IS THE BEST";
        loop_rate.sleep();
    }

    ros::shutdown();

    return 0;
}
