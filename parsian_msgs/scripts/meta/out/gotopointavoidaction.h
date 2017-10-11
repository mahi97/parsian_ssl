// auto-generated don't edit !

#ifndef Gotopointavoid_HEADER_
#define Gotopointavoid_HEADER_


#include <parsian_util/actions/action.h>
#include <parsian_util/geom/geom.h>
#include <parsian_util/actions/gotopointaction.h> 

class GotopointavoidAction : public Action, public gotoPointAction  {

public:
    Gotopointavoid();
    ~Gotopointavoid();

    void setMessage(const parsian_msgs:: _msg) {
            addVel = _msg.addVel;
            avoidPenaltyArea = _msg.avoidPenaltyArea;
            keeplooking = _msg.keeplooking;
            extendStep = _msg.extendStep;
            plan2 = _msg.plan2;
            noAvoid = _msg.noAvoid;
            avoidCenterCircle = _msg.avoidCenterCircle;
            ballObstacleRadius = _msg.ballObstacleRadius;
            avoidBall = _msg.avoidBall;
            avoidGoalPosts = _msg.avoidGoalPosts;
            drawPath = _msg.drawPath;
            nextPos = _msg.nextPos;
            gotoPointAction::setMessage(_msg.base);
    }

    parsian_msgs::Gotopointavoid getMessage() {
        parsian_msgs::Gotopointavoid _msg;
        _msg.base = gotoPointAction::getMessage();
        _msg.addVel = addVel;
        _msg.avoidPenaltyArea = avoidPenaltyArea;
        _msg.keeplooking = keeplooking;
        _msg.extendStep = extendStep;
        _msg.plan2 = plan2;
        _msg.noAvoid = noAvoid;
        _msg.avoidCenterCircle = avoidCenterCircle;
        _msg.ballObstacleRadius = ballObstacleRadius;
        _msg.avoidBall = avoidBall;
        _msg.avoidGoalPosts = avoidGoalPosts;
        _msg.drawPath = drawPath;
        _msg.nextPos = nextPos;

    }

    SkillProperty(Gotopointavoid, Vector2D, Addvel, addVel)
    SkillProperty(Gotopointavoid, bool, Avoidpenaltyarea, avoidPenaltyArea)
    SkillProperty(Gotopointavoid, bool, Keeplooking, keeplooking)
    SkillProperty(Gotopointavoid, float, Extendstep, extendStep)
    SkillProperty(Gotopointavoid, bool, Plan2, plan2)
    SkillProperty(Gotopointavoid, bool, Noavoid, noAvoid)
    SkillProperty(Gotopointavoid, bool, Avoidcentercircle, avoidCenterCircle)
    SkillProperty(Gotopointavoid, float, Ballobstacleradius, ballObstacleRadius)
    SkillProperty(Gotopointavoid, bool, Avoidball, avoidBall)
    SkillProperty(Gotopointavoid, bool, Avoidgoalposts, avoidGoalPosts)
    SkillProperty(Gotopointavoid, bool, Drawpath, drawPath)
    SkillProperty(Gotopointavoid, Vector2D, Nextpos, nextPos)
}

#endif // Gotopointavoid_HEADER_