//
// Created by fateme on 11/12/17.
//

#include <plan_server/planselector.h>

CPlanSelector::CPlanSelector() {
    planLoader = new CLoadPlayOffJson(QDir::currentPath() + QString("/playoff"));
    havePassInPlan = false;
}


void CPlanSelector::initStaticPlay(const POMODE _mode, const QList<int>& _ourplayers) {

    static QList<NGameOff::SPlan*> validPlans, prevValidPlans;
    NGameOff::SPlan* thePlan = NULL;

    prevValidPlans = validPlans;
    validPlans.clear();

    validPlans = getValidPlans(_mode, _ourplayers);

    if (validPlans.isEmpty()) {
        DEBUG("[coach] WE DONT HAVE PLAN AT ALL", D_MAHI);
        return;
    }

    //// SELECT A PLAN :
    /** NEW PLAN SELECTOR **/
    /** PLAN SELECTION BASED ON HISTORY **/
    /** LFU SELECTION **/
    /** SHUFFLE PLAN SELECTION **/
    /** COUNTER PLAN SELECTION **/


    matchPlan(thePlan, _ourplayers); //Match The Plan

//    checkGUItoRefineMatch(thePlan, _ourplayers);
    analyseShoot(thePlan);
    analysePass(thePlan);
    selectedPlan = thePlan;

}

QList<NGameOff::SPlan *> CPlanSelector::getValidPlans(const POMODE _mode, const QList<int> &_ourPlayers) {
    QList<NGameOff::SPlan*> plans;

    plans = planLoader->getPlans();
}

void CPlanSelector::matchPlan(NGameOff::SPlan *_plan, const QList<int>& _ourplayers) {
    MWBM matcher;
    matcher.create(_plan->common.currentSize, _ourplayers.size());
    for (size_t i = 0; i < _plan->common.currentSize; i++) {
        for (size_t j = 0; j < _ourplayers.size(); j++) {

            double weight;
            if (_plan->matching.initPos.agents.at(i).x == -100) {
                weight = wm->our.active(j)->pos.dist(wm->ball->pos);
            } else {
                weight = _plan->matching.initPos.agents.at(i).dist(wm->our.active(j)->pos);
            }
            matcher.setWeight(i, j, -(weight));
        }
    }
    qDebug() << "[Coach] matched plan with : " << matcher.findMatching();
    for (size_t i = 0; i < _plan->common.currentSize;i++) {
        int matchedID = matcher.getMatch(i);
        _plan->common.matchedID.insert(i, _ourplayers.at(matchedID));

    }
    qDebug() << "[Coach] mathched by" << _plan->common.matchedID;
}


QPair<int, int> CPlanSelector::findTheLastShoot(const NGameOff::SExecution &_plan) {
    QPair<int, int> last;
    last.first = last.second = -1;

    QList<POffSkills> finalSkills;
    finalSkills.append(ShotToGoalSkill);
    finalSkills.append(ChipToGoalSkill);
    finalSkills.append(OneTouchSkill);

    int counter = 0;
    qDebug() << "SSS" << _plan.AgentPlan.size();
    Q_FOREACH (QList<playOffRobot> agent, _plan.AgentPlan) {
            int counter2 = 0;
            Q_FOREACH(playOffRobot node, agent) {
                    Q_FOREACH(playOffSkill skill, node.skill) {
                            if (finalSkills.contains(skill.name)) {
                                last.first  = counter;
                                last.second = counter2;
                                qDebug() << "MAHI " << skill.name;
                            }
                            counter2++;
                        }
                }
            counter++;
        }

    return last;
}


void CPlanSelector::analyseShoot(NGameOff::SPlan* thePlan)
{
    if (thePlan != NULL) {
        QPair<int, int> last;
        last = findTheLastShoot(thePlan->execution);
        thePlan->execution.theLastAgent = last.first;
        thePlan->execution.theLastState = last.second;
//        havePassInPlan = (last.first != -1  && last.second != -1);
    }
}

void CPlanSelector::analysePass(NGameOff::SPlan* thePlan) {
    // TODO : need edit for mulitiple pass
    if (thePlan != NULL) {
        // first : passer second : reciver
        QList<AgentPair> tPass;
        findThePasserAndReceiver(thePlan->execution, tPass);
        havePassInPlan = tPass.size() > 0;
        thePlan->execution.passCount = tPass.size();
        if (havePassInPlan) {
            thePlan->execution.passer .id     = tPass.at(0).first.id;
            thePlan->execution.passer .state  = tPass.at(0).first.state;
            thePlan->execution.reciver.id     = tPass.at(0).second.id;
            thePlan->execution.reciver.state  = tPass.at(0).second.state;
        }
    }

    qDebug() << "PI : " << thePlan->execution.passer .id;
    qDebug() << "PS : " << thePlan->execution.passer .state;
    qDebug() << "RI : " << thePlan->execution.reciver.id;
    qDebug() << "RS : " << thePlan->execution.reciver.state;
}

void CPlanSelector::findThePasserAndReceiver(const NGameOff::SExecution &_plan,
                                             QList<AgentPair> &_pairList) {

    QList<NGameOff::AgentPoint> passer;

    for (int i = 0; i < _plan.AgentPlan.size(); i++) {
        const QList<playOffRobot> & agent = _plan.AgentPlan.at(i);
        for (int j = 0; j < agent.size(); j++) {
            const playOffRobot& node = agent.at(j);
            for (int k = 0; k < node.skill.size(); k++) {
                const POffSkills& skill = node.skill.at(k).name;
                if (skill == PassSkill) {
                    passer.append(NGameOff::AgentPoint(i,j));
                }
            }
        }
    }

    for (int i = 0; i < passer.size(); i++) {
        const int &id = passer.at(i).id;
        const int &st = passer.at(i).state;

        int si = (_plan.AgentPlan[id][st].skill[1].name == PassSkill) ? 1 : 2;
        int rid = _plan.AgentPlan[id][st].skill[si].targetAgent;
        int rs  = _plan.AgentPlan[id][st].skill[si].targetIndex;
        DEBUG(QString("PASS : %1, %2, %3, %4").arg(id).arg(st).arg(rid).arg(rs), D_MAHI);
        NGameOff::AgentPoint tempReciver;
        tempReciver.id    = rid;
        tempReciver.state = rs;

        AgentPair ap;
        ap.first  = passer[i];
        ap.second = tempReciver;
        _pairList.append(ap);

    }
}