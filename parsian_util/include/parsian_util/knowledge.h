//
// Created by parsian-ai on 9/21/17.
//

#ifndef PARSIAN_UTIL_KNOWLEDGE_H
#define PARSIAN_UTIL_KNOWLEDGE_H

#include "parsian_util/geom/geom.h"
#include "parsian_util/core/field.h"
#include <parsian_util/core/team.h>

#include <ros/ros.h>

#include <QList>


struct NewFastestToBall
{
    double catch_time;
    bool isFastestOurs;
    QList< pair<double , int> > ourF;
    QList< pair<double , int> > oppF;
    NewFastestToBall(){
        catch_time = 10000;
        isFastestOurs = false;
        ourF.clear();
        oppF.clear();
    }
    int ourFastest(){
        if(ourF.size())
            return ourF.first().second;
        return -1;
    }
    double ourFastestTime(){
        if(ourF.size()){
            return ourF.first().first;
        }
        return -1;
    }
    int oppFastest(){
        if( oppF.size() )
            return oppF.first().second;
        return -1;
    }
    double oppFastestTime(){
        if(oppF.size())
            return oppF.first().first;
        return -1;
    }
};

struct FastestToBall
{
    double catch_time;
    int ourFastest;
    double ourFastestTime;
    int oppFastest;
    double oppFastestTime;
    FastestToBall(){
        catch_time = 1000;
        ourFastest = -1;
        ourFastestTime = -1;
        oppFastest = -1;
        oppFastestTime = -1;
    }
};


struct range {
    double a,b;
};

inline double getangle(double x1,double y1,double x2,double y2)
{
    return atan2(y2-y1,x2-x1);
}

inline double len(double x1,double y1,double x2,double y2)
{
    return hypot(x1-x2, y1-y2);
}

inline double len2(double x1,double y1,double x2,double y2)
{
    return (x1-x2) * (x1-x2) + (y1-y2) * (y1-y2);
}


inline double normalang(double dir)
{
    const double _2PI = 2.0 * M_PI;
    if ( dir < -2.0*M_PI || 2.0*M_PI < dir )
    {
        dir = fmod( dir, _2PI );
    }
    if ( dir < -M_PI)
    {
        dir += 2.0*M_PI;
    }
    if ( dir > M_PI)
    {
        dir -= 2.0*M_PI;
    }
    return dir;
}

inline double normalangabs(double dir)
{
    const double _2PI = 2.0 * M_PI;
    if ( dir < -2.0*M_PI || 2.0*M_PI < dir )
    {
        dir = fmod( dir, _2PI );
    }
    if ( dir < -M_PI)
    {
        dir += 2.0*M_PI;
    }
    if ( dir > M_PI)
    {
        dir -= 2.0*M_PI;
    }
    if (dir < 0) return -dir;
    return dir;
}

class CKnowledge {
public:
    CKnowledge();
    ~CKnowledge();
    
    NewFastestToBall newFastestToBall(double timeStep = 0.1, QList<int> ourList=wm->our.data->activeAgents, QList<int> oppList=wm->opp.data->activeAgents);
	FastestToBall findFastestToBall(QList<int> ourList, QList<int> oppList)




    static double getEmptyAngle(Vector2D p,Vector2D p1, Vector2D p2, QList<Circle2D> obs, double& percent, double &mostOpenAngle, double& biggestAngle, bool oppGoal = true, bool _draw = false);
    static Vector2D getReflectPos(Vector2D goal, double dist, Vector2D _ballpos);
    static int getNearestRobotToPoint(CTeam _team, Vector2D _point);
private:
    static CField field;
};

#endif //PARSIAN_UTIL_KNOWLEDGE_H
