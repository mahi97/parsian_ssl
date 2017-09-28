//
// Created by parsian-ai on 9/22/17.
//

#ifndef PARSIAN_AI_AI_H
#define PARSIAN_AI_AI_H

#include <parsian_msgs/parsian_world_model.h>
#include <parsian_msgs/parsian_robot.h>
#include <parsian_util/core/worldmodel.h>
#include <parsian_util/knowledge.h>
#include <parsian_ai/soccer.h>

class AI {
public:
    AI();
    ~AI();

    void execute();

    void updateWM(const parsian_msgs::parsian_world_model&);
    void updateRobotStatus(const parsian_msgs::parsian_robot&);

private:

    CWorldModel* wm;
    CKnowledge* knowledge;
    CSoccer*    soccer;

protected:

};

#endif //PARSIAN_AI_AI_H
