#include <parsian_protobuf_wrapper/ssl-vision/parsian_vision_nodelet.h>

PLUGINLIB_EXPORT_CLASS(parsian_protobuf_wrapper::VisionNodelet, nodelet::Nodelet);


using namespace parsian_protobuf_wrapper;

void VisionNodelet::onInit() {
    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& nh_private = getPrivateNodeHandle();

    timer = nh.createTimer(ros::Duration(.062), boost::bind(&VisionNodelet::timerCb, this, _1));

    ssl_geometry_pub  = nh.advertise<parsian_msgs::ssl_vision_geometry>("vision_geom", 1000);
    ssl_detection_pub = nh.advertise<parsian_msgs::ssl_vision_detection>("vision_detection", 1000);

    vision = nullptr;
    visionConfig.vision_multicast_port = 10006;
    visionConfig.vision_multicast_ip = "224.5.23.2";
    reconnect();

    configServer.reset(new dynamic_reconfigure::Server<protobuf_wrapper_config::visionConfig>(nh_private));
    dynamic_reconfigure::Server<protobuf_wrapper_config::visionConfig>::CallbackType f;
    f = boost::bind(&VisionNodelet::configCb,this, _1, _2);
    configServer->setCallback(f);
    ros::param::get("/team_color", teamColor);
    isOurColorYellow = (teamColor == "yellow");

}

void VisionNodelet::reconnect()
{
    delete vision;
    vision = new RoboCupSSLClient(visionConfig.vision_multicast_port, visionConfig.vision_multicast_ip);
    if (!vision->open(false)) NODELET_WARN("Connection Failed.");
    else NODELET_INFO("Connected!");

}

void VisionNodelet::configCb(const protobuf_wrapper_config::visionConfig &config , uint32_t level){
    visionConfig.vision_multicast_ip = config.vision_multicast_ip;
    visionConfig.vision_multicast_port = config.vision_multicast_port;
    reconnect();

}

void VisionNodelet::timerCb(const ros::TimerEvent &event) {
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
}

