#ifndef AUTOBALLPLACEMENT_H
#define AUTOBALLPLACEMENT_H

#include "gotopoint.h"
#include "kick.h"
#include "parsian_msgs/parsian_robot_command.h"

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
    parsian_msgs::parsian_robot_command* command;
public:
    DEF_SKILL(CSkillAutoBallPlacement);

    SkillProperty(CSkillAutoBallPlacement, bool, IsFinished, isFinished);
    SkillProperty(CSkillAutoBallPlacement, Vector2D, Target, target);
};

void autoBallPlacement();

#endif // AUTOBALLPLACEMENT_H
