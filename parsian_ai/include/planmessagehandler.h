//
// Created by fateme on 11/12/17.
//

#ifndef PARSIAN_AI_PALNMESSAGEHANDLER_H
#define PARSIAN_AI_PALNMESSAGEHANDLER_H

#include <parsian_msgs/parsian_ai_plan_request.h>
#include <parsian_msgs/parsian_plan.h>

class PlanMessage {
public:
    void setPlanMessage(parsian_msgs::parsian_plan _msg);

    parsian_msgs::parsian_plan getPlanMessage();
};

class PlanRequestMessage {
public:
    void setPlanRequestMessage(parsian_msgs::parsian_ai_plan_request _msg);

    parsian_msgs::parsian_ai_plan_request getPlanRequest();
};

#endif //PARSIAN_AI_PALNMESSAGEHANDLER_H
