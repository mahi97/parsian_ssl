#include <pluginlib/class_list_macros.h>
#include <nodelet/nodelet.h>
#include <ros/ros.h>
#include <math.h>
#include <stdio.h>
#include <parsian_msgs/parsian_robots_status.h>
#include <parsian_msgs/parsian_robot.h>
#include <parsian_msgs/parsian_world_model.h>
#include <parsian_msgs/vector2D.h>
#include <parsian_msgs/parsian_robot_fault.h>
# define buffer_size  60
# define threshold 0.25

//TODOS
//1)select what state the damaged robot is
//2)what kick(or what ever)fault = true means (dameged or not)
//3)whats the best number for threshold and buffersize

namespace auto_decider
{
class Decider : public nodelet::Nodelet
{
public:
    parsian_msgs::parsian_robot_fault res;
    parsian_msgs::parsian_robots_status statmsg;
    parsian_msgs::parsian_world_model wmmsg;
    ros::Publisher pub;
    ros::Subscriber sub1,sub2,sub3;
    bool shootSensors[12];
    bool isNear[12];
private:
    virtual void onInit()
    {

        ros::NodeHandle& private_nh = getPrivateNodeHandle();
        ros::NodeHandle& nh = getNodeHandle();
        pub = private_nh.advertise<parsian_msgs::parsian_robot_fault>("/autofault",5);
        sub1 = nh.subscribe("/robots_status",100, &Decider::statusCb, this);
        sub3 = nh.subscribe("/world_model",100, &Decider::wmCb, this);
    }
    void statusCb(const parsian_msgs::parsian_robots_status msg)
    {
        for(int i=0; i<12;i++)
            shootSensor[i] = msg.status[i].shootSensor;
    }

    void wmCb(const parsian_msgs::parsian_world_model msg)
    {
        for(int i=0; i<12;i++)
            isNear[i] = Vector2D(msg.our[i].pos).dist(msg.ball.pos);
    }
    void faultdetect()
    {

};
}
PLUGINLIB_DECLARE_CLASS(auto_decider,Decider,auto_decider::Decider, nodelet::Nodelet)

