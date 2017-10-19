// auto-generated don't edit !

#ifndef KickAction_HEADER_
#define KickAction_HEADER_



#include <parsian_util/action/action.h>
#include <parsian_util/geom/geom.h>
#include <parsian_msgs/parsian_skill_kick.h>

class KickAction : public Action {

public:

    void setMessage(void* _msg);

    void* getMessage();

    SkillProperty(KickAction, quint8, Robot_Id, robot_id);
    SkillProperty(KickAction, double, Tolerance, tolerance);
    SkillProperty(KickAction, bool, Chip, chip);
    SkillProperty(KickAction, qint32, Kickspeed, kickSpeed);
    SkillProperty(KickAction, qint32, Spin, spin);
    SkillProperty(KickAction, bool, Slow, slow);
    SkillProperty(KickAction, bool, Turn, turn);
    SkillProperty(KickAction, bool, Autochipspeed, autoChipSpeed);
    SkillProperty(KickAction, bool, Throughmode, throughMode);
    SkillProperty(KickAction, bool, Parallelmode, parallelMode);
    SkillProperty(KickAction, bool, Clear, clear);
    SkillProperty(KickAction, qint32, Waitframes, waitFrames);
    SkillProperty(KickAction, bool, Avoidpenaltyarea, avoidPenaltyArea);
    SkillProperty(KickAction, bool, Avoidopppenaltyarea, avoidOppPenaltyArea);
    SkillProperty(KickAction, bool, Interceptmode, interceptMode);
    SkillProperty(KickAction, bool, Dontkick, dontKick);
    SkillProperty(KickAction, bool, Sagmode, sagMode);
    SkillProperty(KickAction, bool, Dontrecvpass, dontRecvPass);
    SkillProperty(KickAction, bool, Recvchip, recvChip);
    SkillProperty(KickAction, bool, Penaltykick, penaltyKick);
    SkillProperty(KickAction, bool, Turnlastmomentforpenalty, turnLastMomentForPenalty);
    SkillProperty(KickAction, bool, Isgotopointavoid, isGotoPointAvoid);
    SkillProperty(KickAction, bool, Kkshotempyspot, kkShotEmpySpot);
    SkillProperty(KickAction, bool, Passprofiler, passProfiler);
    SkillProperty(KickAction, bool, Veryfine, veryFine);
    SkillProperty(KickAction, bool, Goaliemode, goalieMode);
    SkillProperty(KickAction, bool, Alternatemode, alternateMode);
    SkillProperty(KickAction, double, Kickangtol, kickAngTol);
    SkillProperty(KickAction, bool, Kickwithcenterofdribbler, kickWithCenterOfDribbler);
    SkillProperty(KickAction, bool, Playmakemode, playMakeMode);
    SkillProperty(KickAction, bool, Fastintercept, fastIntercept);
    SkillProperty(KickAction, Vector2D, Target, target);

};

#endif // KickAction_HEADER_
