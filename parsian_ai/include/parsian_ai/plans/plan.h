#ifndef PLAN_H
#define PLAN_H

#include <QList>
#include "parsian_util/mathtools.h"
#include <parsian_util/action/action.h>
#include <parsian_util/core/agent.h>
#include <parsian_ai/util/knowledge.h>
//#include <skills.h> // TODO : Actions

class Plan
{
protected:
    QList<CAgent*> agents;
public:
    Plan();
    CAgent* agent(int i);
    int count();
    void init(const QList<CAgent*> & _agents);
    void assignSkill(int i, Action* skill);
    int agentById(int id);
    QList<int> getAgentIdList();
	void debugAgents(QString text);

//abstract functions
    virtual void execute() = 0;
};

#endif // PLAN_H
