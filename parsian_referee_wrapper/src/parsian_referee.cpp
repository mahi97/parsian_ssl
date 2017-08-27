#include <csignal>
#include <chrono>

#include "ros/ros.h"

#include <dynamic_reconfigure/server.h>

#include "parsian_msgs/ssl_refree_command.h"
#include "parsian_util/referee.pb.h"
#include "parsian_referee_wrapper/net/robocup_ssl_client.h"


bool shutDown = false;
RoboCupSSLClient *refBox;
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

void callback(parsian_referee_wrapper::networkConfig  &config, uint32_t level) {
//    ROS_INFO("Reconfigure Request: %s %d", config.vision_multicast_ip, config.vision_multicast_port);
    networkConfig.ip = config.vision_multicast_ip;
    networkConfig.port = config.vision_multicast_port;
    reconnect();
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "parsian_vision", ros::init_options::NoSigintHandler);
    signal(SIGINT, sigintHandler);
    ros::NodeHandle n;

    ros::Publisher ssl_geometry_pub  = n.advertise<parsian_msgs::ssl_vision_geometry>("vision_geom", 1000);
    ros::Publisher ssl_detection_pub = n.advertise<parsian_msgs::ssl_vision_detection>("vision_detection", 1000);

    ros::Rate loop_rate(62);
    refBox = NULL;
    networkConfig.port = 10006;
    networkConfig.ip = "224.5.23.2";
    reconnect();
    std::chrono::high_resolution_clock::time_point last_param_update_time;

    dynamic_reconfigure::Server<ssl_vision_wrapper::networkConfig> server;
    dynamic_reconfigure::Server<ssl_vision_wrapper::networkConfig>::CallbackType f;

    f = boost::bind(&callback, _1, _2);
    server.setCallback(f);

    SSL_WrapperPacket vision_packet;

    while (ros::ok() && !shutDown) {

        parsian_msgs::ssl_vision_detection detection;
        parsian_msgs::ssl_vision_geometry geometry;
        if (refBox->receive(vision_packet)) {
            if (vision_packet.has_detection()) {
                detection = pr::convert_detection_frame(vision_packet.detection());
            }

            if (vision_packet.has_geometry()) {
                geometry = pr::convert_geometry_data(vision_packet.geometry());
            }
        }
        ssl_geometry_pub.publish(geometry);
        ssl_detection_pub.publish(detection);

        ros::spinOnce();

        loop_rate.sleep();
    }

    refBox->close();
    delete refBox;

    ros::shutdown();

    return 0;
}
