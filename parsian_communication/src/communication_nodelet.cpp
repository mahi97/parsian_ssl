//
// Created by parsian-ai on 10/6/17.
//

#include <parsian_communication/communication_nodelet.h>
PLUGINLIB_EXPORT_CLASS(parsian_communication::CommunicationNodelet, nodelet::Nodelet)

using namespace parsian_communication;


void CommunicationNodelet::onInit() {


    ros::NodeHandle& n = getNodeHandle();
    ros::NodeHandle& private_nh = getPrivateNodeHandle();

    timer = n.createTimer(ros::Duration(.016), boost::bind(&CommunicationNodelet::timerCb, this, _1));
    recTimer = n.createTimer(ros::Duration(.02), boost::bind(&CommunicationNodelet::recTimerCb, this, _1));

    drawer = new Drawer();
    debugger = new Debugger();

    drawPub    = n.advertise<parsian_msgs::parsian_draw>("/draws",1000);
    debugPub   = n.advertise<parsian_msgs::parsian_debugs>("/debugs",1000);
    statusPub  = n.advertise<parsian_msgs::parsian_robots_status>("/robots_status",1000);
    robotPacketSub   = n.subscribe("/packets" , 10000, &CommunicationNodelet::callBack, this);
    team_config_sub = n.subscribe("/team_config", 1000, & CommunicationNodelet::teamConfigCb, this);


    communicator.reset(new CCommunicator);
    /////connect serial
    while(!communicator->isSerialConnected()){
        communicator->connectSerial(conf.serial_connect.c_str());
    }

    server.reset(new dynamic_reconfigure::Server<communication_config::communicationConfig>(private_nh));
    dynamic_reconfigure::Server<communication_config::communicationConfig>::CallbackType f;
    f = boost::bind(&CommunicationNodelet::ConfigServerCallBack,this, _1, _2);
    server->setCallback(f);
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
  //ROS_INFO("salam");
    if (realGame)
        communicator->packetCallBack(_packet);

}

void CommunicationNodelet::timerCb(const ros::TimerEvent &event) {
    if (drawer != nullptr){
        drawPub.publish(drawer->draws);
        drawer->draws.vectors.clear();
    }
    if (debugger != nullptr)
        debugPub.publish(debugger->debugs);
}


void CommunicationNodelet::recTimerCb(const ros::TimerEvent &event) {
    communicator->readData();
    if(communicator->robotsStat != nullptr)
    {
        statusPub.publish(communicator->robotsStat);
    }
}

void CommunicationNodelet::ConfigServerCallBack(const communication_config::communicationConfig &config, uint32_t level)
{
  conf = config;
}

void CommunicationNodelet::teamConfigCb(const parsian_msgs::parsian_team_configConstPtr& msg)
{
    realGame = msg->mode == parsian_msgs::parsian_team_config::REAL;
}

//PLUGINLIB_DECLARE_CLASS(parsian_communication,CommunicationNodelet,parsian_communication::CommunicationNodelet,nodelet::Nodelet);
