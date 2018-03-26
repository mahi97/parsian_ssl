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

#include <parsian_msgs/plan_service.h>

enum class State{
    SIMPLE_MOVE = 0,
    WAITFORIT = 1,
    CHIP = 2,
    GOCURVE = 3,
    CATCHBALL = 4
};

class Exprimental {
public:
    GotopointavoidAction *mygpa;
    KickAction *myKick;
        agents = _agents;
        myKick = new KickAction;
    }
    ~Exprimental() {

    }
    void reset() {

    }
    void execute() {
    }
private:
    //int state;
    QList<Vector2D> pos;
    QList<double> dist;
    QList<Agent*> agentList;
    GotopointavoidAction *gpa;
    Agent** agents;


};
//        //my_move->eval();
//        myKick->setTarget(Vector2D{-3, 4.5});
//        myKick->setChip(false);
//        myKick->setKickspeed(2.5);
//        for (int i = 0 ; i < wm->opp.activeAgentsCount() ; i++)
//            if(wm->opp[i]->pos.y - agents[1]->pos().y < 0.5)
//            {
//                ROS_INFO_STREAM("kian: inja 0");
//                if(fabs(wm->opp[i]->pos.x - agents[1]->pos().x) < 0.5)
//                {
//                    ROS_INFO_STREAM("kian: inja");
//                    myKick->setChip(true);
//                    myKick->setChipdist(0.7);
//                }
//            }

//        agents[1]->action = myKick;

#endif //PARSIAN_AI_EXPRIMENTAL_H
