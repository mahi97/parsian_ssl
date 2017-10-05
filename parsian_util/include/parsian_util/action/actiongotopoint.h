//
// Created by parsian-ai on 9/29/17.
//

#ifndef PARSIAN_UTIL_ACTIONGOTOPOINT_H
#define PARSIAN_UTIL_ACTIONGOTOPOINT_H

#include <parsian_util/action/action.h>
#include <parsian_msgs/gotoPoint.h>
#include <parsian_util/geom/geom.h>

class ActionGotoPoint : public Action {
public:
    ActionGotoPoint();
    virtual ~ActionGotoPoint();

    virtual void getParsianMessage();

    parsian_msgs::gotoPoint data;
protected:

private:

};

#endif //PARSIAN_UTIL_ACTIONGOTOPOINT_H
