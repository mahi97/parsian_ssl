// auto-generated don't edit !

#ifndef KickAction_HEADER_
#define KickAction_HEADER_



#include <parsian_util/action/action.h>
#include <parsian_util/geom/geom.h>
#include <parsian_msgs/parsian_skill_kick.h>
#include <list>

class KickAction : public Action {

public:
    KickAction():tolerance(0.0), chip(false), spin(0), slow(false), avoidPenaltyArea(false), avoidOppPenaltyArea(false),
                 interceptMode(false), dontKick(false), sagMode(false), penaltyKick(false), shotEmptySpot(false), passProfiler(false),
                 veryFine(false), goalieMode(false), kickAngTol(0.0), kickWithCenterOfDribbler(false), playMakeMode(false),
                 target(0, 0) {

    }
    void setMessage(const void* _msg);

    void* getMessage();

    QString getActionName() override;
    static QString SActionName();


    SkillProperty(KickAction, double, Tolerance, tolerance);
    SkillProperty(KickAction, bool, Chip, chip);
    SkillProperty(KickAction, double, Kickspeed, kickSpeed);
    SkillProperty(KickAction, int, Spin, spin);
    SkillProperty(KickAction, bool, Slow, slow);
    SkillProperty(KickAction, bool, Avoidpenaltyarea, avoidPenaltyArea);
    SkillProperty(KickAction, bool, Avoidopppenaltyarea, avoidOppPenaltyArea);
    SkillProperty(KickAction, bool, Interceptmode, interceptMode);
    SkillProperty(KickAction, bool, Dontkick, dontKick);
    SkillProperty(KickAction, bool, Sagmode, sagMode);
    SkillProperty(KickAction, bool, Penaltykick, penaltyKick);
    SkillProperty(KickAction, bool, Shotemptyspot, shotEmptySpot);
    SkillProperty(KickAction, bool, Passprofiler, passProfiler);
    SkillProperty(KickAction, bool, Veryfine, veryFine);
    SkillProperty(KickAction, bool, Goaliemode, goalieMode);
    SkillProperty(KickAction, double, Kickangtol, kickAngTol);
    SkillProperty(KickAction, bool, Kickwithcenterofdribbler, kickWithCenterOfDribbler);
    SkillProperty(KickAction, bool, Playmakemode, playMakeMode);
    SkillProperty(KickAction, Vector2D, Target, target);


};

#endif // KickAction_HEADER_
