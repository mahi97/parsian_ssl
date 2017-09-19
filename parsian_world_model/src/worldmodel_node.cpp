#include "ros/ros.h"


#include "parsian_world_model/wm/worldmodel.h"
#include "parsian_msgs/ssl_vision_geometry.h"
#include "parsian_msgs/ssl_vision_detection.h"
#include "parsian_msgs/parsian_world_model.h"

CWorldModel wm;


void geomCallback(const parsian_msgs::ssl_vision_geometry& geom) {
    wm.updateGeom(geom);
}

void detectionCallback(const parsian_msgs::ssl_vision_detection& detection) {
    wm.updateDetection(detection);
}



int main(int argc, char **argv)
{
    ros::init(argc, argv, "world_node");

    ros::NodeHandle n;


    ros::Subscriber geomSub      = n.subscribe("vision_geom", 1000, geomCallback);
    ros::Subscriber detectionSub = n.subscribe("vision_detection", 1000, detectionCallback);

    ros::Publisher worldModelPub = n.advertise<parsian_msgs::parsian_world_model>("world_model",1000);

    ros::Rate loop_rate(62);
    parsian_msgs::parsian_world_model rosWM;
    while (ros::ok()) {
        wm.execute(rosWM);
        worldModelPub.publish(rosWM);
        ros::spinOnce();
        loop_rate.sleep();
    }

    ros::shutdown();

    return 0;
}
