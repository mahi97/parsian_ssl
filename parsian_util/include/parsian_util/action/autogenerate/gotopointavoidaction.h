// auto-generated don't edit !

#ifndef GotopointavoidAction_HEADER_
#define GotopointavoidAction_HEADER_


#include <parsian_util/action/autogenerate/gotopointaction.h> 

#include <parsian_util/geom/geom.h>
#include <parsian_msgs/parsian_skill_gotoPointAvoid.h>

class GotopointavoidAction : public GotopointAction  {

public:

    void setMessage(void* _msg);

    void* getMessage();

    static QString getActionName();


    SkillProperty(GotopointavoidAction, bool, Avoidpenaltyarea, avoidPenaltyArea);
    SkillProperty(GotopointavoidAction, bool, Keeplooking, keeplooking);
    SkillProperty(GotopointavoidAction, float, Extendstep, extendStep);
    SkillProperty(GotopointavoidAction, bool, Plan2, plan2);
    SkillProperty(GotopointavoidAction, bool, Noavoid, noAvoid);
    SkillProperty(GotopointavoidAction, bool, Avoidcentercircle, avoidCenterCircle);
    SkillProperty(GotopointavoidAction, float, Ballobstacleradius, ballObstacleRadius);
    SkillProperty(GotopointavoidAction, bool, Avoidball, avoidBall);
    SkillProperty(GotopointavoidAction, bool, Avoidgoalposts, avoidGoalPosts);
    SkillProperty(GotopointavoidAction, bool, Drawpath, drawPath);
    SkillProperty(GotopointavoidAction, Vector2D, Addvel, addVel);
    SkillProperty(GotopointavoidAction, Vector2D, Nextpos, nextPos);

};

#endif // GotopointavoidAction_HEADER_
