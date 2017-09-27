//
// Created by parsian-ai on 9/21/17.
//

#ifndef PARSIAN_SKILLS_ONETOUCH_H
#define PARSIAN_SKILLS_ONETOUCH_H

#include "skill.h"
#include "kick.h"

////////////////////////////////////////////////////
enum kkOTMode {
    OTWAITPOS = 1,
    OTINTERSECT = 2,
    OTKICK = 3,
    OTKOSKHOL = 4
};

class CSkillKickOneTouch : public CSkill
{
protected:
    int p_area_avoidance_state;
    CSkillGotoPointAvoid* gotopointavoid;
    bool oneTouched;
    CSkillKick* kick;
    QTime* timeAfterForceKick;
    bool forceKicked;
    Vector2D pos0,pos1;
private:
    Vector2D kkBallPos;
    Vector2D kkAgentPos;
    kkOTMode oneTouchMode;
    kkOTMode decideMode();
    double cirThresh;
    double kickCirThresh;
    double velThresh;
    Vector2D findMostPossible();
public:
    double ballRealVel;
    static Vector2D newOneTouchAng(CAgent *oneTouchAgent,Vector2D target,double kickSpeed,double alpha,double beta,double gama);
    static double oneTouchAngle(Vector2D pos, Vector2D vel, Vector2D ballVel, Vector2D ballDir, Vector2D goal, double landa, double gamma);
    static double oneTouchAngle(Vector2D pos, Vector2D dir, Vector2D vel, Vector2D ballVel, Vector2D ballDir, Vector2D goal, double landa, double gamma);
    DEF_SKILL(CSkillKickOneTouch);
    SkillProperty(CSkillKickOneTouch, bool, Chip, chip);
    SkillProperty(CSkillKickOneTouch, int, KickSpeed, kickSpeed);
    SkillProperty(CSkillKickOneTouch, Vector2D, WaitPos, waitpos);
    SkillProperty(CSkillKickOneTouch, Vector2D, Target, target);
    SkillProperty(CSkillKickOneTouch, double, DistToBallLine, distToBallLine);
    SkillProperty(CSkillKickOneTouch, double, VelocityToBallLine, velToBallLine);
    SkillProperty(CSkillKickOneTouch, bool, RecvChip, recvChip);
    SkillProperty(CSkillKickOneTouch, bool, AvoidPenaltyArea, avoidPenaltyArea);
    SkillProperty(CSkillKickOneTouch, bool, MoveTowardTheBall, moveTowardTheBall);
    SkillProperty(CSkillKickOneTouch, bool, ShotToEmptySpot, shotToEmptySpot);
};


#endif //PARSIAN_SKILLS_ONETOUCH_H
