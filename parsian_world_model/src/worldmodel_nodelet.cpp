//
// Created by parsian-ai on 10/5/17.
//

#include <parsian_world_model/worldmodel_nodelet.h>

PLUGINLIB_EXPORT_CLASS(parsian_world_model::WMNodelet, nodelet::Nodelet);

using namespace parsian_world_model;

void WMNodelet::onInit() {
    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& nh_private = getPrivateNodeHandle();

    wm = nullptr;
    wm = new CWorldModel(5);
//    timer = nh.createTimer(ros::Duration(.062), boost::bind(&WMNodelet::timerCb, this, _1));
    wm_pub = nh_private.advertise<parsian_msgs::parsian_world_model>("wm", 1000);
    vision_detection_sub = nh.subscribe("vision_detection", 10, &WMNodelet::detectionCb, this);
//    vision_geom_sub = nh.subscribe("vision_geom", 10, boost::bind(& WMNodelet::geomCb, this, _1));

}

//void WMNodelet::geomCb(const parsian_msgs::ssl_vision_geometryConstPtr &_geom) {
//
//}

void WMNodelet::detectionCb(const parsian_msgs::ssl_vision_detectionConstPtr &_detection) {
//

    wm->updateDetection(_detection);
    wm->execute();
    wm_pub.publish(wm->getParsianWorldModel());
    ROS_INFO("reddddc!");
//
}

