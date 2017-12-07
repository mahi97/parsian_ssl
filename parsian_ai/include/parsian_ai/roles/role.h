#ifndef ROLE_H
#define ROLE_H

#include <parsian_util/base.h>
#include <parsian_ai/util/worldmodel.h>
#include <QString>
#include <parsian_ai/util/agent.h>
#include <parsian_ai/util/knowledge.h>
#include <parsian_ai/gamestate.h>
#include <parsian_util/action/autogenerate/gotopointaction.h>
#include <parsian_util/action/autogenerate/gotopointavoidaction.h>
#include <parsian_util/action/autogenerate/kickaction.h>
#include <parsian_util/action/autogenerate/onetouchaction.h>
#include <parsian_util/action/autogenerate/receivepassaction.h>
#include <parsian_util/action/autogenerate/noaction.h>
//#include <skills.h> TODO : Actions


class CRoleInfo;

class CRole
{
public:
    CRole();
    explicit CRole(Agent* _agent);

    Agent* agent;
    void assign(Agent* _agent);
    virtual CRoleInfo* generateInfoClass();
    static CRole* fromString(QString roleName);
};

class CRoleInfo
{    
protected:
    QString roleName;
public:        

    explicit CRoleInfo(QString _roleName);

    virtual Agent* robot(int i);
    virtual int count();
    virtual int index(CRole* me);

    virtual void reset(){}
};





#endif // ROLE_H
