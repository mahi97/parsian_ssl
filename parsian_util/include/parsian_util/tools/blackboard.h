//
// Created by ali on 3/12/18.
//

#ifndef PARSIAN_UTIL_BLACKBOARD_H
#define PARSIAN_UTIL_BLACKBOARD_H

#include <ros/ros.h>

#define MONITOR(var , node) \
    if (typeof(var) == int )
        ROS_DEBUG_STREAM(node << "::" << #var <<" = "<<var )

#endif //PARSIAN_UTIL_BLACKBOARD_H
