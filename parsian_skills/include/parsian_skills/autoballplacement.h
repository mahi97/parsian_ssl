#ifndef AUTOBALLPLACEMENT_H
#define AUTOBALLPLACEMENT_H

#include "parsian_skills/gotopoint.h"
#include "parsian_skills/trackcurve.h"
#include "knowledge.h"
#include "pid.h"
#include "kick.h"
class CSkillAutoBallPlacement : public CSkill
{
private:
//    CSkillGotoPointAvoid *GPA;
    CSkillKick *kick;
    CNewBangBang *bangBang;
    bool isVisionOkArea;
    void gotoBall();
    void gotoTarget();
    int state;
    Vector2D ballPos;
    Vector2D agentPos;
    int ballCounter = 0;
    bool ballCatchFlag = false;
public:
    DEF_SKILL(CSkillAutoBallPlacement);

    SkillProperty(CSkillAutoBallPlacement, bool, IsFinished, isFinished);
    SkillProperty(CSkillAutoBallPlacement, Vector2D, Target, target);
};

void autoBallPlacement();

#endif // AUTOBALLPLACEMENT_H
