#include <csignal>
#include <chrono>

#include "ros/ros.h"

#include <dynamic_reconfigure/server.h>
#include <parsian_protobuf_wrapper/common/net/robocup_ssl_client.h>
#include <parsian_protobuf_wrapper/ssl-refbox/convert/convert_referee.h>

#include "parsian_protobuf_wrapper/referee.pb.h"
#include "parsian_msgs/ssl_refree_wrapper.h"
#include "parsian_msgs/ssl_refree_command.h"
#include "parsian_msgs/ssl_refree_stage.h"
#include "parsian_protobuf_wrapper/refBoxConfig.h"
#include "parsian_protobuf_wrapper/CommonConfig.h"

bool shutDown = false;
RoboCupSSLClient *refBox;

bool isOurColorYellow = false;

struct SNetworkConfig {
    std::string ip;
    int port;
} networkConfig;

void sigintHandler(int) {
    shutDown = true;
}

void reconnect()
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

void callback(parsian_protobuf_wrapper::refBoxConfig &config, uint32_t level) {
//    ROS_INFO("Reconfigure Request: %s %d", config.vision_multicast_ip, config.vision_multicast_port);
    networkConfig.ip = config.refree_multicast_ip;
    networkConfig.port = config.refree_multicast_port;
    reconnect();
}

void callback2(parsian_protobuf_wrapper::CommonConfig &config, uint32_t level) {
//    ROS_INFO("Reconfigure Request: %s %d", config.vision_multicast_ip, config.vision_multicast_port);
    isOurColorYellow = (config.team_color == 0);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "parsian_vision", ros::init_options::NoSigintHandler);
    signal(SIGINT, sigintHandler);
    ros::NodeHandle n;

    ros::Publisher ssl_referee_pub  = n.advertise<parsian_msgs::ssl_refree_wrapper>("referee", 1000);

    ros::Rate loop_rate(62);
    refBox = NULL;
    networkConfig.port = 10003;
    networkConfig.ip = "224.5.23.1";
    reconnect();
    std::chrono::high_resolution_clock::time_point last_param_update_time;

    dynamic_reconfigure::Server<parsian_protobuf_wrapper::refBoxConfig> server;
    dynamic_reconfigure::Server<parsian_protobuf_wrapper::refBoxConfig>::CallbackType f;
    dynamic_reconfigure::Server<parsian_protobuf_wrapper::CommonConfig> server2;
    dynamic_reconfigure::Server<parsian_protobuf_wrapper::CommonConfig>::CallbackType f2;


    f = boost::bind(&callback, _1, _2);
    server.setCallback(f);


    f2 = boost::bind(&callback2, _1, _2);
    server2.setCallback(f2);

    SSL_Referee ssl_referee;

    while (ros::ok() && !shutDown) {

        parsian_msgs::ssl_refree_wrapper referee;

        if (refBox->receive(ssl_referee)) {
            referee = pr::convert_referee_data(ssl_referee, isOurColorYellow);
        }

        ssl_referee_pub.publish(referee);
        ros::spinOnce();

        loop_rate.sleep();
    }

    refBox->close();
    delete refBox;

    ros::shutdown();

    return 0;
}
