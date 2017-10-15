//
// Created by parsian-ai on 10/6/17.
//

#include <parsian_communication/packet_generate/packet_nodelet.h>

PLUGINLIB_EXPORT_CLASS(parsian_packet::PacketNodelet, nodelet::Nodelet)

using namespace parsian_packet;


void PacketNodelet::onInit() {


    ros::NodeHandle& n = getNodeHandle();
    ros::NodeHandle& private_n = getPrivateNodeHandle();

    timer = n.createTimer(ros::Duration(.062), boost::bind(&PacketNodelet::timerCb, this, _1));

    drawer = new Drawer();
    debugger = new Debugger();

    drawPub    = private_n.advertise<parsian_msgs::parsian_draw>("/draws",1000);
    debugPub   = private_n.advertise<parsian_msgs::parsian_debugs>("/debugs",1000);
    packetPub   = private_n.advertise<parsian_msgs::parsian_packets>("/packets",1000);

    ros::Subscriber robotPacketSub0   = n.subscribe("/robot_command0"  , 1000, &PacketNodelet::callBack, this);
    ros::Subscriber robotPacketSub1   = n.subscribe("/robot_command1"  , 1000, &PacketNodelet::callBack, this);
    ros::Subscriber robotPacketSub2   = n.subscribe("/robot_command2"  , 1000, &PacketNodelet::callBack, this);
    ros::Subscriber robotPacketSub3   = n.subscribe("/robot_command3"  , 1000, &PacketNodelet::callBack, this);
    ros::Subscriber robotPacketSub4   = n.subscribe("/robot_command4"  , 1000, &PacketNodelet::callBack, this);
    ros::Subscriber robotPacketSub5   = n.subscribe("/robot_command5"  , 1000, &PacketNodelet::callBack, this);
    ros::Subscriber robotPacketSub6   = n.subscribe("/robot_command6"  , 1000, &PacketNodelet::callBack, this);
    ros::Subscriber robotPacketSub7   = n.subscribe("/robot_command7"  , 1000, &PacketNodelet::callBack, this);
    ros::Subscriber robotPacketSub8   = n.subscribe("/robot_command8"  , 1000, &PacketNodelet::callBack, this);
    ros::Subscriber robotPacketSub9   = n.subscribe("/robot_command9"  , 1000, &PacketNodelet::callBack, this);
    ros::Subscriber robotPacketSub10  = n.subscribe("/robot_command10" , 1000, &PacketNodelet::callBack, this);
    ros::Subscriber robotPacketSub11  = n.subscribe("/robot_command11" , 1000, &PacketNodelet::callBack, this);


    ros::Subscriber visinSub  = n.subscribe("/vision_detection" , 1000, &PacketNodelet::syncData, this);

    for(int i = 0 ; i < _MAX_ROBOT_NUM ; i ++ )
    {
        for (int j = 0 ; j < _ROBOT_PACKET_SIZE ; j ++ )
        {
            robotPackets[i][j] = 0;
        }
    }
    visionCounter= 0;
}

void PacketNodelet::callBack(const parsian_msgs::parsian_robot_commandConstPtr &_packet) {

    unsigned char outputBuffer[_ROBOT_PACKET_SIZE] = {0};
    unsigned char robotId = _packet->robot_id;
    unsigned char kickSpeed = _packet->kickSpeed;

    outputBuffer[0] = 0x99;
    outputBuffer[1] = robotId & 0x0F;
    int kickNumber = round(kickSpeed);//*160.0;
    if (roller != 0)
        outputBuffer[1] = outputBuffer[1] | ((roller & 0x07) << 4);
    outputBuffer[2] = kickNumber & 0x7F;
    outputBuffer[3] = (kickNumber >> 7) & 0x07;

    double ang=-dir().th().radian();

    //setting BU for kalman
    self()->kalman_velocs.vx = (vforward*cos(ang)) + (vnormal*sin(ang));
    self()->kalman_velocs.vy = -(vforward*sin(ang)) + (vnormal*cos(ang));
    self()->kalman_velocs.vw = vangular;

    int vforwardI = floor(vforward * 487.0);
    int vnormalI  = floor(vnormal * 487.0);
    int vangularI  = ((double)vangular) * ((double)256.0 / (double) 360.0);

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

    requestBit = true; // change to release mode command
    if (requestBit && knowledge->getGameState() != CKnowledge::Halt)
    {
        outputBuffer[8] |= 0x01;
    }

    if (chip)
        outputBuffer[8] = outputBuffer[8] | 0x02;
    if (vangularI < 0)
        outputBuffer[8] = outputBuffer[8] | 0x04;
    outputBuffer[8] = outputBuffer[8] | (((vnormalAbs >> 7) & 0x0F) << 3);
    //    outputBuffer[9] =  (int)(_ACC*5) & (0x7F);
    //    outputBuffer[10] = (int)(_DEC*5) & (0x7F);



    unsigned int velTanSend = (int)(fabs(veltan)*100);

    unsigned int velNormSend = (int)(fabs(velnorm)*100);
    outputBuffer[11] = packetNum & 0x07;
    outputBuffer[12] = ((velTanSend >> 6 ) & 0X07) | ((velNormSend & 0x0F) << 3);
    outputBuffer[13] = (velNormSend >> 4) & 0x1F;

    if(veltan < 0)
        outputBuffer[11] |= 0x40;
    else
        outputBuffer[11] &= 0xBF;

    if( velnorm < 0)
        outputBuffer[13] |= 0x40;
    else
        outputBuffer[13] &= 0xBF;

}

void PacketNodelet::syncData(const parsian_msgs::ssl_vision_detectionConstPtr &_packet) {

    if(visionCounter < 8 )
    {
        visionCounter ++;
    }
    else
    {
        visionCounter = 0;
    }
}

void PacketNodelet::timerCb(const ros::TimerEvent &event) {
    if (drawer != nullptr)
        drawPub.publish(drawer->draws);
    if (debugger != nullptr)
        debugPub.publish(debugger->debugs);



}
