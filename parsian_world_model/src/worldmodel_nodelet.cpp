//
// Created by parsian-ai on 10/5/17.
//

#include <parsian_world_model/worldmodel_nodelet.h>
PLUGINLIB_EXPORT_CLASS(parsian_world_model::WMNodelet, nodelet::Nodelet);

using namespace parsian_world_model;

WMNodelet::WMNodelet() = default;

WMNodelet::~WMNodelet() = default;

void WMNodelet::onInit() {
    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& nh_private = getPrivateNodeHandle();

//    timer = nh.createTimer(ros::Duration(1.0), boost::bind(&WMNodelet::timerCb, this, _1));

    wm_pub = nh.advertise<parsian_msgs::parsian_world_model>("wm", 1000);
    vision_detection_sub = nh.subscribe("vision_detection", 10, &WMNodelet::detectionCb, this);
//    vision_geom_sub = nh.subscribe("vision_geom", 10, boost::bind(& WMNodelet::geomCb, this, _1));
}

//void WMNodelet::geomCb(const parsian_msgs::ssl_vision_geometryConstPtr &_geom) {
//
//}

void WMNodelet::detectionCb(const parsian_msgs::ssl_vision_detectionConstPtr &_detection) {
    parsian_msgs::parsian_world_model worldModel;

    wm.updateDetection(_detection);
    wm.execute();
    worldModel = wm.getParsianWorldModel();

    wm_pub.publish(worldModel);

}

