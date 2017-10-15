#ifndef GotoPoint_H
#define GotoPoint_H

#include <parsian_agent/skill.h>
#include <parsian_util/mathtools.h>
#include <parsian_agent/newbangbang.h>
#include <QTime>
#include <QFile>
#include <QDebug>
#include <QFile>
#include <parsian_util/mathtools.h>
#include <parsian_util/core/agent.h>
#include <algorithm>
#include <math.h>
#include <parsian_util/action/autogenerate/gotopointaction.h>

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
class CSkillGotoPoint : public CSkill, public GotopointAction
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
};


#endif // GOTOPOINT_H
