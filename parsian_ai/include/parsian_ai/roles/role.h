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

enum ESkill {
    Gotopoint      = 0,
    GotopointAvoid = 1,
    Kick           = 2,
    ReceivePass    = 3,
    OneTouch       = 4,
    Mark           = 5,  // After Life (Steady)
    Support        = 6,  // After Life (Move for ball)
    Defense        = 7   // After Life (Move back to our field)
};

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
    CRoleInfo(QString _roleName);

    virtual CAgent* robot(int i);
    virtual int count();
    virtual int index(CRole* me);

    virtual void reset(){}
};

#define ClassProperty(skill,type,name,local,chflag) \
        public: inline type get##name() const {return local;} \
        public: inline skill* set##name(type val) {local = val;chflag = true;return this;} \
        protected: type local



#endif // ROLE_H
