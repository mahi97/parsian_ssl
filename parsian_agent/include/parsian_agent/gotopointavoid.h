//
// Created by parsian-ai on 9/29/17.
//

#ifndef PARSIAN_SKILLS_GOTOPOINTAVOID_H
#define PARSIAN_SKILLS_GOTOPOINTAVOID_H

#include <parsian_agent/gotopoint.h>
#include <parsian_util/action/autogenerate/gotopointaction.h>
#include <parsian_util/action/autogenerate/gotopointavoidaction.h>

class CSkillGotoPointAvoid : public CSkillGotoPoint, public GotopointavoidAction
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
};

#endif //PARSIAN_SKILLS_GOTOPOINTAVOID_H
