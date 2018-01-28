//
// Created by parsian-ai on 10/6/17.
//

#ifndef PARSIAN_COMMUNICATION_COMMUNICATION_NODELET_H
#define PARSIAN_COMMUNICATION_COMMUNICATION_NODELET_H

#include <nodelet/nodelet.h>
#include <pluginlib/class_list_macros.h>

#include "parsian_msgs/parsian_debugs.h"
#include "parsian_msgs/parsian_draw.h"
#include "parsian_util/tools/drawer.h"
#include "parsian_util/tools/debuger.h"
#include "parsian_msgs/parsian_packets.h"

#include <parsian_communication/communicator.h>

#include <parsian_msgs/parsian_team_config.h>

#include <dynamic_reconfigure/server.h>
#include "parsian_communication/communicationConfig.h"


namespace parsian_communication {
    class CommunicationNodelet : public nodelet::Nodelet {

    private:
        void onInit();

        boost::shared_ptr<CCommunicator> communicator;
        ros::Subscriber robotPacketSub;
        ros::Subscriber team_config_sub;
        ros::Publisher  drawPub;
        ros::Publisher  debugPub;
        ros::Publisher  statusPub;
        ros::Timer      timer;
        ros::Timer      recTimer;


        void callBack(const parsian_msgs::parsian_packetsConstPtr& _packet);
        // Timer CallBack (to publish)
        void timerCb(const ros::TimerEvent& event);

        void recTimerCb(const ros::TimerEvent& event);

        boost::shared_ptr<dynamic_reconfigure::Server<communication_config::communicationConfig>> server;
        void ConfigServerCallBack(const communication_config::communicationConfig &config, uint32_t level);
        communication_config::communicationConfig conf;

        void teamConfigCb(const parsian_msgs::parsian_team_configConstPtr& msg);
        bool realGame = false;

    };
}


#endif //PARSIAN_COMMUNICATION_COMMUNICATION_NODELET_H
