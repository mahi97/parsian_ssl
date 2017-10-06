#include "ros/ros.h"

#include "parsian_msgs/parsian_debugs.h"
#include "parsian_msgs/parsian_draw.h"
#include "parsian_util/tools/drawer.h"
#include "parsian_util/tools/debuger.h"
#include "parsian_msgs/parsian_packets.h"
#include <parsian_communication/communicator.h>

CCommunicator communicator;

void callBack(const parsian_msgs::parsian_packetsConstPtr& _packet) {
    communicator.packetCallBack(_packet);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "com_node");

    ros::NodeHandle n;

    ros::Publisher  drawPub    = n.advertise<parsian_msgs::parsian_draw>("/draws",1000);
    ros::Publisher  debugPub   = n.advertise<parsian_msgs::parsian_debugs>("/debugs",1000);

    ros::Subscriber robotPacketSub   = n.subscribe("/robot_packets" , 1000, callBack);
    /////connect serial
    if(!communicator.isSerialConnected()){
        communicator.connectSerial("/dev/ttyUSB0");
    }
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
