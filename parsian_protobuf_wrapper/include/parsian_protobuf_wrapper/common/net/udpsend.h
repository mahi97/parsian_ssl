#ifndef UDPSEND_H
#define UDPSEND_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QString>
#include <QByteArray>
#include "ros/ros.h"


class UDPSend// : public QObject
{
   // Q_OBJECT
public:
    explicit UDPSend(std::string address, int _port);
    ~UDPSend();
    void send(std::__cxx11::string buf);

private:
    QUdpSocket* socket;
    QHostAddress host;
    int port;



};

#endif // UDPSEND_H
