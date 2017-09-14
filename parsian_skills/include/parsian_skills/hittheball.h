#ifndef HITTHEBALL_H
#define HITTHEBALL_H

#include <parsian_skills/gotoball.h>
#include <parsian_skills/gotopoint.h>
#include <parsian_skills/skills.h>

class CSkillHitTheBall : public CSkill
{
protected:
    CSkillGotoBall* gotoball;
    CSkillGotoPointAvoid* gotopoint;
    CSkillGoAcrossLine* goacrossline;
    CSkillKick* kick;
    bool lastThroughed;
public:
    DEF_SKILL(CSkillHitTheBall);
    virtual void generateFromConfig(CAgent *a);
    virtual CSkillConfigWidget* generateConfigWidget(QWidget *parent);
    SkillPropertyNoSet(CSkillHitTheBall, bool, ThroughMode, throughMode);
    SkillPropertyNoSet(CSkillHitTheBall, Vector2D, Target, target);
};

#endif // HitTheBall_H
