//
// Created by parsian-ai on 10/5/17.
//

#ifndef PARSIAN_WORLD_MODEL_WORLDMODEL_NODELET_H
#define PARSIAN_WORLD_MODEL_WORLDMODEL_NODELET_H

#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <nodelet/nodelet.h>

#include <parsian_msgs/parsian_world_model.h>
#include <parsian_msgs/ssl_vision_geometry.h>
#include <parsian_msgs/ssl_vision_detection.h>

#include <parsian_world_model/worldmodel_nodelet.h>
#include <parsian_world_model/wm/worldmodel.h>

namespace parsian_world_model {

    class WMNodelet : public nodelet::Nodelet {
    public:
        WMNodelet();
        virtual ~WMNodelet();

    protected:
    private:
        virtual void onInit();

        // Timer CallBack (to publish)
        void timerCb(const ros::TimerEvent& event);

        //Subs CallBack
        void geomCb(const parsian_msgs::ssl_vision_geometryConstPtr& _geom);
        void detectionCb(const parsian_msgs::ssl_vision_detectionConstPtr& _detection);


        CWorldModel wm;

        ros::Publisher wm_pub;

        ros::Subscriber vision_detection_sub;
        ros::Subscriber vision_geom_sub;

        ros::Timer timer;



    };
}

#endif //PARSIAN_WORLD_MODEL_WORLDMODEL_NODELET_H
