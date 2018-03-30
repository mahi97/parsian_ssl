//
// Created by ali on 3/12/18.
//

#ifndef PARSIAN_UTIL_BLACKBOARD_H
#define PARSIAN_UTIL_BLACKBOARD_H

#include <ros/ros.h>
#include <QVector2D>

#define D_ERROR      0
#define D_GAME       1
#define D_EXPERIMENT 2
#define D_DEBUG      4
#define D_NADIA      8
#define D_MANI       16
#define D_ARASH      32
#define D_ALI        64
#define D_SEPEHR     128
#define D_MASOOD     256
#define D_MOHAMMED   512
#define D_HOSSEIN    1024
#define D_KK         2048
#define D_MHMMD      4096
#define D_ERF        8192
#define D_MAHI       16384
#define D_MAHMOOD    32768
#define D_FATEME     32769
#define D_ATOUSA     32770
#define D_AHZ        32772
#define D_AMIN       32776
#define D_PARSA      32784
#define D_HAMED      32800

#define PDEBUG(prefix, value, type) MONITOR(prefix,value,type)
#define NDEBUG(value, type) MONITOR(#value,value,type)
#define PDEBUGV2D(prefix, value, type) MONITOR_V2D(prefix,value,type)
#define NDEBUGV2D(value, type) MONITOR_V2D(#value,value,type)
#define DEBUG(value, type) MONITOR("",value,type)

#define MONITOR_V2D(prefix,var) \
    ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Debug);\
    ROS_DEBUG_STREAM_NAMED(prefix, " = x:"<<var.x()<<"  y:"<<var.y());\
    ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Info);

#define MONITOR(prefix,var,type) \
    ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Debug);\
    ROS_DEBUG_STREAM_NAMED(QString("::%1::").arg(type).toStdString(),\
                           (prefix)<<" = "<<QString("%1").arg(var).toStdString()); \
    ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Info);

#endif //PARSIAN_UTIL_BLACKBOARD_H
