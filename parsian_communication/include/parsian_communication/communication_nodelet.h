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


namespace parsian_communication {
    class CommunicationNodelet : public nodelet::Nodelet {
    public:
        CommunicationNodelet();
        ~CommunicationNodelet();
    private:
        void onInit();

        CCommunicator communicator;
        void callBack(const parsian_msgs::parsian_packetsConstPtr& _packet);

    };
}


#endif //PARSIAN_COMMUNICATION_COMMUNICATION_NODELET_H
