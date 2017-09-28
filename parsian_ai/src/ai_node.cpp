#include "ros/ros.h"

#include "parsian_msgs/parsian_world_model.h"
#include "parsian_msgs/parsian_debugs.h"
#include "parsian_msgs/parsian_draw.h"
#include "parsian_ai/ai.h"


AI ai;

void wmCallback(const parsian_msgs::parsian_world_model& _wm) {
    ai.updateWM(_wm);
}

void robotStatusCallback(const parsian_msgs::parsian_robot& _robotStatus) {
    ai.updateRobotStatus(_robotStatus);
}

void gameStateCallBack(const int& ) {

}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "ai_node");

    ros::NodeHandle n;

    ros::Subscriber worldModelSub   = n.subscribe("/world_model", 1000, wmCallback);
    ros::Subscriber robotStatusSub  = n.subscribe("/robot_status", 1000, robotStatusCallback);
    ros::Subscriber gameStateSub    = n.subscribe("/game_state", 1000, gameStateCallBack);

    ros::Publisher  drawPub    = n.advertise<parsian_msgs::parsian_draw>("/draws",1000);
    ros::Publisher  debugPub   = n.advertise<parsian_msgs::parsian_debugs>("/debugs",1000);
//    ros::Publisher  statusPub  = n.advertise("/ai_status",1000);

    ros::Rate loop_rate(62);

    parsian_msgs::parsian_debugs debugs;
    parsian_msgs::parsian_draw   draw;


    while (ros::ok()) {
        ai.execute();

        drawPub.publish(debugs);
        debugPub.publish(draw);

        ros::spinOnce();
        loop_rate.sleep();
    }

    ros::shutdown();

    return 0;
}
