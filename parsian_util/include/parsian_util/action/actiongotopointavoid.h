//
// Created by parsian-ai on 9/29/17.
//

#ifndef PARSIAN_UTIL_ACTIONGOTOPOINTAVOID_H
#define PARSIAN_UTIL_ACTIONGOTOPOINTAVOID_H

#include <parsian_util/action/action.h>
#include <parsian_msgs/gotoPointAvoid.h>

class ActionGotoPointAvoid : public Action {
public:
    parsian_msgs::gotoPointAvoid data;
protected:
private:
};

#endif //PARSIAN_UTIL_ACTIONGOTOPOINTAVOID_H
