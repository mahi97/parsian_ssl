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

    QByteArray datagram(buf.c_str(), static_cast<int>(buf.length()));
    ROS_INFO_STREAM(socket->writeDatagram(datagram, host, static_cast<quint16>(port)));

}

UDPSend::~UDPSend() = default;
