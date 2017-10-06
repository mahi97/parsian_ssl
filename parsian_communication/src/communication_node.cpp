#include "ros/ros.h"

#include "parsian_msgs/parsian_debugs.h"
#include "parsian_msgs/parsian_draw.h"
#include "parsian_util/tools/drawer.h"
#include "parsian_util/tools/debuger.h"
#include "parsian_msgs/parsian_robot_packet.h"
#include "parsian_communication/communicator.h"


int main(int argc, char **argv)
{
    ros::init(argc, argv, "com_node");

    ros::NodeHandle n;
    CCommunicator communicator;

    ros::Publisher  drawPub    = n.advertise<parsian_msgs::parsian_draw>("/draws",1000);
    ros::Publisher  debugPub   = n.advertise<parsian_msgs::parsian_debugs>("/debugs",1000);

    ros::Subscriber robotPacketSub   = n.subscribe("/robot_packet_0" , 1000, communicator.packetCallBack);
    ros::Subscriber robotPacketSub   = n.subscribe("/robot_packet_1" , 1000, communicator.packetCallBack);
    ros::Subscriber robotPacketSub   = n.subscribe("/robot_packet_2" , 1000, communicator.packetCallBack);
    ros::Subscriber robotPacketSub   = n.subscribe("/robot_packet_3" , 1000, communicator.packetCallBack);
    ros::Subscriber robotPacketSub   = n.subscribe("/robot_packet_4" , 1000, communicator.packetCallBack);
    ros::Subscriber robotPacketSub   = n.subscribe("/robot_packet_5" , 1000, communicator.packetCallBack);
    ros::Subscriber robotPacketSub   = n.subscribe("/robot_packet_6" , 1000, communicator.packetCallBack);
    ros::Subscriber robotPacketSub   = n.subscribe("/robot_packet_7" , 1000, communicator.packetCallBack);
    ros::Subscriber robotPacketSub   = n.subscribe("/robot_packet_8" , 1000, communicator.packetCallBack);
    ros::Subscriber robotPacketSub   = n.subscribe("/robot_packet_9" , 1000, communicator.packetCallBack);
    ros::Subscriber robotPacketSub   = n.subscribe("/robot_packet_10", 1000, communicator.packetCallBack);
    ros::Subscriber robotPacketSub   = n.subscribe("/robot_packet_11", 1000, communicator.packetCallBack);



    ros::Rate loop_rate(62);

    while (ros::ok()) {

        drawPub.publish(drawer.draws);
        debugPub.publish(debugger.debugs);

        ros::spinOnce();
        loop_rate.sleep();
    }

    ros::shutdown();

    return 0;
}
