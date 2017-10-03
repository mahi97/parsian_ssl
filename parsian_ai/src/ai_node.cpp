#include "ros/ros.h"

#include "parsian_msgs/parsian_world_model.h"
#include "parsian_msgs/parsian_debugs.h"
#include "parsian_msgs/parsian_draw.h"
#include "parsian_msgs/ssl_refree_wrapper.h"

#include "parsian_ai/ai.h"

#include <parsian_util/tools/drawer.h>
#include <parsian_util/tools/debuger.h>


int main(int argc, char **argv)
{
    ros::init(argc, argv, "ai_node");
    ros::NodeHandle n;

    AI ai;

    ros::Subscriber worldModelSub   = n.subscribe("/world_model", 1000, ai.updateWM);
    ros::Subscriber robotStatusSub  = n.subscribe("/robot_status", 1000, ai.updateRobotStatus);
    ros::Subscriber refereeSub      = n.subscribe("/referee", 1000, ai.updateReferee);

    ros::Publisher  drawPub    = n.advertise<parsian_msgs::parsian_draw>("/draws",1000);
    ros::Publisher  debugPub   = n.advertise<parsian_msgs::parsian_debugs>("/debugs",1000);
//    ros::Publisher  statusPub  = n.advertise("/ai_status",1000);

    ros::Rate loop_rate(62);
    drawer = new Drawer();
    debugger = new Debugger();

    while (ros::ok()) {
        ai.execute();

        drawPub.publish(drawer->draws);
        debugPub.publish(debugger->debugs);

        ros::spinOnce();
        loop_rate.sleep();
    }

    ros::shutdown();

    return 0;
}
