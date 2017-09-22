#ifndef KICK_H
#define KICK_H

#include "gotopoint.h"
#include <QtCore/QQueue>
#include "parsian_util/core/ball.h"
#include "parsian_util/core/movingobject.h"

#define penaltyAreaAvoidanceRadius (0.9)




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum kckMode {
    KDIRECT = 0,
    KINDIRECCT = 1,
    KJTURN = 2,
    KTURN = 3,
    KINTERCEPT = 4,
    KKICK = 5,
    KPENALTY = 6,
    KAVOIDOPPENALTY =7,
    KWAITANDKICK = 8,
    KDONTKICK = 9,
    KWAITFORTURN = 10,
    KGOALIE = 11

};

class CSkillKick : public CSkill
{
private:
    double kkRadLimit;
    kckMode decideMode();
    kckMode kickMode;
    _PID *angPid;
    _PID *speedPid;
    _PID *posPid;


    Vector2D agentPos;
    Vector2D agentDir;
    QQueue <Vector2D> dirQueue;
    double kkDist;
    double kkBallDist;
    double kkRadThresh;
    double kkAngleThresh;
    double kkTAngleThresh;
    double kkKickThresh;
    double kkKickAngThresh;
    double jTurnThr;
    Vector2D oneTouchDir;

    double kkJTurnThresh;
    double kkMovementTheta;
    Vector2D ballPos;
    double ballRealVel;
    AngleDeg kickFinalDir;
    Vector2D finalDirVec;
    Vector2D finalPos;
    void direct();
    void indirect();
    void jTurn();
    void turnForKick();
    void kkKick();
    void kkIntercept();
    void kkPenalty();
    void avoidOppPenalty();
    void avoidOurPenalty();
    void waitAndKick();
    void kDontKick();
    void kWaitForTurn();
    void kgoalie();
    void findPosToGo();
    void findPosToGoAlt();
    double oneTouchAngle(Vector2D pos,Vector2D vel,Vector2D ballVel,Vector2D ballDir,Vector2D goal,double landa,double gamma);
    Vector2D jTurnStartPos;
    bool isJturn;
    bool jTurnFromBack;
    CSkillGotoPointAvoid *gpa;
    Vector2D getTurnFunc(double ang, double _r, double _angOff = 0);



protected:
    CSkillGotoPointAvoid* gotopoint;
    bool lastThroughed;
    int chipWait;
    int chipSpinSpeed;
    bool readyToChip;
    CAgent* agent0;
    bool ballflag;
    bool ballWasFar;
    bool kickerOn;
public:
    DEF_SKILL(CSkillKick);
    void resetI();
    bool kickable();
    Vector2D findMostPossible();
    static double kickTimeEstimation(CAgent * _agent, Vector2D target, const CBall& _ball);
SkillProperty(CSkillKick, double, Tolerance, tol);
SkillProperty(CSkillKick, bool, Chip, chip);
SkillProperty(CSkillKick, int, KickSpeed, kickSpeed);
SkillProperty(CSkillKick, int, Spin, spin);
SkillProperty(CSkillKick, bool, Slow, slow);
SkillProperty(CSkillKick, bool, Turn, turn);
SkillProperty(CSkillKick, bool, AutoChipSpeed, autoChipSpeed);
SkillProperty(CSkillKick, bool, ThroughMode, throughMode);
SkillProperty(CSkillKick, bool, ParallelMode, parallelMode);
SkillProperty(CSkillKick, bool, Clear, clear);
SkillProperty(CSkillKick, int, WaitFrames, waitFrames);
SkillProperty(CSkillKick, bool, AvoidPenaltyArea, avoidPenaltyArea);
SkillProperty(CSkillKick, bool, AvoidOppPenaltyArea, avoidOppPenaltyArea);
SkillProperty(CSkillKick, bool, InterceptMode, interceptMode);
SkillProperty(CSkillKick, bool, DontKick, dontKick);
SkillProperty(CSkillKick, bool, SagMode, sagMode);
SkillProperty(CSkillKick, bool, DontRecvPass, dontRecvPass);
SkillProperty(CSkillKick, bool, RecvChip, recvChip);
SkillProperty(CSkillKick, bool, PenaltyKick, penaltyKick);
SkillProperty(CSkillKick, bool, TurnLastMomentForPenalty, turnLastMomentForPenalty);
SkillPropertyNoSet(CSkillKick, Vector2D, Target, target);
SkillProperty(CSkillKick, bool, IsGotoPointAvoid, isGotoPointAvoid);
SkillProperty(CSkillKick, bool, ShotToEmptySpot, kkShotEmpySpot);
SkillProperty(CSkillKick, bool, PassProfiler, passProfiler);
SkillProperty(CSkillKick, bool, VeryFine, veryFine);
SkillProperty(CSkillKick, bool, GoalieMode, goalieMode);
SkillProperty(CSkillKick, bool, AlternateMode, alternateMode);
SkillProperty(CSkillKick, double,KickAngTol, kickAngTol);
SkillProperty(CSkillKick, bool,KickWithCenterOfDribbler, kickWithCenterOfDribbler);
SkillProperty(CSkillKick, bool,PlayMakeMode, playMakeMode);
SkillProperty(CSkillKick, bool,FastIntercept, fastIntercept);
};




#endif // KICK_H
