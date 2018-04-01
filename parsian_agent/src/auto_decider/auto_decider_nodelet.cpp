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
    int robot_id;
    float robot_x;
    float robot_y;
    float ball_y;
    float ball_x;
    int vision_sensor;
    int decide_array[100];
    int sum;
    int decide_1;
    int counter_1;
    bool robot_sensor;
    bool decide;
    bool final_decide;
    bool getCbs;
    bool gotstat;
private:
    virtual void onInit()
    {
        counter_1=1;
        robot_sensor=0;
        decide=1;
        final_decide=1;
        getCbs = true;
        gotstat = false;
        ros::NodeHandle& private_nh = getPrivateNodeHandle();
        ros::NodeHandle& nh = getNodeHandle();
        pub = private_nh.advertise<parsian_msgs::parsian_robot_fault>("/autofault",5);
        sub1 = nh.subscribe("/robots_status",100, &Decider::statCb, this);
        sub3 = nh.subscribe("/world_model",100, &Decider::wmCb, this);
    }
    void statCb(const parsian_msgs::parsian_robots_status msg)
    {
        if(getCbs)
        {
            statmsg = msg;
            gotstat = true;
        }
    }

    void wmCb(const parsian_msgs::parsian_world_model msg)
    {
        if(getCbs && gotstat)
        {
            wmmsg = msg;
            getCbs = false;
            gotstat = false;
            faultdetect();
        }
    }
    void faultdetect()
    {
    	for(int ee{}; ee < statmsg.status.size(); ee++)
        {
        	robot_id=statmsg.status[ee].id;
                robot_sensor=statmsg.status[ee].shootSensor;
                if(statmsg.status[ee].battery<=3||statmsg.status[ee].kickFault||statmsg.status[ee].chipFault||statmsg.status[ee].m1Fault
                        ||statmsg.status[ee].m1Fault||statmsg.status[ee].m2Fault||statmsg.status[ee].m3Fault||statmsg.status[ee].En4Fault
                        ||statmsg.status[ee].En1Fault||statmsg.status[ee].En2Fault||statmsg.status[ee].En3Fault||statmsg.status[ee].En4Fault||statmsg.status[ee].shootBoardFault)
                    decide=0;
        
                parsian_msgs::parsian_robot mrobot;
                for(auto robot : wmmsg.our)
                    if(robot.id == robot_id)
                        mrobot = robot;
                robot_x = mrobot.pos.x;
                robot_y = mrobot.pos.y;
        
                ball_x=wmmsg.ball.pos.x;
                ball_y=wmmsg.ball.pos.y;
                if(sqrt((robot_x - ball_x)*(robot_x - ball_x) + (robot_y - ball_y)*(robot_y - ball_y)) < threshold )
                {
                    vision_sensor=1;
                    //ROS_INFO_STREAM("vision nearball: "<<sqrt((robot_x - ball_x)*(robot_x - ball_x) + (robot_y - ball_y)*(robot_y - ball_y)));
                }
                else
                {
                    vision_sensor=0;
                    //ROS_INFO_STREAM("vision not nearball: "<<sqrt((robot_x - ball_x)*(robot_x - ball_x) + (robot_y - ball_y)*(robot_y - ball_y)));
                }
                if(!vision_sensor && robot_sensor)
                    decide=0;
                if(counter_1> buffer_size ){
                    decide_1=decide;
                    sum=0;
                    for(int aa=1;aa<=buffer_size;aa++)
                        sum=decide_array[aa]+sum;
                    if(sum<=buffer_size/2)
                        final_decide=0;
                    else
                        final_decide=1;
                    for(int counter_2=1;counter_2<buffer_size;counter_2++)
                        decide_array[counter_2]=decide_array[counter_2+1];
                    decide_array[buffer_size]=decide_1;
                }
                else
                {
                    decide_array[counter_1]=decide;
                    counter_1++;
                }
                if(final_decide == 0)
                {
                    res.robot_id = robot_id;
                    pub.publish(res);
                    res.select = 2;
                    //ROS_INFO("Message published ");
                }
                if(final_decide == 1)
                {
                    res.robot_id = robot_id;
                    res.select = 0;
                    pub.publish(res);
                    //ROS_INFO("Message published ");
                }
        
                decide=1;
                robot_id = -1;
            }
        getCbs = true;
    }
};
}
PLUGINLIB_DECLARE_CLASS(auto_decider,Decider,auto_decider::Decider, nodelet::Nodelet)

