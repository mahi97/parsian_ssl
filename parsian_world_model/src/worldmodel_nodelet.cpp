#include <parsian_world_model/worldmodel_nodelet.h>

PLUGINLIB_EXPORT_CLASS(parsian_world_model::WMNodelet, nodelet::Nodelet);

using namespace parsian_world_model;

void WMNodelet::onInit() {
    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& private_nh = getPrivateNodeHandle();

    wm = nullptr;
    wm = new CWorldModel(5);
//    timer = nh.createTimer(ros::Duration(.062), boost::bind(&WMNodelet::timerCb, this, _1));
    wm_pub = nh.advertise<parsian_msgs::parsian_world_model>("/world_model", 1000);
    vision_detection_sub = nh.subscribe("vision_detection", 1000, &WMNodelet::detectionCb, this);
//    vision_geom_sub = nh.subscribe("vision_geom", 10, boost::bind(& WMNodelet::geomCb, this, _1));

//    server.reset(new dynamic_reconfigure::Server<world_model_config::world_modelConfig>(private_nh));
//    dynamic_reconfigure::Server<world_model_config::world_modelConfig>::CallbackType f;
//    f = boost::bind(&WMNodelet::ConfigServerCallBack,this, _1, _2);
//    server->setCallback(f);

}

//void WMNodelet::geomCb(const parsian_msgs::ssl_vision_geometryConstPtr &_geom) {
//
//}

void WMNodelet::detectionCb(const parsian_msgs::ssl_vision_detectionConstPtr &_detection) {

    wm->updateDetection(_detection);
    wm->execute();
    frame ++;
    packs ++;
    if (packs >= 4) {
        packs = 0;
        wm->merge(frame);
        parsian_msgs::parsian_world_modelPtr temp = wm->getParsianWorldModel(colour_yellow, side_left);
        temp->header.stamp = ros::Time::now();
        temp->header.frame_id = std::to_string(_detection->frame_number);
        wm_pub.publish(temp);
    }

}

//void WMNodelet::ConfigServerCallBack(const world_model_config::world_modelConfig &config, uint32_t level)
//{
//  m_config.active_cam_num = config.active_cam_num;
//  m_config.camera_one_active = config.camera_one_active;
//  m_config.camera_two_active = config.camera_two_active;
//  m_config.camera_three_active = config.camera_three_active;
//  m_config.camera_four_active = config.camera_four_active;
//}
