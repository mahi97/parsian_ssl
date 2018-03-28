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
#include <behavior/move_forward/move_forward.h>

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
        myRec = new ReceivepassAction;
        state = State::SIMPLE_MOVE;
        //my_move = new BehaviorMove_Forward;
    }
    ~Exprimental() {

    }
    void reset() {

    }
    void execute() {
        ROS_INFO_STREAM("kian1: " << (int)state);
        if(state == State::SIMPLE_MOVE)
            simplemove();
        if(state == State::WAITFORIT)
            if(waitforit())
                state = State::CHIP;
        if(state == State::CHIP)
            if(chip())
//                state = State::GOCURVE;
//        if(state == State::GOCURVE)
//            if(gocurve())
//                state = State::CATCHBALL;

        if(isthreat())
            state = State::WAITFORIT;
    }
private:
    //int state;
    QList<Vector2D> pos;
    QList<double> dist;
    QList<Agent*> agentList;
    GotopointavoidAction *gpa;
    Agent** agents;
    BehaviorMove_Forward *my_move;
    State state;
    Vector2D target{-3, 4.5};
    int ID{1};
    double threatlen{1};
    bool isthreat()
    {
        Vector2D ballvel{wm->ball->vel.x, wm->ball->vel.y};
        Segment2D tmp{Vector2D{0, 0}, ballvel};
        Vector2D perballvel{1, -1*wm->ball->vel.x/wm->ball->vel.y};
        Vector2D ballpos{wm->ball->pos.x, wm->ball->pos.y};
        Vector2D a1{ballpos.x + threatlen*ballvel.x + (threatlen/3)*perballvel.x , ballpos.y + threatlen*ballvel.y + (threatlen/3)*perballvel.y};
        Vector2D a2{ballpos.x + threatlen*ballvel.x + (-1*threatlen/3)*perballvel.x , ballpos.y + threatlen*ballvel.y + (-1*threatlen/3)*perballvel.y};
        Vector2D a3{ballpos.x + ballvel.x + (threatlen/8)*perballvel.x , ballpos.y + ballvel.y + (threatlen/8)*perballvel.y};
        Vector2D a4{ballpos.x + ballvel.x + (-1*threatlen/8)*perballvel.x , ballpos.y + ballvel.y + (-1*threatlen/8)*perballvel.y};
        std::vector<Vector2D> threathead;
        threathead.push_back(a1); threathead.push_back(a2); threathead.push_back(a3); threathead.push_back(a4); threathead.push_back(a1);
        Polygon2D threatZone{threathead};
        for(int i{}; i < wm->opp.activeAgentsCount(); i++)
            if(threatZone.contains(Vector2D{wm->opp[i]->pos.x + wm->opp[i]->vel.x, wm->opp[i]->pos.y + wm->opp[i]->vel.y}, false))
                return true;
    }

    void simplemove()
    {
        myKick->setTarget(target);
        myKick->setChip(false);
        myKick->setKickspeed(2.5);
        myKick->setIskhafan(true);
        agents[ID]->action = myKick;

    }
    bool waitforit()
    {
        Vector2D ballposvel{wm->ball->pos.x - wm->ball->vel.x - 0.2, wm->ball->pos.y - wm->ball->vel.y - 0.2};
        myRec->setReceiveradius(3);
        myRec->setSlow(false);
        myRec->setTarget(ballposvel);
        myRec->setIatargetdir(target);
        agents[ID]->action = myRec;
        if(sqrt(wm->ball->vel.x * wm->ball->vel.x + wm->ball->vel.y * wm->ball->vel.y) < 1)
            return true;
        return false;
    }

    bool chip()
    {
        myKick->setTarget(target);
        myKick->setChip(true);
        myKick->setChipdist(0.7);
        agents[ID]->action = myKick;
        if(sqrt(wm->ball->pos.x * wm->ball->pos.x + wm->ball->pos.y * wm->ball->pos.y) > 1.2)
            return true;
        return false;
    }
    bool gocurve()
    {

    }


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
