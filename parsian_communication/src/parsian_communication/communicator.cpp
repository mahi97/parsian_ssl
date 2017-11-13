#include <parsian_communication/communicator.h>
#include <string.h>
#include <stdio.h>
#include <parsian_util/tools/drawer.h>


/*************************************** CBaseCommunicator Class ******************************************/

CBaseCommunicator::CBaseCommunicator()
{
    serial_open= false;
    error      = false;
    p = new CMySerialPort();
    p->serial_port = nullptr;
    PortSettings settings = {BAUD115200, DATA_8, PAR_NONE, STOP_1, FLOW_OFF,40};
//    recTime = new QTimer;
//    recTime->setInterval(100);
//    recTime->start();
    recDataFlow.clear();
    for(int i = 0 ; i < 16 ; i++)
    {
        for(int j = 0 ; j < 13 ; j ++)
        {
            robotPacket[i][j]=0;
        }
    }

    for(int i = 0 ; i < 12 ; i++)
    {
        onlineRobotsTimer[i].start();
    }
}


void CBaseCommunicator::readData()
{

    QByteArray dataFlow;
    if(p->serial_port->bytesAvailable())
        dataFlow = p->serial_port->read(p->serial_port->bytesAvailable());
    p->serial_port->flush();
    recDataFlow.append(dataFlow);
    if(recDataFlow.size() > 100)
    {
        recDataFlow.remove(0,recDataFlow.size()/2);
    }

//    debug(QString("SIZE : %1").arg(recDataFlow.size()), D_MHMMD);
    if(recDataFlow.size())
    {

        for(int i = 0 ; i < recDataFlow.size() ; i++) {
            if(recDataFlow[i] == 0x99)
            {

                if(i >= 12)
                {

                    for(int j = 0 ; j <= 12 ; j++)
                    {
                        if(i-12 < 12) {
                            robotPacket[recDataFlow[i-12]][j] = static_cast<unsigned char>(recDataFlow[j + i - 12]);
                            onlineRobotsTimer[recDataFlow[i-12]].restart();
                        }
                    }
                    if (!recDataFlow.isEmpty())
                    {
                        recDataFlow.remove(0,i+1);
                        i = 0;
                    }

                }
                else
                {
                    if (!recDataFlow.isEmpty())
                    {
                        recDataFlow.remove(0,i+1);
                        i =0;
                    }
                }

            }
        }
    }

//    knowledge->onlineRobots.clear();
//    for(int i = 0 ; i < 12 ; i++)
//    {
//        knowledge->onlineRobots.append(i);
//        if(onlineRobotsTimer[i].elapsed() > 200)
//        {
//            knowledge->onlineRobots.removeOne(i);
//        }
//    }

//    debug(QString("id : %1").arg((int)robotPacket[4][1]), D_MHMMD);
//
//    for (int i = 0; i < _MAX_NUM_PLAYERS; i++) {
//        CAgent* tempAgent = knowledge->getAgent(i);
//        tempAgent->setShootSensor(robotPacket[i][1] & 0x01);
//
//        tempAgent->changeIsNeeded         = robotPacket[i][4] & 0b11000000;
//
//        tempAgent->status.battery         = robotPacket[i][1] & 0b11111100;
//        tempAgent->status.capCharge       = robotPacket[i][2] & 0b00111111;
//        tempAgent->status.dataLost        = robotPacket[i][4] & 0b00001111;
//        tempAgent->status.spin            = robotPacket[i][1] & 0b00000010;
//        tempAgent->status.shotSensor      = robotPacket[i][1] & 0b00000001;
//        tempAgent->status.fault           = robotPacket[i][4] & 0b10000000;
//        tempAgent->status.faild           = robotPacket[i][4] & 0b01000000;
//        tempAgent->status.halt            = robotPacket[i][4] & 0b00100000;
//        tempAgent->status.shotBoard       = robotPacket[i][5] & 0b01000000;
//        tempAgent->status.kickFault       = robotPacket[i][5] & 0b00100000;
//        tempAgent->status.chipFault       = robotPacket[i][5] & 0b00010000;
//        tempAgent->status.encoderFault[0] = robotPacket[i][5] & 0b00001000;
//        tempAgent->status.encoderFault[1] = robotPacket[i][5] & 0b00000100;
//        tempAgent->status.encoderFault[2] = robotPacket[i][5] & 0b00000010;
//        tempAgent->status.encoderFault[3] = robotPacket[i][5] & 0b00000001;
//        tempAgent->status.motorFault[0]   = robotPacket[i][6] & 0b00000001;
//        tempAgent->status.motorFault[1]   = robotPacket[i][6] & 0b00000010;
//        tempAgent->status.motorFault[2]   = robotPacket[i][6] & 0b00000100;
//        tempAgent->status.motorFault[3]   = robotPacket[i][6] & 0b00001000;
//        tempAgent->status.motorFault[4]   = robotPacket[i][6] & 0b00010000;
//        tempAgent->status.beep            = robotPacket[i][6] & 0b00100000;
//        tempAgent->status.shotSensorFault = robotPacket[i][6] & 0b01000000;
//
//        tempAgent->status.boardID         = robotPacket[i][3] & 0b00011111;
//    }

    if(robotPacket[0][1] & 0x01)
    {
        drawer->draw(Vector2D(0,0));
    }
}

