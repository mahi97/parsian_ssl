//
// Created by parsian-ai on 9/21/17.
//
#include <parsian_util/core/field.h>

CField::CField()
{
//    if(!ros::param::get("/field_width", _FIELD_WIDTH)) _FIELD_WIDTH = 9.0;
    ros::param::get("/field_height"            , _FIELD_HEIGHT);
    ros::param::get("/field_width"           , _FIELD_WIDTH);
    ros::param::get("/field_margin_height"    , _FIELD_MARGIN_HEIGHT);
    ros::param::get("/field_margin_width"     , _FIELD_MARGIN_WIDTH);
    ros::param::get("/field_penalty"          , _FIELD_PENALTY);
    ros::param::get("/goal_width"             , _GOAL_WIDTH);
    ros::param::get("/goal_rad"               , _GOAL_RAD);
    ros::param::get("/goal_depth"      , _GOAL_DEPTH);
    ros::param::get("/penalty_width"    , _PENALTY_WIDTH);
    ros::param::get("/center_circle_rad"             , _CENTER_CIRCLE_RAD);
    ros::param::get("/penalty_area_circle_x"          , _PENALTY_AREA_CIRCLE_X);
    ros::param::get("/penalty_area_circle_rad"      , _PENALTY_AREA_CIRCLE_RAD);

    _MAX_DIST = sqrt(_FIELD_WIDTH * _FIELD_WIDTH + _FIELD_HEIGHT * _FIELD_HEIGHT);


    fCenter = Vector2D(0.0 , 0.0);
    fOurGoal = Vector2D(- _FIELD_WIDTH/2.0 , 0.0);
    fOppGoal = Vector2D(_FIELD_WIDTH/2.0 , 0.0);
    fOurCornerL = Vector2D(- _FIELD_WIDTH/2.0 , _FIELD_HEIGHT/2.0);
    fOurCornerR = Vector2D(- _FIELD_WIDTH/2.0 , - _FIELD_HEIGHT/2.0);
    fOppCornerL = Vector2D(_FIELD_WIDTH/2.0 , _FIELD_HEIGHT/2.0);
    fOppCornerR = Vector2D(_FIELD_WIDTH/2.0 , - _FIELD_HEIGHT/2.0);
    fOurPenalty = Vector2D(_FIELD_PENALTY - _FIELD_WIDTH/2.0 , 0.0);
    fOppPenalty = Vector2D(_FIELD_WIDTH/2.0 - _FIELD_PENALTY , 0.0);
    fOurGoalL = Vector2D( - _FIELD_WIDTH/2.0, _GOAL_WIDTH / 2.0);
    fOurGoalR = Vector2D( - _FIELD_WIDTH/2.0, -_GOAL_WIDTH / 2.0);
    fOppGoalL = Vector2D(  _FIELD_WIDTH/2.0, _GOAL_WIDTH / 2.0);
    fOppGoalR = Vector2D(  _FIELD_WIDTH/2.0, -_GOAL_WIDTH / 2.0);
    fFieldRect = Rect2D(fOurCornerR+Vector2D(-0.005,-0.005),fOppCornerL+Vector2D(+0.005,+0.005));
    fMarginedFieldRect = Rect2D(fOurCornerR+Vector2D(-0.25,-0.25),fOppCornerL+Vector2D(+0.25,+0.25));
    fOurPenaltyRect  = Rect2D(Vector2D(ourGoal().x,ourGoal().y + 1.25),
                              Vector2D(ourGoal().x + _GOAL_RAD,ourGoal().y - 1.25));
    fOppPenaltyRect  = Rect2D(Vector2D(oppGoal().x - _GOAL_RAD,oppGoal().y + 1.25),
                              Vector2D(oppGoal().x ,oppGoal().y - 1.25));
    fOurOneThirdL = Vector2D(-_FIELD_WIDTH / 2.0 + _FIELD_WIDTH / 3.0, _FIELD_HEIGHT / 2.0);
    fOurOneThirdR = Vector2D(-_FIELD_WIDTH / 2.0 + _FIELD_WIDTH / 3.0, -_FIELD_HEIGHT / 2.0);
    fOppOneThirdL = Vector2D(+_FIELD_WIDTH / 2.0 - _FIELD_WIDTH / 3.0, _FIELD_HEIGHT / 2.0);
    fOppOneThirdR = Vector2D(+_FIELD_WIDTH / 2.0 - _FIELD_WIDTH / 3.0, -_FIELD_HEIGHT / 2.0);
}

CField::~CField()
= default;

Vector2D CField::center()
{
    return fCenter;
}
Vector2D CField::ourGoal()
{
    return fOurGoal;
}
Vector2D CField::oppGoal()
{
    return fOppGoal;
}
Vector2D CField::ourCornerL()
{
    return fOurCornerL;
}
Vector2D CField::ourCornerR()
{
    return fOurCornerR;
}
Vector2D CField::oppCornerL()
{
    return fOppCornerL;
}
Vector2D CField::oppCornerR()
{
    return fOppCornerR;
}
Vector2D CField::ourPenalty()
{
    return fOurPenalty;
}
Vector2D CField::oppPenalty()
{
    return fOppPenalty;
}

Vector2D CField::ourGoalL() {
    return fOurGoalL;
}

Vector2D CField::ourGoalR() {
    return fOurGoalR;
}

Vector2D CField::oppGoalL() {
    return fOppGoalL;
}

Vector2D CField::oppGoalR() {
    return fOppGoalR;
}

Rect2D   CField::fieldRect() {
    return fFieldRect;
}

Rect2D   CField::marginedField(){
    return fMarginedFieldRect;
}

bool     CField::isInField(Vector2D point){
    return fFieldRect.contains(point);
}

Rect2D   CField::ourPenaltyRect()
{
    return fOurPenaltyRect;
}

Rect2D   CField::oppPenaltyRect()
{
    return fOppPenaltyRect;
}

Vector2D CField::ourOneThirdL()
{
    return fOurOneThirdL;
}

Vector2D CField::ourOneThirdR()
{
    return fOurOneThirdR;
}

Vector2D CField::oppOneThirdL()
{
    return fOppOneThirdL;
}

Vector2D CField::oppOneThirdR()
{
    return fOppOneThirdR;
}

