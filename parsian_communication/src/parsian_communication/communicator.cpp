#include <parsian_communication/communicator.h>
#include <string.h>
#include <stdio.h>


/*************************************** CBaseCommunicator Class ******************************************/

CBaseCommunicator::CBaseCommunicator()
{
    serial_open= false;
    error      = false;
    p = new CMySerialPort();
    p->serial_port = nullptr;
    PortSettings settings = {BAUD115200, DATA_8, PAR_NONE, STOP_1, FLOW_OFF,40};
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
        ROS_INFO(test);
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