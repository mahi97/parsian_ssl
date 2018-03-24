//
// Created by parsian-ai on 3/23/18.
//

#ifndef PARSIAN_AI_EXPRIMENTAL_H
#define PARSIAN_AI_EXPRIMENTAL_H


#include <QStringList>

#include <algorithm>

#include <QtCore/QMap>
#include <parsian_util/core/agent.h>
#include <QtCore/QTime>
#include <QtCore/QFile>
#include <QPair>
#include <QTextStream>
#include "parsian_util/base.h"
#include <parsian_ai/util/worldmodel.h>
#include <parsian_ai/plans/plans.h>
#include <parsian_ai/plays/plays.h>
#include <parsian_ai/roles/stop.h>

#include <parsian_util/action/autogenerate/gotopointaction.h>
#include <parsian_util/action/autogenerate/gotopointavoidaction.h>
#include <parsian_util/action/autogenerate/kickaction.h>
#include <parsian_util/geom/geom.h>
#include <behavior/direct/direct.h>

#include <parsian_msgs/plan_service.h>

class Exprimental {
public:
    GotopointavoidAction *mygpa;
    KickAction *myKick;
    Exprimental(Agent** _agents) {
        agents = _agents;
        gpa = new GotopointavoidAction();
        pos  << Vector2D(1,-3) << Vector2D(1,3) << Vector2D(4.5,3) << Vector2D(1,3);
        dist << 0.3 << 1 << 0.3 << 0.3;
        agents[8]->action = gpa;
        state = 0;
        mygpa = new GotopointavoidAction();
        myKick = new KickAction;
        direct = new BehaviorDirect;
        for (int i = 0; i < 12; i++) if (agents[i] != nullptr) agentList.append(_agents[i]);

    }
    ~Exprimental() {

    }
    void reset() {

    }
    void execute() {
        ROS_INFO("adsfasdf");
        direct->init(agentList, m_behav);
        direct->execute();
    }
private:
    int state;
    QList<Vector2D> pos;
    QList<double> dist;
    QList<Agent*> agentList;
    GotopointavoidAction *gpa;
    Agent** agents;
    BehaviorDirect* direct;
    parsian_msgs::parsian_behaviorConstPtr m_behav;


};

#endif //PARSIAN_AI_EXPRIMENTAL_H
