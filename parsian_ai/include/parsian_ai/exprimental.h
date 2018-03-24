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
    KickAction *myKick2;

    Exprimental(Agent** _agents) {
        agents = _agents;
        gpa = new GotopointavoidAction();
        pos  << Vector2D(-3,4.5) << Vector2D(1,3) << Vector2D(4.5,3) << Vector2D(1,3);
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
        myKick->setTarget(Vector2D{-3, 4.5});
        myKick->setChip(false);
        myKick->setKickspeed(2);
        for (int i = 0 ; i < wm->opp.activeAgentsCount() ; i++)
            if(wm->opp[i]->pos.y - agents[1]->pos().y < 0.8 && fabs(wm->opp[i]->pos.x - agents[1]->pos().x < 0.8))
            {
                myKick->setChip(true);
                myKick->setChipdist(0);
            }

        agents[1]->action = myKick;
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
