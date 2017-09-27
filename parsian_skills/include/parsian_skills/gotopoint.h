#ifndef GotoPoint_H
#define GotoPoint_H

#include <parsian_skills/skill.h>
#include "parsian_util/mathtools.h"
#include "newbangbang.h"
#include <QTime>
#include <QFile>
#include <QDebug>
#include <QFile>
#include <parsian_util/mathtools.h>
#include <parsian_util/core/agent.h>
#include <algorithm>
#include <math.h>

#define EPS 1e-6
#define DELTA_T 0.060      //wm->commandSampleTime()
//#define DELTA_T wm->commandSampleTime()

//  New Goto Point Class Powered by DON
//  start 12/4/2015

enum gpMode {
    GPACC1 = 0,
    GPACC2 = 1,
    GPVCONST = 2,
    GPDEC1 = 3,
    GPDEC2 = 4,
    GPPOS = 5,
    GPDIVE = 6
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CSkillGotoPoint : public CSkill
{


private:
    _PID *angPid;
    _PID *posPid;
    _PID *posXpid;
    _PID *posYpid;
    _PID *speedPidX;
    _PID *speedPidY;
    _PID *thPid;
    _PID *velPid;
    Vector2D startingPoint;
     ///////Arash.Z//////////
    //////////if boll dec = true the function return optimal dec and
    //////////otherwise return optimal acc//////////
    double optimalAccOrDec(double agentDirInRadian, bool dec);

    ///////////////////////
     void trajectoryPlanner();
    double appliedTh;
    //////////////////////


protected:

    Vector2D targetVel;
    Vector2D agentPos;
    Vector2D agentVel;
    double agentBestAcc;
    double agentBestDec;
    double agentDist;
    double agentVc;
    double _Vx, _Vy;
    double agentVm;
    double realAcc, realDec;
    double appliedAcc,appliedDec;
    double posPidDist;
    double agentVDesire;
    double decThr;
    double posThr;
    double vConstThr;
    double angleOfMovement;

    double agentX1, agentX2, agentX3;

    gpMode decideMode();
    AngleDeg agentMovementTh;
    AngleDeg lastPath;

public:
    void targetValidate();

    virtual double timeNeeded();
    int counting;
    DEF_SKILL(CSkillGotoPoint);
    double timeRemaining();
    gpMode lastGPmode;
    gpMode currentGPmode;
    void init(Vector2D _target, Vector2D _targetDir, Vector2D _targetVel = Vector2D(0.0, 0.0),bool dynamicStart = true);
    SkillProperty(CSkillGotoPoint, bool, DynamicStart, dynamicStart);
    SkillProperty(CSkillGotoPoint, Vector2D, InitialPos, pos1);
    SkillProperty(CSkillGotoPoint, Vector2D, FinalPos, targetPos);
    SkillProperty(CSkillGotoPoint, Vector2D, InitialDir, dir1);
    SkillProperty(CSkillGotoPoint, Vector2D, FinalDir, targetDir);
    SkillProperty(CSkillGotoPoint, Vector2D, InitialVel, vel1);
    SkillProperty(CSkillGotoPoint, Vector2D, FinalVel, vel2);
    SkillProperty(CSkillGotoPoint, Vector2D, LookAt, lookat);
    SkillProperty(CSkillGotoPoint, Vector2D, BiasVel, biasVel);
    SkillProperty(CSkillGotoPoint, Vector2D, ShotTarget, target);
    SkillProperty(CSkillGotoPoint, double, MaxAcceleration, _Acc);
    SkillProperty(CSkillGotoPoint, double, MaxDeceleration, _Dec);
    SkillProperty(CSkillGotoPoint, double, MaxVelocity, _VmDesire);
    SkillProperty(CSkillGotoPoint, double, MaxAccelerationNormal, aMaxNorm);
    SkillProperty(CSkillGotoPoint, double, MaxDecelerationNormal, dMaxNorm);
    SkillProperty(CSkillGotoPoint, double, MaxVelocityNormal, vMaxNorm);
    SkillProperty(CSkillGotoPoint, double, ConstantVelocity, vConst);
    SkillProperty(CSkillGotoPoint, double, TurningDist, turningDist);
    SkillProperty(CSkillGotoPoint, bool, FasterEnd, fasterEnd);
    SkillProperty(CSkillGotoPoint, bool, NotEnd, notEnd);
    SkillProperty(CSkillGotoPoint, bool, FastW, fastw);
    SkillProperty(CSkillGotoPoint, bool, RecordProfile, recordProf);
    SkillProperty(CSkillGotoPoint, bool, BallMode, ballMode);
    SkillProperty(CSkillGotoPoint, bool, SlowShot, slow);
    SkillProperty(CSkillGotoPoint, bool, moveLookingTarget, lootTarget);
    SkillProperty(CSkillGotoPoint, bool, InterceptMode, interceptMode);
    SkillProperty(CSkillGotoPoint, bool, LookForward, lookForward);
    SkillProperty(CSkillGotoPoint, bool, OneTouchMode, oneTouchMode);
    SkillProperty(CSkillGotoPoint, bool, SlowMode, slowMode);
    SkillProperty(CSkillGotoPoint, bool, NoPID, noPID);
    SkillProperty(CSkillGotoPoint, bool, PenaltyKick, penaltyKick);
    SkillProperty(CSkillGotoPoint, bool, GoalieMode, goalieMode);
    SkillProperty(CSkillGotoPoint, Vector2D, MotionDir, motiondir);
    SkillProperty(CSkillGotoPoint, float, ForwardBias, forwardBias);
    SkillProperty(CSkillGotoPoint, bool, DiveMode, diveMode);
    SkillProperty(CSkillGotoPoint, bool, VerySlow, verySlow);
    SkillProperty(CSkillGotoPoint, bool , Smooth,smooth);
};

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

#endif // GOTOPOINT_H
