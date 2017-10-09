//
// Created by parsian-ai on 10/6/17.
//

#include <parsian_communication/communication_nodelet.h>
PLUGINLIB_EXPORT_CLASS(parsian_communication::CommunicationNodelet, nodelet::Nodelet)

using namespace parsian_communication;


void CommunicationNodelet::onInit() {


    ros::NodeHandle& n = getNodeHandle();
    ros::NodeHandle& private_n = getPrivateNodeHandle();

    timer = n.createTimer(ros::Duration(.062), boost::bind(&CommunicationNodelet::timerCb, this, _1));

    drawer = new Drawer();
    debugger = new Debugger();

    drawPub    = private_n.advertise<parsian_msgs::parsian_draw>("/draws",1000);
    debugPub   = private_n.advertise<parsian_msgs::parsian_debugs>("/debugs",1000);
    ros::Subscriber robotPacketSub   = n.subscribe("/robot_packets" , 1000, &CommunicationNodelet::callBack, this);
    /////connect serial
    if(!communicator.isSerialConnected()){
        communicator.connectSerial("/dev/ttyUSB0");
    }
//    ros::Rate loop_rate(62);
//
//    while (ros::ok()) {
//
//        drawPub.publish(drawer->draws);
//        debugPub.publish(debugger->debugs);
//
//        ros::spinOnce();
//        loop_rate.sleep();
//    }

}

void CommunicationNodelet::callBack(const parsian_msgs::parsian_packetsConstPtr& _packet) {
    communicator.packetCallBack(_packet);
}

void CommunicationNodelet::timerCb(const ros::TimerEvent &event) {
    if (drawer != nullptr)
        drawPub.publish(drawer->draws);
    if (debugger != nullptr)
        debugPub.publish(debugger->debugs);
}
//PLUGINLIB_DECLARE_CLASS(parsian_communication,CommunicationNodelet,parsian_communication::CommunicationNodelet,nodelet::Nodelet);