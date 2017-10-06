//
// Created by parsian-ai on 9/22/17.
//

#ifndef PARSIAN_AI_AI_H
#define PARSIAN_AI_AI_H

#include <parsian_msgs/parsian_world_model.h>
#include <parsian_msgs/ssl_refree_wrapper.h>
#include <parsian_msgs/parsian_robot.h>
#include <parsian_util/core/worldmodel.h>
#include <parsian_util/knowledge.h>
#include <parsian_ai/soccer.h>

class AI {
public:
    AI();
    ~AI();

    void execute();

    static void updateWM(const parsian_msgs::parsian_world_model&);
    static void updateRobotStatus(const parsian_msgs::parsian_robot&);
    static void updateReferee(const parsian_msgs::ssl_refree_wrapper&);

    void publish(std::vector<ros::Publisher*> publishers);

    parsian_msgs::parsian_debugs getDebugs() { return debugger->debugs; }
    parsian_msgs::parsian_draw   getDraw()   { return drawer->draws; }

private:

    CSoccer*    soccer;

protected:

};

#endif //PARSIAN_AI_AI_H
