//
// Created by parsian-ai on 9/29/17.
//

#ifndef PARSIAN_SKILLS_GOTOPOINTAVOID_H
#define PARSIAN_SKILLS_GOTOPOINTAVOID_H

#include <parsian_skills/gotopoint.h>

class CSkillGotoPointAvoid : public CSkillGotoPoint
{
private:
//    CMotionProfile *prof;
    bool pathRestarted;
    CNewBangBang *bangBang;

protected:
    int stucked;
    Vector2D lastPoint;
    CSkillGotoPoint* gotopoint;
    QList<int> ourRelaxList , oppRelaxList;
    QList <Vector2D> pathPoints;
    int counter;
    bool inited;
    void followPath();
    Vector2D averageDir;
public:
    double timeStarted, timeEstimated; //for skill widget
    static double timeNeeded(CAgent *_agentT, Vector2D posT, double vMax, QList <int> _ourRelax, QList <int> _oppRelax , bool avoidPenalty, double ballObstacleReduce, bool _noAvoid);
    DEF_SKILL(CSkillGotoPointAvoid);
    CSkillGotoPointAvoid* noRelax();
    CSkillGotoPointAvoid* ourRelax(int element);
    CSkillGotoPointAvoid* oppRelax(int element);
    CSkillGotoPointAvoid* setTargetLook(Vector2D finalPos, Vector2D lookAtPoint);
    CSkillGotoPointAvoid* setTarget(Vector2D finalPos, Vector2D finalDir);
    QList<Vector2D> result;
    SkillProperty(CSkillGotoPointAvoid, Vector2D, AddVel, addVel);
    SkillProperty(CSkillGotoPointAvoid, bool, AvoidPenaltyArea, avoidPenaltyArea);
    SkillProperty(CSkillGotoPointAvoid, bool, KeepLooking, keeplooking);
    SkillProperty(CSkillGotoPointAvoid, double, ExtendStep, extendStep);
    SkillProperty(CSkillGotoPointAvoid, bool, DynamicStart, dynamicStart);
    SkillProperty(CSkillGotoPointAvoid, bool, Plan2, plan2);
    SkillProperty(CSkillGotoPointAvoid, bool, NoAvoid, noAvoid);
    SkillProperty(CSkillGotoPointAvoid, bool, AvoidCenterCircle, avoidCenterCircle);
    SkillProperty(CSkillGotoPointAvoid, double, BallObstacleRadius , ballObstacleRadius);
    SkillProperty(CSkillGotoPointAvoid, bool, ADiveMode, diveMode);
    SkillProperty(CSkillGotoPointAvoid, bool, AvoidBall, avoidBall);
    SkillProperty(CSkillGotoPointAvoid, bool, AvoidGoalPosts, avoidGoalPosts);
    SkillProperty(CSkillGotoPointAvoid, bool, DrawPath, drawPath);

    SkillProperty(CSkillGotoPointAvoid, Vector2D, NextPos, nextPos);

};

#endif //PARSIAN_SKILLS_GOTOPOINTAVOID_H
