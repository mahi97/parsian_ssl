#ifndef DYNAMICROLE_H
#define DYNAMICROLE_H

#include <parsian_util/base.h>
#include <parsian_util/core/agent.h>
#include <parsian_ai/roles/role.h>

class CRoleDynamic
{
public:
    CRoleDynamic();
    ~CRoleDynamic();
    void execute();

private:
    KickAction           *shotSkill;
    ReceivepassAction    *receiveSkill;
    GotopointavoidAction *moveSkill;
    OnetouchAction       *oneTouchSkill;
    // CSKill "KEEP"

    void update();
    bool updated;

    SkillProperty(CRoleDynamic, Agent*, Agent, agent);
    SkillProperty(CRoleDynamic, DynamicEnums::DynamicSkill, SelectedSkill, selectedSkill);
    SkillProperty(CRoleDynamic, Vector2D, Target, target);
    SkillProperty(CRoleDynamic, Vector2D, TargetDir, targetDir);
    SkillProperty(CRoleDynamic, bool, AvoidPenaltyArea, avoidPenaltyArea);
    SkillProperty(CRoleDynamic, double, Tolerance, tolerance);
    SkillProperty(CRoleDynamic, bool, Chip, chip);
    SkillProperty(CRoleDynamic, int, KickSpeed, kickSpeed);
    SkillProperty(CRoleDynamic, double, ReceiveRadius, receiveRadius);
    SkillProperty(CRoleDynamic, Vector2D, WaitPos, waitPos);
    SkillProperty(CRoleDynamic, bool, VeryFine, veryFine);
    SkillProperty(CRoleDynamic, bool, EmptySpot, emptySpot);
    SkillProperty(CRoleDynamic, bool, NoKick, noKick);
    SkillProperty(CRoleDynamic, int, AgentID, agentID);

public:
    CRoleDynamic* setKickRealSpeed(double val) {
//        kickSpeed = knowledge -> getProfile(agent->id(), val, !chip, false);
        DBUG(QString("setkickrealspeed : %1 %2").arg(val).arg(kickSpeed), D_MAHI);
        updated = true;
        return this;
    }


    CRoleDynamic* setAddKickRealSpeed(double val) {
//        kickSpeed = knowledge -> getProfile(agent->id(), agent->pos().dist(target) + val, !chip, false);
        updated = true;
        return this;
    }

};

#endif // DYNAMICROLE_H