CBaseCommunicator::~CBaseCommunicator()
{
    closeSerial();
    delete p;
}

void CBaseCommunicator::setSerialParams(unsigned int baud, unsigned int charsize, unsigned int parity, short stopbits)
{
    p->setSerialParams(baud, charsize, parity, stopbits);
}


void CBaseCommunicator::connectSerial(const char* port)
{
    closeSerial();
    _port = QString(port);
    p->setSerialParams(115200,8,0,1);

    p->serial_port = new QextSerialPort(p->portSettings);
    p->serial_port->setPortName(_port);
    if(p->serial_port->open(QIODevice::ReadWrite)){
        serial_open = true;
        ROS_INFO("successfully");
//        connect(recTime,SIGNAL(timeout()),this,SLOT(readData()));
    }
    else{
        ROS_ERROR("Error occured, comunicator connection failed");
        err = QString("Error occured, comunicator connection failed");
        serial_open = false;
        error = true;
        return;
    }
}

void CBaseCommunicator::closeSerial()
{
    if (p->serial_port == nullptr) return;
    p->serial_port->close();
    delete p->serial_port;
    p->serial_port = nullptr;
    serial_open = false;
}

bool CBaseCommunicator::errorOccured()     {return error;}
bool CBaseCommunicator::isSerialConnected(){return serial_open;}

///////////////////////////////////// communicator
CCommunicator::CCommunicator() : CBaseCommunicator()
{
}

CCommunicator::~CCommunicator() = default;

void CCommunicator::packetCallBack(const parsian_msgs::parsian_packetsConstPtr &_packet)
{
    char* tempStr;
    char test[100];
    for (const auto &robotPacket : _packet->value) {
        tempStr = new char[robotPacket.packets.size()];
        for(int j = 0 ; j < robotPacket.packets.size() ; j++)
        {
            tempStr[j] = robotPacket.packets.at(j);
        }
        sprintf(test,"packet :%lu", robotPacket.packets.size());
        ROS_INFO_STREAM(test);
        tempStr[0] = static_cast<char>(0x99);

        sendString(tempStr, static_cast<int>(robotPacket.packets.size()));

        delete tempStr;
    }
}

void CCommunicator::sendString(const char* s,int len)
{
    bool flag = false;

    for(int i = 0 ; i < len ; i++)
    {
        p->serial_port->write(s + i,1);
    }
}

void CCommunicator::sendByte(char c)
{
    char ch = c;
    sendString(&ch,1);
}