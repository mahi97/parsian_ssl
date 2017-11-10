//
// Created by parsian-ai on 10/6/17.
//

#include <parsian_communication/packet_generate/packet_nodelet.h>
#include <parsian_msgs/parsian_world_model.h>
PLUGINLIB_EXPORT_CLASS(parsian_packet::PacketNodelet, nodelet::Nodelet)

using namespace parsian_packet;


void PacketNodelet::onInit() {


    ros::NodeHandle& n = getNodeHandle();
    ros::NodeHandle& private_n = getPrivateNodeHandle();
    timer = n.createTimer(ros::Duration(.062), boost::bind(&PacketNodelet::timerCb, this, _1));

    drawer = new Drawer();
    debugger = new Debugger();

    drawPub    = n.advertise<parsian_msgs::parsian_draw>("draws",1000);
    debugPub   = n.advertise<parsian_msgs::parsian_debugs>("debugs",1000);
    packetPub  = n.advertise<parsian_msgs::parsian_packets>("packets",1000);

    robotPacketSub0   = n.subscribe("robot_command0"  , 10000, &PacketNodelet::callBack, this);
    robotPacketSub1   = n.subscribe("robot_command1"  , 10000, &PacketNodelet::callBack, this);
    robotPacketSub2   = n.subscribe("robot_command2"  , 10000, &PacketNodelet::callBack, this);
    robotPacketSub3   = n.subscribe("robot_command3"  , 10000, &PacketNodelet::callBack, this);
    robotPacketSub4   = n.subscribe("robot_command4"  , 10000, &PacketNodelet::callBack, this);
    robotPacketSub5   = n.subscribe("robot_command5"  , 10000, &PacketNodelet::callBack, this);
    robotPacketSub6   = n.subscribe("robot_command6"  , 10000, &PacketNodelet::callBack, this);
    robotPacketSub7   = n.subscribe("robot_command7"  , 10000, &PacketNodelet::callBack, this);
    robotPacketSub8   = n.subscribe("robot_command8"  , 10000, &PacketNodelet::callBack, this);
    robotPacketSub9   = n.subscribe("robot_command9"  , 10000, &PacketNodelet::callBack, this);
    robotPacketSub10  = n.subscribe("robot_command10" , 10000, &PacketNodelet::callBack, this);
    robotPacketSub11  = n.subscribe("robot_command11" , 10000, &PacketNodelet::callBack, this);


    visinSub  = n.subscribe("world_model" , 1000, &PacketNodelet::syncData, this);

    for(int i = 0 ; i < _MAX_ROBOT_NUM ; i ++ )
    {
        for (int j = 0 ; j < _ROBOT_PACKET_SIZE ; j ++ )
        {
            robotPackets[i][j] = 255;
        }
    }
    visionCounter= 0;

}

void PacketNodelet::callBack(const parsian_msgs::parsian_robot_commandConstPtr &_packet) {

    ROS_INFO("miad inja");
    unsigned char outputBuffer[_ROBOT_PACKET_SIZE] = {0};
    unsigned char robotId = _packet->robot_id;
    int kickNumber = round(_packet->kickSpeed);

    outputBuffer[0] = 0x99;
    outputBuffer[1] = robotId & 0x0F;
    if (_packet->roller_speed != 0)
        outputBuffer[1] = outputBuffer[1] | ((_packet->roller_speed & 0x07) << 4);
    outputBuffer[2] = kickNumber & 0x7F;
    outputBuffer[3] = (kickNumber >> 7) & 0x07;

    int vforwardI = floor(_packet->vel_F * 487.0);
    int vnormalI  = floor(_packet->vel_N * 487.0);
    int vangularI  = (_packet->vel_w) * ((double)256.0 / (double) 360.0);

    if (vforwardI > 2047) vforwardI = 2047;
    if (vforwardI < -2047) vforwardI = -2047;

    if (vnormalI > 2047) vnormalI = 2047;
    if (vnormalI < -2047) vnormalI = -2047;

    if (vangularI > 2047) vangularI = 2047;
    if (vangularI < -2047) vangularI = -2047;

    int vangularAbs = abs(vangularI);
    int vforwardAbs = abs(vforwardI);
    int vnormalAbs  = abs(vnormalI);

    outputBuffer[3] = (((vangularAbs >> 7) & 0x0F) << 3) | outputBuffer[3];

    outputBuffer[4] = vforwardAbs & 0x7F;
    outputBuffer[5] = vnormalAbs & 0x7F;
    outputBuffer[6] = (vforwardAbs >> 7) & 0x0F;
    if (vforwardI < 0) outputBuffer[6] = outputBuffer[6] | 0x10;
    if (vnormalI < 0) outputBuffer[6] = outputBuffer[6] | 0x20;

    outputBuffer[7] = vangularAbs & 0x7F;

    if (_packet->release)
    {
        outputBuffer[8] |= 0x01;
    }

    if (_packet->chip)
        outputBuffer[8] = outputBuffer[8] | 0x02;
    if (vangularI < 0)
        outputBuffer[8] = outputBuffer[8] | 0x04;
    outputBuffer[8] = outputBuffer[8] | (((vnormalAbs >> 7) & 0x0F) << 3);


    outputBuffer[11] = visionCounter & 0x07;
    outputBuffer[12] = 0;
    outputBuffer[13] = 0;

    for(int k = 0 ; k < _ROBOT_PACKET_SIZE ; k++)
    {

        robotPackets[robotId][k] = outputBuffer[k];
    }

}

void PacketNodelet::syncData(const parsian_msgs::parsian_world_modelConstPtr &_packet) {

    if(visionCounter < 8 )
    {
        visionCounter ++;
    }
    else
    {
        visionCounter = 0;
    }

    robotPacks.value.clear();
    parsian_msgs::parsian_robot_packet temp;
    bool validPack = false;
    for(int k = 0 ; k < _MAX_ROBOT_NUM ; k++)
    {
        temp.packets.clear();
        validPack = false;
        for (int i = 0 ; i < _ROBOT_PACKET_SIZE ; i ++)
        {
            if(robotPackets[k][i] != 255)
            {
                temp.packets.push_back(robotPackets[k][i]);
                validPack = true;
            }
        }
        if(validPack)
        {
            robotPacks.value.push_back(temp);
        }
    }

    packetPub.publish(robotPacks);
}

void PacketNodelet::timerCb(const ros::TimerEvent &event) {
    if (drawer != nullptr){
        drawPub.publish(drawer->draws);
        ROS_INFO_STREAM("paket draw "<<drawer);
    }
    if (debugger != nullptr)
        debugPub.publish(debugger->debugs);



}
