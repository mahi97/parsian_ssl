//
// Created by parsian-ai on 10/6/17.
//

#ifndef PARSIAN_COMMUNICATION_COMMUNICATION_NODELET_H
#define PARSIAN_COMMUNICATION_COMMUNICATION_NODELET_H
#define _MAX_ROBOT_NUM 12
#define _ROBOT_PACKET_SIZE 14
#include <nodelet/nodelet.h>
#include <pluginlib/class_list_macros.h>
#include <ros/ros.h>
#include "parsian_msgs/parsian_debugs.h"
#include "parsian_msgs/parsian_draw.h"
#include "parsian_util/tools/drawer.h"
#include "parsian_util/tools/debuger.h"
#include "parsian_msgs/parsian_robot_command.h"
#include "parsian_msgs/parsian_packets.h"
#include <parsian_communication/packet_generate/packet.h>
#include "parsian_msgs/ssl_vision_detection.h"
#include "parsian_msgs/parsian_world_model.h"
namespace parsian_packet {
    class PacketNodelet : public nodelet::Nodelet {

    private:
        void onInit();

        Packet packet;
        parsian_msgs::parsian_packets robotPacks;

        ros::Publisher  drawPub;
        ros::Publisher  debugPub;
        ros::Publisher  packetPub;
        ros::Timer      timer;

        ros::Subscriber robotPacketSub0 ;
        ros::Subscriber robotPacketSub1 ;
        ros::Subscriber robotPacketSub2 ;
        ros::Subscriber robotPacketSub3 ;
        ros::Subscriber robotPacketSub4 ;
        ros::Subscriber robotPacketSub5 ;
        ros::Subscriber robotPacketSub6 ;
        ros::Subscriber robotPacketSub7 ;
        ros::Subscriber robotPacketSub8 ;
        ros::Subscriber robotPacketSub9 ;
        ros::Subscriber robotPacketSub10;
        ros::Subscriber robotPacketSub11;


        ros::Subscriber visinSub;
        void callBack(const parsian_msgs::parsian_robot_commandConstPtr& _packet);
        void syncData(const parsian_msgs::parsian_world_modelConstPtr& _packet);

        unsigned char robotPackets[_MAX_ROBOT_NUM][_ROBOT_PACKET_SIZE];
        unsigned char visionCounter;

        // Timer CallBack (to publish)
        void timerCb(const ros::TimerEvent& event);

    };
}


#endif //PARSIAN_COMMUNICATION_COMMUNICATION_NODELET_H
