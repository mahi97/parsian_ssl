//
// Created by fateme on 11/12/17.
//

#ifndef PARSIAN_AI_PLANSELECTOR_H
#define PARSIAN_AI_PLANSELECTOR_H

#include <plan_server/util/loadplayoffjson.h>
#include <parsian_ai/gamestate.h>
#include <parsian_util/core/worldmodel.h>
#include <parsian_util/mathtools.h>

class CPlanSelector{
public:
    CPlanSelector();
    QList<NGameOff::SPlan *> getValidPlans(const POMODE _mode, const QList<int>& _ourPlayers);
    void initStaticPlay(const POMODE _mode, const QList<int>& _ourplayers);
    void matchPlan(NGameOff::SPlan *_plan, const QList<int>& _ourplayers);
    QPair<int, int> findTheLastShoot(const NGameOff::SExecution &_plan);
    void analyseShoot(NGameOff::SPlan* thePlan);
    void analysePass(NGameOff::SPlan* thePlan);
    void findThePasserAndReceiver(const NGameOff::SExecution &_plan, QList<AgentPair> &_pairList);
    void setHavePassInPlan(bool input){havePassInPlan = input;}
    bool getHavePassInPlan(){return havePassInPlan;}

    NGameOff::SPlan * selectedPlan;
    CLoadPlayOffJson* planLoader;
    bool havePassInPlan;

};

#endif //PARSIAN_AI_PLANSELECTOR_H