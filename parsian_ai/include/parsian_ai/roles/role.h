#ifndef ROLE_H
#define ROLE_H

#include <parsian_util/core/worldmodel.h>
#include <parsian_util/base.h>
#include <parsian_ai/soccer.h>

//#include <skills.h> TODO : Actions
//#include <knowledge.h>
//#include <behaviours/behaviour.h>
#include <QString>
#include <parsian_util/core/agent.h>
#include <parsian_util/action/autogenerate/gotopointaction.h>
#include <parsian_util/action/autogenerate/gotopointavoidaction.h>
#include <parsian_util/action/autogenerate/kickaction.h>
#include <parsian_util/action/autogenerate/onetouchaction.h>
#include <parsian_util/action/autogenerate/receivepassaction.h>
#include <parsian_util/action/autogenerate/noaction.h>

class CRoleInfo;

class CRole
{
public:
    CRole();
    CRole(CAgent* _agent);
    CAgent* agent;
    void assign(CAgent* _agent);
    virtual CRoleInfo* generateInfoClass();
    static CRole* fromString(QString roleName);
};

class CRoleInfo
{    
protected:
    QString roleName;
public:        
    bool calculated;

    explicit CRoleInfo(QString _roleName);

    virtual CAgent* robot(int i);
    virtual int count();
    virtual int index(CRole* me);

    virtual void reset(){}
};





#endif // ROLE_H
