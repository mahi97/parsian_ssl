//
// Created by ali on 10/11/17.
//

#include "parsian_protobuf_wrapper/ssl-refbox/parsian_referee_nodelet.h"


PLUGINLIB_EXPORT_CLASS(parsian_protobuf_wrapper::RefreeNodelet, nodelet::Nodelet);

using namespace parsian_protobuf_wrapper;

void RefreeNodelet::onInit() {


    ros::NodeHandle &nh = getNodeHandle();
    ros::NodeHandle& nh_private = getPrivateNodeHandle();

    ssl_referee_pub  = nh_private.advertise<parsian_msgs::ssl_refree_wrapper>("referee", 1000);

    refBox = nullptr;
    networkConfig.port = 10003;
    networkConfig.ip = "224.5.23.1";
    reconnect();
    std::chrono::high_resolution_clock::time_point last_param_update_time;

    dynamic_reconfigure::Server<protobuf_wrapper_config::refereeConfig> server;
    dynamic_reconfigure::Server<protobuf_wrapper_config::refereeConfig>::CallbackType f;
    f = boost::bind(&RefreeNodelet::callback,this, _1, _2);
    server.setCallback(f);
    ros::param::get("/team_color", teamColor);
    isOurColorYellow = (teamColor == "yellow");
    timer = nh.createTimer(ros::Duration(.062), boost::bind(&RefreeNodelet::timerCb, this, _1));


  //  refBox->close();
   // delete refBox;

  //  ros::shutdown();

}

void RefreeNodelet::reconnect() {
    {
//    mergedHalfWorld.ball.clear();
//    for (int i=0;i<_NUM_PLAYERS;i++)
//    {
//        mergedHalfWorld.ourTeam[i].clear();
//        mergedHalfWorld.oppTeam[i].clear();
//    }
        if (refBox != NULL) delete refBox;
        refBox = new RoboCupSSLClient(networkConfig.port, networkConfig.ip.c_str());
        if (!refBox->open(false)) ROS_WARN("Connection Failed.");
        else ROS_INFO("Connected!");
    }
}

void RefreeNodelet::callback(const protobuf_wrapper_config::refereeConfig &config, uint32_t level) {
    {
//    ROS_INFO("Reconfigure Request: %s %d", config.vision_multicast_ip, config.vision_multicast_port);
        networkConfig.ip = config.refree_multicast_ip;
        networkConfig.port = config.refree_multicast_port;
        reconnect();

    }
}


void RefreeNodelet::timerCb(const ros::TimerEvent& event){
    parsian_msgs::ssl_refree_wrapper referee;

    if (refBox->receive(ssl_referee)) {
        referee = pr::convert_referee_data(ssl_referee, isOurColorYellow);
    }

    ssl_referee_pub.publish(referee);

}
