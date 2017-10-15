#include "parsian_protobuf_wrapper/common/net/udpsend.h"

UDPSend::UDPSend(std::__cxx11::string address, int _port)// : QObject(parent)
{
    QString add(address.c_str());
    host.setAddress(add);
    port = _port;
    socket = new QUdpSocket();
    socket->bind(host, port);

}

void UDPSend::send(std::string buf)
{

    /////////////////////////////////////////////////////////////////////////////////////////////
    ROS_INFO("sending data over udp");
    /////////////////////////////////////////////////////////////////////////////////////////////
    QByteArray datagram(buf.c_str(), buf.length());
    ROS_INFO("sending byte: %lld", socket->writeDatagram(datagram, host, port));

}



UDPSend::~UDPSend()
{

}
