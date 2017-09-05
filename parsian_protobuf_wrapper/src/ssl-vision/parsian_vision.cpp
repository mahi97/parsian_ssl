#include <csignal>
#include <chrono>

#include "ros/ros.h"

#include <dynamic_reconfigure/server.h>
#include <parsian_protobuf_wrapper/ssl-vision/convert/convert_detection.h>
#include <parsian_protobuf_wrapper/ssl-vision/convert/convert_geometry.h>

#include "parsian_protobuf_wrapper/messages_robocup_ssl_wrapper.pb.h"
#include "parsian_protobuf_wrapper/common/net/robocup_ssl_client.h"

#include "parsian_protobuf_wrapper/protoConfig.h"


bool shutDown = false;
bool isOurColorYellow = false;
RoboCupSSLClient *vision;
protobuf_wrapper_config::protoConfig visionConfig;


void sigintHandler(int /*unused*/) {
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
    if (vision != NULL) delete vision;
    vision = new RoboCupSSLClient(visionConfig.vision_multicast_port, visionConfig.vision_multicast_ip.c_str());
    if (!vision->open(false)) ROS_WARN("Connection Failed.");
    else ROS_INFO("Connected!");
}

void callback(protobuf_wrapper_config::protoConfig  &config, uint32_t level) {
    visionConfig.vision_multicast_ip = config.vision_multicast_ip;
    visionConfig.vision_multicast_port = config.vision_multicast_port;
    reconnect();

    isOurColorYellow = config.team_color == 0;

}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "vision_node", ros::init_options::NoSigintHandler);
    signal(SIGINT, sigintHandler);
    ros::NodeHandle n;

    ros::Publisher ssl_geometry_pub  = n.advertise<parsian_msgs::ssl_vision_geometry>("vision_geom", 1000);
    ros::Publisher ssl_detection_pub = n.advertise<parsian_msgs::ssl_vision_detection>("vision_detection", 1000);

    ros::Rate loop_rate(62);
    vision = NULL;
    visionConfig.vision_multicast_port = 10006;
    visionConfig.vision_multicast_ip = "224.5.23.2";
    reconnect();

    dynamic_reconfigure::Server<protobuf_wrapper_config::protoConfig> server;
    dynamic_reconfigure::Server<protobuf_wrapper_config::protoConfig>::CallbackType f;

    f = boost::bind(&callback, _1, _2);
    server.setCallback(f);



    SSL_WrapperPacket vision_packet;

    while (ros::ok() && !shutDown) {

        parsian_msgs::ssl_vision_detection detection;
        parsian_msgs::ssl_vision_geometry geometry;
        if (vision->receive(vision_packet)) {
            if (vision_packet.has_detection()) {
                detection = pr::convert_detection_frame(vision_packet.detection(), isOurColorYellow);
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

    vision->close();
    delete vision;

    ros::shutdown();

    return 0;
}
