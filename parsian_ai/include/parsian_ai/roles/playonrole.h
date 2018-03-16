#ifndef PLAYONROLE_H
#define PLAYONROLE_H

#include <role.h>

enum PSkills {
    None = 0,
    MoveOffensive = 1,
    MoveDefensive = 2,
    PassOffensive = 3,
    PassDefensive = 4,
    KickOffensive = 5,
    KickDefensive = 6,
    ChipOffensive = 7,
    ChipDefensive = 8,
    MarkOffensive = 9,
    MarkDefensive = 10,
    OneTouch = 11,
    CatchBall = 12,
    ReceivePass = 13,
    Shot = 14,
    ChipToGoal = 15
};

enum MSkills {
    MNone = 0,
    MMove = 1,
    MPass = 2,
    MReceivePass = 3,
    MShoot = 4,
    MChip = 5,
    MMark = 6,
    MOneTouch = 7

};

class CRolePlayOn {
private:

    CSkillGotoPoint *gotoPointSkill;
    CSkillGotoPointAvoid *gotoPointAvoidSkill;
    CSkillKick *kickSkill;
    CSkillReceivePass *receivePassSkill;
    CSkillKickOneTouch *oneTouchSkill;

    Agent *agent;
public:
    CRolePlayOn();
    ~CRolePlayOn();
    void execute();

    bool updated;
    void initTask();

    void setAgent(Agent *_agent);

    ClassProperty(CRolePlayOn, PSkills, Task, task, updated);
    ClassProperty(CRolePlayOn, bool, IsActive, isActive, updated);
    ClassProperty(CRolePlayOn, int, AgentID, agentID, updated);
//    ClassProperty(CRolePlayOn, Agent ,Agent, agent,  updated);
    ClassProperty(CRolePlayOn, roleSkill::ESkill, SelectedSkill, selectedSkill, updated);


    ClassProperty(CRolePlayOn, Vector2D, Target, target, updated);
    ClassProperty(CRolePlayOn, Vector2D, TargetDir, targetDir, updated);
    ClassProperty(CRolePlayOn, Vector2D, TargetVel, targetVel, updated);


    ClassProperty(CRolePlayOn, int, OppRelax, oppRelax, updated);
    ClassProperty(CRolePlayOn, int, OurRelax, ourRelax, updated);
    ClassProperty(CRolePlayOn, bool, AvoidPenaltyArea, avoidPenaltyArea, updated);
    ClassProperty(CRolePlayOn, bool, AvoidCenterCircle, avoidCenterCircle, updated);

    ClassProperty(CRolePlayOn, double, Tolerance, tolerance, updated);
    ClassProperty(CRolePlayOn, bool, Chip, chip, updated);
    ClassProperty(CRolePlayOn, bool, Spin, spin, updated);
    ClassProperty(CRolePlayOn, int, KickSpeed, kickSpeed, updated);
    ClassProperty(CRolePlayOn, bool, IsGotoPointAvoid, isGotoPointAvoid, updated);

    ClassProperty(CRolePlayOn, bool, Slow, slow, updated);
    ClassProperty(CRolePlayOn, float, ReceiveRadius, receiveRadius, updated);

    ClassProperty(CRolePlayOn, Vector2D, WaitPos, waitPos, updated);

    //fateme
    ClassProperty(CRolePlayOn, bool, DontKick, dontKick, updated);
//    ClassProperty(CRolePlayOn, Vector2D, Pos, pos, updated);
};

#endif // PLAYONROLE_H
