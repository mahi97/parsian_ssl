#ifndef BASICSKILL_H
#define BASICSKILL_H

#include "parsian_util/statemachine.h"
#include "parsian_util/base.h"
#include "parsian_msgs/parsian_world_model.h"
#include "parsian_msgs/parsian_robot_command.h"
#include "parsian_msgs/parsian_robot.h"
#include "parsian_msgs/parsian_agent.h"

class CSkill
{
protected:
    CStateMachine sm;
    QString stateName();
public:
    QString localAgentName;
    CSkill() {}
    CSkill(CAgent* _agent);
    ~CSkill();
    virtual int level();
    bool done();
    bool failed();
    void assign(parsian_msgs::parsian_agent* _agent);
    virtual void parse(QStringList params);
    virtual void generateFromConfig(parsian_msgs::parsian_agent* a);
    virtual CSkill* allocate(parsian_msgs::parsian_agent* _agent)=0;
    virtual QString getName()=0;
    virtual double timeNeeded();  //in seconds
    virtual double successRate(); //between 0-1    

    //these functions should be defined in child classes    
    virtual double progress()=0;  //between 0-1 ; less than zero on failure
    virtual void execute()=0;

    Property(parsian_msgs::parsian_agent*, Agent, agent);
friend class CSkills;
};

class CSkills
{
public:
    CSkills();
    ~CSkills();
    static bool registerSkill(const char *name, CSkill* Skill);
    static CSkill* initSkill(const char *name, CAgent* _agent);
    static int skillsCount();
    static CSkill* skill(int i);
    static void* getInfo(const char* name);
private:
    struct RegisteredSkill
    {
        const char *name;
        CSkill *Skill;
        void* Info;
    };
    static QList<RegisteredSkill>* Skills;
    static bool inited;
};

#define DEF_SKILL(skill) \
    skill() {skill(NULL);} \
    skill(parsian_msgs::parsian_agent* _agent); \
    ~skill(); \
    static const char *Name;\
    virtual CSkill* allocate(parsian_msgs::parsian_agent* _agent); \
    virtual void execute(); \
    virtual double progress(); \
    virtual QString getName(); \
    static skill* set(CAgent* a); \
    static skill* get(CAgent* a)
#define INIT_SKILL(Skill,name) \
    bool Skill##_registered \
            = CSkills::registerSkill(Skill::Name,new Skill(NULL)); \
    CSkill* Skill::allocate(parsian_msgs::parsian_agent* _agent) \
    {return new Skill(_agent);} \
    QString Skill::getName() {return QString(Name);} \
    Skill* Skill::set(parsian_msgs::parsian_agent* a) \
    { \
        if (QString(Skill::Name)!=a->skillName) \
        { \
            if (a->skill!=NULL && a->skillName!="") delete ((Skill*) a->skill); \
            else a->skill = (CSkill*) (new Skill(a)); \
            a->skillName = Skill::Name; \
        } \
        return (Skill*) a->skill; \
    } \
    Skill* Skill::get(parsian_msgs::parsian_agent* a) \
    { \
        return (Skill*) a->skill; \
    } \
    const char* Skill::Name = name

#define SkillState(classname,statenumber) void classname::State##statenumber ()
#define SkillInitState(statenumber,statename) \
        CState* st##statenumber = new CState; \
        QObject::connect(st##statenumber, SIGNAL(run()), this, SLOT(State##statenumber())); \
        st##statenumber -> name = statename; \
        sm.addState(st##statenumber)
#define SkillSwitchState(tostate) sm.setCurrentState(tostate)
#define SkillBreakState return
#define SkillRunStates sm.run();return

#define SkillProperty(skill,type,name,local) \
        public: inline type get##name() {return local;} \
        public: inline skill* set##name(type val) {local = val;return this;} \
        protected: type local

#define SkillPropertyNoSet(skill,type,name,local) \
        public: inline type get##name() {return local;} \
        public: skill* set##name(type val); \
        protected: type local

#endif // BASICSKILL_H