QList<Vector2D> CField::ourPAreaIntersect(Line2D line)
{
    QList<Vector2D> results;
    results.clear();
    Circle2D c1(fOurGoal + Vector2D(0,-_GOAL_WIDTH/4),_GOAL_RAD);
    Circle2D c2(fOurGoal + Vector2D(0,+_GOAL_WIDTH/4),_GOAL_RAD);
    Segment2D s(fOurGoal + Vector2D(+_GOAL_RAD,-_GOAL_WIDTH/4),fOurGoal + Vector2D(+_GOAL_RAD,+_GOAL_WIDTH/4));
//    draw(c1,0,90,"blue",false);
//    draw(c2,90,180,"blue",false);
//    draw(s,"blue");
    int n;
    Vector2D vSol1,vSol2;
    n = c1.intersection(line,&vSol1,&vSol2);
    if(n==1)
    {
        double th = (vSol1 - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
        {
            results.append(vSol1);
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
        {
            results.append(vSol1);
        }
        th = (vSol2 - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
        {
            results.append(vSol2);
        }
    }

    n = c2.intersection(line,&vSol1,&vSol2);
    if(n==1)
    {
        double th = (vSol1 - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
        {
            results.append(vSol1);
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
        {
            results.append(vSol1);
        }
        th = (vSol2 - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
        {
            results.append(vSol2);
        }
    }

    vSol1 = s.intersection(line);
    if(vSol1.valid())
    {
        results.append(vSol1);
    }

//    for(int i=0; i<results.count(); i++)
//    {
//        draw(Circle2D(results[i],0.05),0,360,"purple",true);
//    }

    return results;
}

QList<Vector2D> CField::ourBigPAreaIntersect(Line2D line, double scale, float bias)
{
    if (scale < 0)
        scale = (0.975+0.16) / 0.975;
    QList<Vector2D> results;
    results.clear();
    Circle2D c1(fOurGoal + Vector2D(0,-_GOAL_WIDTH/4)*scale,_GOAL_RAD*scale + bias);
    Circle2D c2(fOurGoal + Vector2D(0,+_GOAL_WIDTH/4)*scale,_GOAL_RAD*scale + bias);
    Segment2D s(fOurGoal + Vector2D(+_GOAL_RAD + bias,-_GOAL_WIDTH/4)*scale,fOurGoal + Vector2D(+_GOAL_RAD + bias,+_GOAL_WIDTH/4)*scale);
//    draw(c1,0,90,"blue",false);
//    draw(c2,90,180,"blue",false);
//    draw(s,"blue");
    int n;
    Vector2D vSol1,vSol2;
    n = c1.intersection(line,&vSol1,&vSol2);
    if(n==1)
    {
        double th = (vSol1 - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
        {
            results.append(vSol1);
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
        {
            results.append(vSol1);
        }
        th = (vSol2 - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
        {
            results.append(vSol2);
        }
    }

    n = c2.intersection(line,&vSol1,&vSol2);
    if(n==1)
    {
        double th = (vSol1 - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
        {
            results.append(vSol1);
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
        {
            results.append(vSol1);
        }
        th = (vSol2 - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
        {
            results.append(vSol2);
        }
    }

    vSol1 = s.intersection(line);
    if(vSol1.valid())
    {
        results.append(vSol1);
    }

//    for(int i=0; i<results.count(); i++)
//    {
//        draw(Circle2D(results[i],0.05),0,360,"purple",true);
//    }

    return results;
}





QList<Vector2D> CField::ourPAreaIntersect(Segment2D segment)
{
    QList<Vector2D> results;
    results.clear();
    Circle2D c1(fOurGoal + Vector2D(0,-_GOAL_WIDTH/4),_GOAL_RAD);
    Circle2D c2(fOurGoal + Vector2D(0,+_GOAL_WIDTH/4),_GOAL_RAD);
    Segment2D s(fOurGoal + Vector2D(+_GOAL_RAD,-_GOAL_WIDTH/4),fOurGoal + Vector2D(+_GOAL_RAD,+_GOAL_WIDTH/4));
//    draw(c1,0,90,"blue",false);
//    draw(c2,90,180,"blue",false);
//    draw(s,"blue");
    int n;
    Vector2D vSol1,vSol2;
    n = c1.intersection(segment,&vSol1,&vSol2);
    if(n==1)
    {

        double th = (vSol1 - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
        {
            results.append(vSol1);
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
        {
            results.append(vSol1);
        }
        th = (vSol2 - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
        {
            results.append(vSol2);
        }
    }

    n = c2.intersection(segment,&vSol1,&vSol2);
    if(n==1)
    {
        double th = (vSol1 - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
        {
            results.append(vSol1);
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
        {
            results.append(vSol1);
        }
        th = (vSol2 - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
        {
            results.append(vSol2);
        }
    }

    vSol1 = s.intersection(segment);
    if(vSol1.valid())
    {
        results.append(vSol1);
    }

//    for(int i=0; i<results.count(); i++)
//    {
//        draw(Circle2D(results[i],0.05),0,360,"purple",true);
//    }

    return results;
}

QList<Vector2D> CField::ourPAreaIntersect(Circle2D circle)
{
    QList<Vector2D> results;
    results.clear();
    Circle2D c1(fOurGoal + Vector2D(0,-_GOAL_WIDTH/4),_GOAL_RAD);
    Circle2D c2(fOurGoal + Vector2D(0,+_GOAL_WIDTH/4),_GOAL_RAD);
    Segment2D s(fOurGoal + Vector2D(+_GOAL_RAD,-_GOAL_WIDTH/4),fOurGoal + Vector2D(+_GOAL_RAD,+_GOAL_WIDTH/4));
//    draw(c1,0,90,"blue",false);
//    draw(c2,90,180,"blue",false);
//    draw(s,"blue");
    int n;
    Vector2D vSol1,vSol2;
    n = c1.intersection(circle,&vSol1,&vSol2);
    if(n==1)
    {

        double th = (vSol1 - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
        {
            results.append(vSol1);
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
        {
            results.append(vSol1);
        }
        th = (vSol2 - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
        {
            results.append(vSol2);
        }
    }

    n = c2.intersection(circle,&vSol1,&vSol2);
    if(n==1)
    {
        double th = (vSol1 - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
        {
            results.append(vSol1);
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
        {
            results.append(vSol1);
        }
        th = (vSol2 - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
        {
            results.append(vSol2);
        }
    }

    n = circle.intersection(s,&vSol1,&vSol2);
    if(n==1)
    {
        results.append(vSol1);
    }
    if(n==2)
    {
        results.append(vSol1);
        results.append(vSol2);
    }

//    for(int i=0; i<results.count(); i++)
//    {
//        draw(Circle2D(results[i],0.05),0,360,"purple",true);
//    }

    return results;
}

bool CField::isInOurPenaltyArea(Vector2D point)
{
    Circle2D c1(fOurGoal + Vector2D(0,-_GOAL_WIDTH/4),_GOAL_RAD);
    Circle2D c2(fOurGoal + Vector2D(0,+_GOAL_WIDTH/4),_GOAL_RAD);
    Rect2D r(fOurGoal + Vector2D(+0,-_GOAL_WIDTH/4),fOurGoal + Vector2D(+_GOAL_RAD,+_GOAL_WIDTH/4));
//    draw(c1,0,90,"blue",false);
//    draw(c2,90,180,"blue",false);
//    draw(r,"blue");
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

QList<Vector2D> CField::oppPAreaIntersect(Line2D line)
{
    QList<Vector2D> results;
    results.clear();
    Circle2D c1(fOppGoal + Vector2D(0,-_GOAL_WIDTH/4),_GOAL_RAD);
    Circle2D c2(fOppGoal + Vector2D(0,+_GOAL_WIDTH/4),_GOAL_RAD);
    Segment2D s(fOppGoal + Vector2D(-_GOAL_RAD,-_GOAL_WIDTH/4),fOppGoal + Vector2D(-_GOAL_RAD,+_GOAL_WIDTH/4));
//    draw(c1,270,360,"blue",false);
//    draw(c2,180,270,"blue",false);
//    draw(s,"blue");
    int n;
    Vector2D vSol1,vSol2;
    n = c1.intersection(line,&vSol1,&vSol2);
    if(n==1)
    {

        double th = (vSol1 - c1.center()).th().degree();
        if( (th < -90) && (th > -180) )
        {
            results.append(vSol1);
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c1.center()).th().degree();
        if( (th < -90) && (th > -180) )
        {
            results.append(vSol1);
        }
        th = (vSol2 - c1.center()).th().degree();
        if( (th < -90) && (th > -180) )
        {
            results.append(vSol2);
        }
    }

    n = c2.intersection(line,&vSol1,&vSol2);
    if(n==1)
    {
        double th = (vSol1 - c2.center()).th().degree();
        if( (th > 90) && (th < 180) )
        {
            results.append(vSol1);
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c2.center()).th().degree();
        if( (th > 90) && (th < 180) )
        {
            results.append(vSol1);
        }
        th = (vSol2 - c2.center()).th().degree();
        if( (th > 90) && (th < 180) )
        {
            results.append(vSol2);
        }
    }

    vSol1 = s.intersection(line);
    if(vSol1.valid())
    {
        results.append(vSol1);
    }

//    for(int i=0; i<results.count(); i++)
//    {
//        draw(Circle2D(results[i],0.05),0,360,"purple",true);
//    }

    return results;
}



QList<Vector2D> CField::oppPAreaIntersect(Segment2D segment)
{
    QList<Vector2D> results;
    results.clear();
    Circle2D c1(fOppGoal + Vector2D(0,-_GOAL_WIDTH/4),_GOAL_RAD);
    Circle2D c2(fOppGoal + Vector2D(0,+_GOAL_WIDTH/4),_GOAL_RAD);
    Segment2D s(fOppGoal + Vector2D(-_GOAL_RAD,-_GOAL_WIDTH/4),fOppGoal + Vector2D(-_GOAL_RAD,+_GOAL_WIDTH/4));
//    draw(c1,270,360,"blue",false);
//    draw(c2,180,270,"blue",false);
//    draw(s,"blue");
    int n;
    Vector2D vSol1,vSol2;
    n = c1.intersection(segment,&vSol1,&vSol2);
    if(n==1)
    {

        double th = (vSol1 - c1.center()).th().degree();
        if( (th < -90) && (th > -180) )
        {
            results.append(vSol1);
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c1.center()).th().degree();
        if( (th < -90) && (th > -180) )
        {
            results.append(vSol1);
        }
        th = (vSol2 - c1.center()).th().degree();
        if( (th < -90) && (th > -180) )
        {
            results.append(vSol2);
        }
    }

    n = c2.intersection(segment,&vSol1,&vSol2);
    if(n==1)
    {
        double th = (vSol1 - c2.center()).th().degree();
        if( (th > 90) && (th < 180) )
        {
            results.append(vSol1);
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c2.center()).th().degree();
        if( (th > 90) && (th < 180) )
        {
            results.append(vSol1);
        }
        th = (vSol2 - c2.center()).th().degree();
        if( (th > 90) && (th < 180) )
        {
            results.append(vSol2);
        }
    }

    vSol1 = s.intersection(segment);
    if(vSol1.valid())
    {
        results.append(vSol1);
    }

//    for(int i=0; i<results.count(); i++)
//    {
//        draw(Circle2D(results[i],0.05),0,360,"purple",true);
//    }

    return results;
}

QList<Vector2D> CField::oppPAreaIntersect(Circle2D circle)
{
    QList<Vector2D> results;
    results.clear();
    Circle2D c1(fOppGoal + Vector2D(0,-_GOAL_WIDTH/4),_GOAL_RAD);
    Circle2D c2(fOppGoal + Vector2D(0,+_GOAL_WIDTH/4),_GOAL_RAD);
    Segment2D s(fOppGoal + Vector2D(-_GOAL_RAD,-_GOAL_WIDTH/4),fOppGoal + Vector2D(-_GOAL_RAD,+_GOAL_WIDTH/4));
//    draw(c1,270,360,"blue",false);
//    draw(c2,180,270,"blue",false);
//    draw(s,"blue");
    int n;
    Vector2D vSol1,vSol2;
    n = c1.intersection(circle,&vSol1,&vSol2);
    if(n==1)
    {

        double th = (vSol1 - c1.center()).th().degree();
        if( (th < -90) && (th > -180) )
        {
            results.append(vSol1);
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c1.center()).th().degree();
        if( (th < -90) && (th > -180) )
        {
            results.append(vSol1);
        }
        th = (vSol2 - c1.center()).th().degree();
        if( (th < -90) && (th > -180) )
        {
            results.append(vSol2);
        }
    }

    n = c2.intersection(circle,&vSol1,&vSol2);
    if(n==1)
    {
        double th = (vSol1 - c2.center()).th().degree();
        if( (th > 90) && (th < 180) )
        {
            results.append(vSol1);
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c2.center()).th().degree();
        if( (th > 90) && (th < 180) )
        {
            results.append(vSol1);
        }
        th = (vSol2 - c2.center()).th().degree();
        if( (th > 90) && (th < 180) )
        {
            results.append(vSol2);
        }
    }

    n = circle.intersection(s,&vSol1,&vSol2);
    if(n==1)
    {
        results.append(vSol1);
    }
    if(n==2)
    {
        results.append(vSol1);
        results.append(vSol2);
    }

//    for(int i=0; i<results.count(); i++)
//    {
//        draw(Circle2D(results[i],0.05),0,360,"purple",true);
//    }

    return results;
}

bool CField::isInOppPenaltyArea(Vector2D point)
{
    Circle2D c1(fOppGoal + Vector2D(0,-_GOAL_WIDTH/4),_GOAL_RAD);
    Circle2D c2(fOppGoal + Vector2D(0,+_GOAL_WIDTH/4),_GOAL_RAD);
    Rect2D r(fOppGoal + Vector2D(-0,-_GOAL_WIDTH/4),fOppGoal + Vector2D(-_GOAL_RAD,+_GOAL_WIDTH/4));
    Rect2D Back(fOppGoal + Vector2D(-0,-_GOAL_WIDTH/2) + Vector2D(-0,-0.75),fOppGoal + Vector2D(0.05,+_GOAL_WIDTH/2)+Vector2D(-0,+0.75));
//    draw(c1,270,360,"blue",false);
//    draw(c2,180,270,"blue",false);
//    draw(r,"blue");
//
//    draw(Back,"red");
    if (r.contains(point))
        return true;
    if (c1.contains(point))
    {
        double th = (point - c1.center()).th().degree();
        if( (th < -90) && (th > -180) )
            return true;
    }
    if (c2.contains(point))
    {
        double th = (point - c2.center()).th().degree();
        if( (th > 90) && (th < 180) )
            return true;
    }
    return Back.contains(point);
}

Rect2D   CField::getRegion(Region region, double k)
{
    Rect2D tmp;
    k *= 0.5;
    double d = 0.33;
    switch (region)
    {
        case Region::LargeOppCornerTop   : return Rect2D(Vector2D( _FIELD_WIDTH*k, _FIELD_HEIGHT*0.5)*2.0, Vector2D( _FIELD_WIDTH*0.5, _GOAL_WIDTH*0.5 + d)*2.0);
        case Region::LargeOppCornerBottom: return Rect2D(Vector2D( _FIELD_WIDTH*k,-_FIELD_HEIGHT*0.5)*2.0, Vector2D( _FIELD_WIDTH*0.5,-_GOAL_WIDTH*0.5 - d)*2.0);
        case Region::LargeOppCornerBottomLeft : tmp = getRegion(Region::LargeOppCornerBottom);tmp.setLength(tmp.size().length()*0.5);return tmp;
        case Region::LargeOppCornerTopLeft    : tmp = getRegion(Region::LargeOppCornerTop);tmp.setLength(tmp.size().length()*0.5);return tmp;
        case Region::LargeOppCornerBottomRight: tmp = getRegion(Region::LargeOppCornerBottom);tmp.setLength(tmp.size().length()*0.5);tmp.assign(tmp.topRight(), tmp.size());return tmp;
        case Region::LargeOppCornerTopRight   : tmp = getRegion(Region::LargeOppCornerTop);tmp.setLength(tmp.size().length()*0.5);tmp.assign(tmp.topRight(), tmp.size());return tmp;
        case Region::OppCornerTop   : return Rect2D(Vector2D( _FIELD_WIDTH*k, _FIELD_HEIGHT*0.5), Vector2D( _FIELD_WIDTH*0.5, _GOAL_WIDTH*0.5 + d));
        case Region::OppCornerBottom: return Rect2D(Vector2D( _FIELD_WIDTH*k,-_FIELD_HEIGHT*0.5), Vector2D( _FIELD_WIDTH*0.5,-_GOAL_WIDTH*0.5 - d));
        case Region::OurCornerBottom: return Rect2D(Vector2D(-_FIELD_WIDTH*k,-_FIELD_HEIGHT*0.5), Vector2D(-_FIELD_WIDTH*0.5,-_GOAL_WIDTH*0.5 - d));
        case Region::OurCornerTop   : return Rect2D(Vector2D(-_FIELD_WIDTH*k, _FIELD_HEIGHT*0.5), Vector2D(-_FIELD_WIDTH*0.5, _GOAL_WIDTH*0.5 + d));
        case Region::MidFieldTop    : return Rect2D(Vector2D(-_FIELD_WIDTH*k, _FIELD_HEIGHT*0.5), Vector2D(+_FIELD_WIDTH*k, _GOAL_WIDTH*0.5 + d));
        case Region::MidFieldBottom : return Rect2D(Vector2D(+_FIELD_WIDTH*k,-_FIELD_HEIGHT*0.5), Vector2D(-_FIELD_WIDTH*k,-_GOAL_WIDTH*0.5 - d));
        case Region::FrontGoalOpp   : return Rect2D(Vector2D(+_FIELD_WIDTH*k,+_GOAL_WIDTH*0.5 + d), Vector2D(_FIELD_WIDTH*0.5-0.5,-_GOAL_WIDTH*0.5 - d));
        case Region::FrontGoalOur   : return Rect2D(Vector2D(-_FIELD_WIDTH*0.5+1.5, +_GOAL_WIDTH*0.5 + d), Vector2D(-_FIELD_WIDTH*0.5+2.5,-_GOAL_WIDTH*0.5 - d));
        case Region::Center         : return Rect2D(Vector2D(-_FIELD_WIDTH*k, +_GOAL_WIDTH*0.5 + d), Vector2D(+_FIELD_WIDTH*k,-_GOAL_WIDTH*0.5 - d));
        case Region::BigCenter      : return Rect2D(Vector2D(-1.5,-1.5), Vector2D(1.5, 1.5));
        case Region::OurPenalty     : return fOurPenaltyRect;
        case Region::OppPenalty     : return fOppPenaltyRect;
        case Region::OurField       : return Rect2D(Vector2D(-_FIELD_WIDTH*0.5, _FIELD_HEIGHT*0.5), Vector2D(0.0,-_FIELD_HEIGHT*0.5));
        case Region::OppField       : return Rect2D(Vector2D(0.0, _FIELD_HEIGHT*0.5), Vector2D(_FIELD_WIDTH*0.5,-_FIELD_HEIGHT*0.5));
        case Region::OppCornerBottomLeft : tmp = getRegion(Region::OppCornerBottom);tmp.setLength(tmp.size().length()*0.5);return tmp;
        case Region::OppCornerTopLeft    : tmp = getRegion(Region::OppCornerTop);tmp.setLength(tmp.size().length()*0.5);return tmp;
        case Region::OppCornerBottomRight: tmp = getRegion(Region::OppCornerBottom);tmp.setLength(tmp.size().length()*0.5);tmp.assign(tmp.topRight(), tmp.size());return tmp;
        case Region::OppCornerTopRight   : tmp = getRegion(Region::OppCornerTop);tmp.setLength(tmp.size().length()*0.5);tmp.assign(tmp.topRight(), tmp.size());return tmp;
        case Region::OppCornerPointTop    : return Rect2D(Vector2D(oppCornerL().x-0.45, oppCornerL().y - 0.45) , oppCornerL());
        case Region::OppCornerPointBottom: return Rect2D(Vector2D(oppCornerR().x-0.45, oppCornerR().y + 0.45) , oppCornerR());
        case Region::OurMidFieldTop      : tmp = getRegion(Region::MidFieldTop);tmp.setLength(tmp.size().length()*0.5);return tmp;
        case Region::OurMidFieldBottom   : tmp = getRegion(Region::MidFieldBottom);tmp.setLength(tmp.size().length()*0.5);return tmp;
        case Region::OppMidFieldTop      : tmp = getRegion(Region::MidFieldTop);tmp.setLength(tmp.size().length()*0.5);tmp.assign(tmp.topRight(), tmp.size());return tmp;
        case Region::OppMidFieldBottom   : tmp = getRegion(Region::MidFieldBottom);tmp.setLength(tmp.size().length()*0.5);tmp.assign(tmp.topRight(), tmp.size());return tmp;
        case Region::OppFieldBottom      : return Rect2D(Vector2D(0.0, 0.0), Vector2D(_FIELD_WIDTH*0.5,-_FIELD_HEIGHT*0.5));
        case Region::OppFieldTop         : return Rect2D(Vector2D(0.0, _FIELD_HEIGHT*0.5), Vector2D(_FIELD_WIDTH*0.5,0.0));
        case Region::OurFieldBottom      : return Rect2D(Vector2D(-_FIELD_WIDTH*0.5, 0.0), Vector2D(0.0,-_FIELD_HEIGHT*0.5));
        case Region::OurFieldTop         : return Rect2D(Vector2D(-_FIELD_WIDTH*0.5, _FIELD_HEIGHT*0.5), Vector2D(0.0,0.0));
        case Region::OurMidFieldTopMost    : tmp = getRegion(Region::MidFieldTop);tmp.setLength(tmp.size().length()*0.5);tmp.setWidth(tmp.size().width()*0.5);tmp.assign(tmp.topLeft(), tmp.size());tmp.setTopLeft(tmp.topLeft()+Vector2D(-0.3,0.0));return tmp;
        case Region::OurMidFieldBottomMost : tmp = getRegion(Region::MidFieldBottom);tmp.setLength(tmp.size().length()*0.5);tmp.setWidth(tmp.size().width()*0.5);tmp.assign(tmp.bottomLeft(), tmp.size());tmp.setTopLeft(tmp.topLeft()+Vector2D(-0.3,0.0));return tmp;
        case Region::Field1stQuarter     : return Rect2D(Vector2D(-_FIELD_WIDTH*0.5, _FIELD_HEIGHT*0.5), Vector2D(-_FIELD_WIDTH*0.25,-_FIELD_HEIGHT*0.5));
        case Region::Field2ndQuarter     : return Rect2D(Vector2D(-_FIELD_WIDTH*0.25, _FIELD_HEIGHT*0.5), Vector2D(0.0,-_FIELD_HEIGHT*0.5));
        case Region::Field3rdQuarter     : return Rect2D(Vector2D(0.0, _FIELD_HEIGHT*0.5), Vector2D(_FIELD_WIDTH*0.25,-_FIELD_HEIGHT*0.5));
        case Region::Field4thQuarter     : return Rect2D(Vector2D(_FIELD_WIDTH*0.25, _FIELD_HEIGHT*0.5), Vector2D(_FIELD_WIDTH*0.5,-_FIELD_HEIGHT*0.5));
        case Region::OurMidFieldTopWing  : return Rect2D(Vector2D(-0.2, _FIELD_HEIGHT*0.5), Vector2D(-0.1,1.4));
        case Region::OurMidFieldBottomWing  : return Rect2D(Vector2D(-0.2, -_FIELD_HEIGHT*0.5), Vector2D(-0.1,-1.4));
        case Region::OurCornerBottomLeft : tmp = getRegion(Region::OurCornerBottom);tmp.setLength(tmp.size().length()*0.5);return tmp;
        case Region::OurCornerTopLeft    : tmp = getRegion(Region::OurCornerTop);tmp.setLength(tmp.size().length()*0.5);return tmp;
        case Region::OurCornerBottomRight: tmp = getRegion(Region::OurCornerBottom);tmp.setLength(tmp.size().length()*0.5);tmp.assign(tmp.topRight(), tmp.size());return tmp;
        case Region::OurCornerTopRight   : tmp = getRegion(Region::OurCornerTop);tmp.setLength(tmp.size().length()*0.5);tmp.assign(tmp.topRight(), tmp.size());return tmp;
        case Region::OurCornerPointTop    : return Rect2D(ourCornerL() , Vector2D(ourCornerL().x+0.45, ourCornerL().y - 0.45));
        case Region::OurCornerPointBottom: return Rect2D(Vector2D(ourCornerR().x, ourCornerR().y + 0.45) , Vector2D(ourCornerR().x+0.45, ourCornerR().y));


            // For Mani
        case Region::OurDeffenseOneThird : return Rect2D(Vector2D( (-_FIELD_WIDTH/2.0)-0.2 , (-_FIELD_HEIGHT/2.0)-0.2), Vector2D( (-_FIELD_WIDTH/2.0)+(_FIELD_WIDTH/3.0) , (_FIELD_HEIGHT/2.0)+0.2));
        case Region::OurAttackOneThird   : return Rect2D(Vector2D( (_FIELD_WIDTH/2.0)+(-_FIELD_WIDTH/3.0) , (-_FIELD_HEIGHT/2.0)-0.2), Vector2D( (_FIELD_WIDTH/2.0)+0.2 , (_FIELD_HEIGHT/2.0)+0.2));
        case Region::OurMidOneThird      : return Rect2D(Vector2D( (-_FIELD_WIDTH/2.0)+(_FIELD_WIDTH/3.0) , (-_FIELD_HEIGHT/2.0)-0.2) , Vector2D( (_FIELD_WIDTH/2.0)+(-_FIELD_WIDTH/3.0) , (_FIELD_HEIGHT/2.0)+0.2));
        case Region::OppMidField         : return Rect2D(Vector2D(0, -_GOAL_WIDTH*0.5 - d), Vector2D(+_FIELD_WIDTH*k, _GOAL_WIDTH*0.5 + d));
        case Region::OppCornerLineTop    : return Rect2D(Vector2D(_FIELD_WIDTH/2.0 - 0.55, 1.2), Vector2D(_FIELD_WIDTH/2.0 - 0.4, 0.95));
        case Region::OppCornerLineBottom : return Rect2D(Vector2D(_FIELD_WIDTH/2.0 - 0.55, -0.95), Vector2D(_FIELD_WIDTH/2.0 - 0.4, -1.2));
        case Region::FieldGrid1Top       : return Rect2D(Vector2D( -1.5, _FIELD_HEIGHT/2.0), Vector2D( -0.75, 0.5));
        case Region::FieldGrid1Center    : return Rect2D(Vector2D( -1.5, 0.5), Vector2D( -0.75, -0.5));
        case Region::FieldGrid1Bottom    : return Rect2D(Vector2D( -1.5, -0.5), Vector2D( -0.75, -_FIELD_HEIGHT/2.0));
        case Region::FieldGrid2Top       : return Rect2D(Vector2D( -0.75, _FIELD_HEIGHT/2.0), Vector2D( 0.0, 0.5));
        case Region::FieldGrid2Center    : return Rect2D(Vector2D( -0.75, 0.5), Vector2D( 0.0, -0.5));
        case Region::FieldGrid2Bottom    : return Rect2D(Vector2D( -0.75, -0.5), Vector2D( 0.0, -_FIELD_HEIGHT/2.0));
        case Region::FieldGrid3Top       : return Rect2D(Vector2D( 0.0, _FIELD_HEIGHT/2.0), Vector2D( 0.75, 0.5));
        case Region::FieldGrid3Center    : return Rect2D(Vector2D( 0.0, 0.5), Vector2D( 0.75, -0.5));
        case Region::FieldGrid3Bottom    : return Rect2D(Vector2D( 0.0, -0.5), Vector2D( 0.75, -_FIELD_HEIGHT/2.0));
        case Region::FieldGrid4Top       : return Rect2D(Vector2D( 0.75, _FIELD_HEIGHT/2.0), Vector2D( 1.5, 0.5));
        case Region::FieldGrid4Center    : return Rect2D(Vector2D( 0.75, 0.5), Vector2D( 1.5, -0.5));
        case Region::FieldGrid4Bottom    : return Rect2D(Vector2D( 0.75, -0.5), Vector2D( 1.5, -_FIELD_HEIGHT/2.0));
        case Region::FieldGrid5Top       : return Rect2D(Vector2D( 1.5, _FIELD_HEIGHT/2.0), Vector2D( 2.25, 0.5));
        case Region::FieldGrid5Center    : return Rect2D(Vector2D( 1.5, 0.5), Vector2D( 2.25, -0.5));
        case Region::FieldGrid5Bottom    : return Rect2D(Vector2D( 1.5, -0.5), Vector2D( 2.25, -_FIELD_HEIGHT/2.0));
        case Region::TheirPenaltyTop     : return Rect2D(Vector2D(2.38349,1.33186),Vector2D(2.71047,1.06512));
        case Region::TheirPenaltyBottom  : return Rect2D(Vector2D(2.38349,-1.33186),Vector2D(2.71047,-1.06512));
        case Region::AttackRecCornerTopB : return Rect2D(Vector2D(2.2,oppCornerL().y),Vector2D(oppGoal().x,0.35));
        case Region::AttackRecCornerTopP1 : return Rect2D(Vector2D(1.3,-0.2),Vector2D(2.9,-1.7));
        case Region::AttackRecCornerTopP2 : return Rect2D(Vector2D(0,1.8),Vector2D(2,0));
        case Region::AttackRecCornerBottomB : return Rect2D(Vector2D(2.2,-0.35),oppCornerR());
        case Region::AttackRecCornerBottomP1 : return Rect2D(Vector2D(1.3,1.9),Vector2D(2.7,0.2));
        case Region::AttackRecCornerBottomp2 : return Rect2D(Vector2D(0,0),Vector2D(2,-1.8));
        case Region::AttackRecMidTopB   : return Rect2D(Vector2D(0.9,oppCornerL().y),Vector2D(2.2,0));
        case Region::AttackRecMidTopP1  : return Rect2D(Vector2D(1.8,0),Vector2D(oppGoal().x-0.1, oppCornerR().y+0.45));
        case Region::AttackRecMidTopP2  : return Rect2D(Vector2D(0,-0.3),Vector2D(1.5,-1.8));
        case Region::AttackRecMidBottomB    : return Rect2D(Vector2D(0.9,0),Vector2D(2.2, oppCornerR().y));
        case Region::AttackRecMidBottomP1   : return Rect2D(Vector2D(1.8, oppCornerL().y-0.45),Vector2D(oppGoal().x-0.1, 0));
        case Region::AttackRecMidBottomp2   : return Rect2D(Vector2D(0,1.8),Vector2D(1.5,0.3));
        case Region::TC2012Top1 : return Rect2D(Vector2D(-3 , 2),Vector2D(-1.8 , 0.8));
        case Region::TC2012Top2 : return Rect2D(Vector2D(-1.8 , 2),Vector2D(-0.6 , 0.8));
        case Region::TC2012Top3 : return Rect2D(Vector2D(-0.6 , 2),Vector2D(0.6 , 0.8));
        case Region::TC2012Top4 : return Rect2D(Vector2D(0.6 , 2),Vector2D(2.5 , 0.8));
        case Region::TC2012Bottom1 : return Rect2D(Vector2D(-3 , -0.8),Vector2D(-1.8 , -2));
        case Region::TC2012Bottom2 : return Rect2D(Vector2D(-1.8 , -0.8),Vector2D(-0.6 , -2));
        case Region::TC2012Bottom3 : return Rect2D(Vector2D(-0.6 , -0.8),Vector2D(0.6 , -2));
        case Region::TC2012Bottom4 : return Rect2D(Vector2D(0.6 , -0.8),Vector2D(2.5 , -2));
        case Region::TC2012Rect1 : return Rect2D(Vector2D(0,2) , Vector2D(1.5,0));
        case Region::TC2012Rect2 : return Rect2D(Vector2D(3,2) , Vector2D(1.5,0));
        case Region::TC2012Rect3 : return Rect2D(Vector2D(1.5,0) , Vector2D(0,-2));
        case Region::TC2012Rect4 : return Rect2D(Vector2D(1.5,0) , Vector2D(3,-2));
    }
    return Rect2D(Vector2D::INVALIDATED,Vector2D::INVALIDATED);
}

Rect2D  CField::getCircleRegion(int n, int i)
{
    double theta = 45;
//	if (knowledge->variables.contains("circle.radius")) rad = knowledge->variables["circle.radius"].toDouble();
//    if (knowledge->variables.contains("circle.x")) c.x = knowledge->variables["circle.x"].toDouble();
//    if (knowledge->variables.contains("circle.y")) c.y = knowledge->variables["circle.y"].toDouble();
//    if (knowledge->variables.contains("circle.theta")) theta = knowledge->variables["circle.theta"].toDouble();
//    if (knowledge->variables.contains("circle.diam")) diam.x = diam.y = knowledge->variables["circle.theta"].toDouble();
    Vector2D c(1.5, 0);
    Vector2D diam(0.3, 0.3);
    double rad = 1.5;
    Vector2D s;
    s = c + Vector2D::unitVector(i*360.0/n + theta)*rad;

    //TOF for technical challenge iranopen2012

    if (i==0)
        s.assign(0.5, 1.4);
    else if (i==1)
        s.assign(2.4, 1.4);
    else if (i==2)
        s.assign(2.4, -1.4);
    else if (i==3)
        s.assign(0.5, -1.4);


    return Rect2D{Vector2D(s-diam), Vector2D(s+diam)};
}

Rect2D  CField::getRegion(QString name, double k)
{
    Region r = Region::OppField;
    if      (name=="ourcornertop")              r=Region::OurCornerTop;
    else if (name=="ourcornerbottom")           r=Region::OurCornerBottom;
    else if (name=="oppcornertop")              r=Region::OppCornerTop;
    else if (name=="oppcornerbottom")           r=Region::OppCornerBottom;
    else if (name=="midfieldtop")               r=Region::MidFieldTop;
    else if (name=="midfieldbottom")            r=Region::MidFieldBottom;
    else if (name=="frontgoalopp")              r=Region::FrontGoalOpp;
    else if (name=="frontgoalour")              r=Region::FrontGoalOur;
    else if (name=="center")                    r=Region::Center;
    else if (name=="bigcenter")                 r=Region::BigCenter;
    else if (name=="ourpenalty")                r=Region::OurPenalty;
    else if (name=="opppenalty")                r=Region::OppPenalty;
    else if (name=="ourfield")                  r=Region::OurField;
    else if (name=="oppfield")                  r=Region::OppField;
    else if (name=="oppcornertopright")         r=Region::OppCornerTopRight;
    else if (name=="oppcornertopleft")          r=Region::OppCornerTopLeft;
    else if (name=="oppcornerbottomright")      r=Region::OppCornerBottomRight;
    else if (name=="oppcornerbottomleft")       r=Region::OppCornerBottomLeft;
    else if (name=="ourcornertopright")         r=Region::OurCornerTopRight;
    else if (name=="ourcornertopleft")          r=Region::OurCornerTopLeft;
    else if (name=="ourcornerbottomright")      r=Region::OurCornerBottomRight;
    else if (name=="ourcornerbottomleft")       r=Region::OurCornerBottomLeft;
    else if (name=="ourmidfieldtop")            r=Region::OurMidFieldTop;
    else if (name=="ourmidfieldbottom")         r=Region::OurMidFieldBottom ;
    else if (name=="oppmidfieldtop")            r=Region::OppMidFieldTop;
    else if (name=="oppmidfieldbottom")         r=Region::OppMidFieldBottom;
    else if (name=="oppfieldtop")               r=Region::OppFieldTop;
    else if (name=="oppfieldbottom")            r=Region::OppFieldBottom;
    else if (name=="ourfieldtop")               r=Region::OurFieldTop;
    else if (name=="ourfieldbottom")            r=Region::OurFieldBottom;
    else if (name=="ourmidfieldtopmost")        r=Region::OurMidFieldTopMost;
    else if (name=="ourmidfieldbottommost")     r=Region::OurMidFieldBottomMost;
    else if (name=="field1stquarter")           r=Region::Field1stQuarter;
    else if (name=="field2ndquarter")           r=Region::Field2ndQuarter;
    else if (name=="field3rdquarter")           r=Region::Field3rdQuarter;
    else if (name=="field4thquarter")           r=Region::Field4thQuarter;
    else if (name=="ourmidfieldtopwing")        r=Region::OurMidFieldTopWing;
    else if (name=="ourmidfieldbottomwing")     r=Region::OurMidFieldBottomWing;
    else if (name=="ourattackonethird")         r=Region::OurAttackOneThird;
    else if (name=="ourdeffenseonethird")       r=Region::OurDeffenseOneThird;
    else if (name=="ourmidonethird")            r=Region::OurMidOneThird;
    else if (name=="oppmidfield")               r=Region::OppMidField;
    else if (name=="oppcornerlinetop")          r=Region::OppCornerLineTop;
    else if (name=="oppcornerlinebottom")       r=Region::OppCornerLineBottom;
    else if (name=="fieldgrid1top")             r=Region::FieldGrid1Top;
    else if (name=="fieldgrid1center")          r=Region::FieldGrid1Center;
    else if (name=="fieldgrid1bottom")          r=Region::FieldGrid1Bottom;
    else if (name=="fieldgrid2top")             r=Region::FieldGrid2Top;
    else if (name=="fieldgrid2center")          r=Region::FieldGrid2Center;
    else if (name=="fieldgrid2bottom")          r=Region::FieldGrid2Bottom;
    else if (name=="fieldgrid3top")             r=Region:: FieldGrid3Top;
    else if (name=="fieldgrid3center")          r=Region:: FieldGrid3Center;
    else if (name=="fieldgrid3bottom")          r=Region:: FieldGrid3Bottom;
    else if (name=="fieldgrid4top")             r=Region:: FieldGrid4Top;
    else if (name=="fieldgrid4center")          r=Region:: FieldGrid4Center;
    else if (name=="fieldgrid4bottom")          r=Region:: FieldGrid4Bottom;
    else if (name=="fieldgrid5top")             r=Region:: FieldGrid5Top;
    else if (name=="fieldgrid5center")          r=Region:: FieldGrid5Center;
    else if (name=="fieldgrid5bottom")          r=Region:: FieldGrid5Bottom;
    else if (name=="oppcornerpointtop")         r=Region:: OppCornerPointTop;
    else if (name=="oppcornerpointbottom")      r=Region:: OppCornerPointBottom;
    else if (name=="theirpenaltytop")           r=Region:: TheirPenaltyTop;
    else if (name=="theirpenaltybottom")        r=Region:: TheirPenaltyBottom;
    else if (name=="attreccornertopb")          r=Region::AttackRecCornerTopB;
    else if (name=="attreccornertopp1")         r=Region::AttackRecCornerTopP1;
    else if (name=="attreccornertopp2")         r=Region::AttackRecCornerTopP2;
    else if (name=="attreccornerbottomb")       r=Region::AttackRecCornerBottomB;
    else if (name=="attreccornerbottomp1")      r=Region::AttackRecCornerBottomP1;
    else if (name=="attreccornerbottomp2")      r=Region::AttackRecCornerBottomp2;
    else if (name=="attrecmidtopb")             r=Region::AttackRecMidTopB;
    else if (name=="attrecmidtopp1")            r=Region::AttackRecMidTopP1;
    else if (name=="attrecmidtopp2")            r=Region::AttackRecMidTopP2;
    else if (name=="attrecmidbottomb")          r=Region::AttackRecMidBottomB;
    else if (name=="attrecmidbottomp1")         r=Region::AttackRecMidBottomP1;
    else if (name=="attrecmidbottomp2")         r=Region::AttackRecMidBottomp2;
    else if (name=="tc2012top1")                r=Region::TC2012Top1;
    else if (name=="tc2012top2")                r=Region::TC2012Top2;
    else if (name=="tc2012top3")                r=Region::TC2012Top3;
    else if (name=="tc2012top4")                r=Region::TC2012Top4;
    else if (name=="tc2012bottom1")             r=Region::TC2012Bottom1;
    else if (name=="tc2012bottom2")             r=Region::TC2012Bottom2;
    else if (name=="tc2012bottom3")             r=Region::TC2012Bottom3;
    else if (name=="tc2012bottom4")             r=Region::TC2012Bottom4;
    else if (name=="tc2012rect1")               r=Region::TC2012Rect1;
    else if (name=="tc2012rect2")               r=Region::TC2012Rect2;
    else if (name=="tc2012rect3")               r=Region::TC2012Rect3;
    else if (name=="tc2012rect4")               r=Region::TC2012Rect4;
    else if (name=="ourcornerpointtop")         r=Region::OurCornerPointTop;
    else if (name=="ourcornerpointbottom")      r=Region::OurCornerPointBottom;
    else if (name=="largeoppcornertopright")    r=Region::LargeOppCornerTopRight;
    else if (name=="largeoppcornertopleft")     r=Region::LargeOppCornerTopLeft;
    else if (name=="largeoppcornerbottomright") r=Region::LargeOppCornerBottomRight;
    else if (name=="largeoppcornerbottomleft")  r=Region::LargeOppCornerBottomLeft;
    else if (name.startsWith("circle"))
    {
        QStringList q = name.right(name.length()-6).split("_");
        if (q.length() == 2)
        {
            bool ok;
            int n = q.first().toInt(&ok);
            if (ok)
            {
                int i = q.last().toInt(&ok);
                if (ok)
                {
                    return getCircleRegion(n, i);
                }
            }
        }
    }
    return getRegion(r, k);
}


Vector2D CField::ourPAreaPerpendicularVector(double angle, Vector2D& intersectpoint)
{
    Line2D l(fOurGoal, AngleDeg(angle));
    Vector2D p = fOurGoal + Vector2D::unitVector(angle)*2.0;
    QList<Vector2D> results;
    QList<Vector2D> perps;
    results.clear();
    perps.clear();
    Circle2D c1(fOurGoal + Vector2D(0,-_GOAL_WIDTH/4),0.500);
    Circle2D c2(fOurGoal + Vector2D(0,+_GOAL_WIDTH/4),0.500);
    Segment2D s(fOurGoal + Vector2D(+0.500,-_GOAL_WIDTH/4),fOurGoal + Vector2D(+0.500,+_GOAL_WIDTH/4));
//    draw(c1,0,90,"blue",false);
//    draw(c2,90,180,"blue",false);
//    draw(s,"blue");
    int n;
    Vector2D vSol1,vSol2;
    n = c1.intersection(l,&vSol1,&vSol2);
    if(n==1)
    {
        double th = (vSol1 - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
        {
            results.append(vSol1);
            perps.append((p-c1.center()).norm());
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
        {
            results.append(vSol1);
            perps.append((p-c1.center()).norm());
        }
        th = (vSol2 - c1.center()).th().degree();
        if( (th < 0) && (th > -90) )
        {
            results.append(vSol2);
            perps.append((p-c1.center()).norm());
        }
    }

    n = c2.intersection(l,&vSol1,&vSol2);
    if(n==1)
    {
        double th = (vSol1 - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
        {
            results.append(vSol1);
            perps.append((p-c2.center()).norm());
        }
    }
    if(n==2)
    {
        double th;
        th = (vSol1 - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
        {
            results.append(vSol1);
            perps.append((p-c2.center()).norm());
        }
        th = (vSol2 - c2.center()).th().degree();
        if( (th > 0) && (th < 90) )
        {
            results.append(vSol2);
            perps.append((p-c2.center()).norm());
        }
    }

    vSol1 = s.intersection(l);
    if(vSol1.valid())
    {
        results.append(vSol1);
        perps.append(Vector2D(1.0, 0.0));
    }

    int k=-1;
    double minDist = 0.0;
    for (int i=0;i<results.count(); i++)
    {
        double dist = (p-results[i]).length();
        if (k==-1 || dist<minDist)
        {
            minDist = dist;
            k = i;
        }
    }
    if (k==-1)
    {
        intersectpoint = fOurGoal + Vector2D(1.0, 0.0)*fOurPenaltyRect.size().length();
        return Vector2D{1.0, 0.0};
    }

    intersectpoint = results[k];
    if (!fieldRect().contains(p))
    {
        if (p.y < 0)
        {
            intersectpoint = Vector2D(fOurGoal + Vector2D(0,-_GOAL_WIDTH/4));
            intersectpoint = (p-intersectpoint).norm() * 0.5 + intersectpoint;
            return Vector2D(0.0, -1.0);
        }

        intersectpoint = Vector2D(fOurGoal + Vector2D(0,_GOAL_WIDTH/4));
        intersectpoint = (p-intersectpoint).norm() * 0.5 + intersectpoint;
        return Vector2D(0.0, 1.0);

    }
    return perps[k];

}

double CField::ourPAreaPos(double angle)
{
    Vector2D p, n;
    n = ourPAreaPerpendicularVector(angle, p);
    if (angle > 90) return _GOAL_WIDTH/4 + fabs(Vector2D::angleBetween(n, Vector2D(0.0, 0.0)).radian()) * 0.50;
    if (angle <-90) return -_GOAL_WIDTH/4 - fabs(Vector2D::angleBetween(n, Vector2D(0.0, 0.0)).radian()) * 0.50;
    if (p.y <= -_GOAL_WIDTH/4)
    {
        return -_GOAL_WIDTH/4 - fabs(Vector2D::angleBetween(n, Vector2D(0.0, 0.0)).radian()) * 0.50;
    }
    if (p.y > -_GOAL_WIDTH/4 && p.y < _GOAL_WIDTH/4 )
    {
        return p.y;
    }
    if (p.y > _GOAL_WIDTH / 4)
    {
        return _GOAL_WIDTH/4 + fabs(Vector2D::angleBetween(n, Vector2D(0.0, 0.0)).radian()) * 0.50;
    }
}
