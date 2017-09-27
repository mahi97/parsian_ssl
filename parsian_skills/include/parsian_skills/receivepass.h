//
// Created by parsian-ai on 9/21/17.
//

#ifndef PARSIAN_SKILLS_RECEIVEPASS_H
#define PARSIAN_SKILLS_RECEIVEPASS_H

#include "skill.h"
#include "gotopoint.h"
#include "kick.h"

/////////////////////////////////////////////////////////////// receive pass skill created by DON MHMMD SHIRAZI
enum kkRPMode {
    RPWAITPOS = 1,
    RPINTERSECT = 2,
    RPDAMP = 3,
    RPRECEIVE = 4,
    RPNONE = 5
};

class CSkillReceivePass : public CSkill
{
private:
    CSkillGotoPointAvoid* gotopointavoid;
    Vector2D kkBallPos;
    Vector2D kkAgentPos;
    kkRPMode receivePassMode;
    kkRPMode decideMode();
    double cirThresh;
    double kickCirThresh;
    double velThresh;
public:
    double ballRealVel;
    DEF_SKILL(CSkillReceivePass);
    /*CSkillReceivePass() {CSkillReceivePass(NULL);}
    CSkillReceivePass(CAgent*);*/
    bool received;
    /*void execute();
    void setAgent(CAgent*);*/
    bool isReceived();
    SkillProperty(CSkillReceivePass, Vector2D, Target, target);
    SkillProperty(CSkillReceivePass, bool , AvoidOurPenaltyArea, avoidOurPenaltyArea);
    SkillProperty(CSkillReceivePass, bool , AvoidOppPenaltyArea, avoidOppPenaltyArea);
    SkillProperty(CSkillReceivePass, bool , Slow, slow);
    SkillProperty(CSkillReceivePass, float , ReceiveRadius, receiveR);
    SkillProperty(CSkillReceivePass, bool , IgnoreAngle, ignoreAngle);
    SkillProperty(CSkillReceivePass, Vector2D, IATargetDir, IATargetDir);

};




#endif //PARSIAN_SKILLS_RECEIVEPASS_H
