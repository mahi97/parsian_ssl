#include <parsian_communication/communicator.h>
#include <string.h>
#include <stdio.h>


/*************************************** CBaseCommunicator Class ******************************************/

CBaseCommunicator::CBaseCommunicator()
{
    serial_open= false;
    error      = false;
    p = new CMySerialPort();
    p->serial_port = NULL;
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
    if( p->serial_port->open(QIODevice::ReadWrite) == true ){
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
    if (p->serial_port==NULL) return;
    p->serial_port->close();
    delete p->serial_port;
    p->serial_port = NULL;
    serial_open = false;
}

bool CBaseCommunicator::errorOccured()     {return error;}
bool CBaseCommunicator::isSerialConnected(){return serial_open;}

///////////////////////////////////// communicator
CCommunicator::CCommunicator() : CBaseCommunicator()
{
}

CCommunicator::~CCommunicator()
{
}

void CCommunicator::packetCallBack(const parsian_msgs::parsian_packetsConstPtr &_packet)
{
    char* tempStr;
    char test[100];
    for(int i = 0 ; i < _packet->value.size() ; i ++)
    {
        tempStr = new char[_packet->value.at(i).packets.size()];
        for(int j = 0 ; j < _packet->value.at(i).packets.size() ; j++)
        {
            tempStr[j] = _packet->value.at(i).packets.at(j);
        }
        sprintf(test,"packet :%d",_packet->value.at(i).packets.size());
        ROS_INFO(test);
        tempStr[0] = 0x99;

        sendString(tempStr,_packet->value.at(i).packets.size());

        delete tempStr;
    }
}

void CCommunicator::sendString(const char* s,int len)
{
    bool flag = false;

    if (true /*use_serial && serial_open*/)
    {
        for(int i = 0 ; i < len ; i++)
        {
            p->serial_port->write(s + i,1);
        }
    }
}

void CCommunicator::sendByte(char c)
{
    char ch = c;
    sendString(&ch,1);
}

