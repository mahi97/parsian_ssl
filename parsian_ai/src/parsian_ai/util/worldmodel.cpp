#include <parsian_ai/util/worldmodel.h>

WorldModel * wm;

WorldModel::WorldModel() : CWorldModel()
{

}

WorldModel::WorldModel(const parsian_msgs::parsian_world_model &_pwm) : CWorldModel(_pwm)
{

}

WorldModel::~WorldModel() {

}

bool WorldModel::AHZIsInOurPenaltyArea(Vector2D point)
{
    Circle2D c1(field->ourGoal() + Vector2D(0,-field->_GOAL_WIDTH/4),_GOAL_RAD_FOR_GOALKEEPER);
    Circle2D c2(field->ourGoal() + Vector2D(0,+field->_GOAL_WIDTH/4), _GOAL_RAD_FOR_GOALKEEPER);
    Rect2D r(field->ourGoal() + Vector2D(+0,-field->_GOAL_WIDTH/4),field->ourGoal + Vector2D(+_GOAL_RAD_FOR_GOALKEEPER,+field->_GOAL_WIDTH/4));
    drawer->draw(c1,0,90,"blue",false);
    drawer->draw(c2,90,180,"blue",false);
    drawer->draw(r,"blue");
    if (r.contains(point))
        return true;
    if (c1.contains(point))
    {
        double th = (point - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
            return true;
    }
    if (c2.contains(point))
    {
        double th = (point - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
            return true;
    }
    return false;
}
