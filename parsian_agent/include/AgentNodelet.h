//
// Created by ali on 10/15/17.
//

#ifndef PARSIAN_AGENT_AGENTNODELET_H
#define PARSIAN_AGENT_AGENTNODELET_H

#include <nodelet/nodelet.h>
#include <ros/ros.h>

class AgentNodelet : public nodelet::Nodelet {
private:
    virtual void onInit();

    void timerCb(const ros::TimerEvent& event);

    // must use a ConstPtr callback to use zero-copy transport
    //void messageCb(const std_msgs::StringConstPtr message);

    ros::Subscriber sub_;
    ros::Publisher pub_;
    ros::Timer timer_;
};


#endif //PARSIAN_AGENT_AGENTNODELET_H
